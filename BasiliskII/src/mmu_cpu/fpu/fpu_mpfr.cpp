#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "exception.h"
#include "fpu/fpu.h"
#include "fpu/mpfr_inline.h"
#include "intop.h"
#include "main.h"
#include "newcpu.h"
#include "op.h"
#include "sysdeps.h"
#include <limits>
#include <math.h>
#include <stdlib.h>
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536 >> 6];
M68881 fpu;

uint64_t read64(uint32_t addr) {
    uint64_t v = read32(addr);
    v = v << 32 | read32(addr + 4);
    return v;
}
void write64(uint32_t addr, uint64_t v) {
    write32(addr, v >> 32);
    write32(addr + 4, v);
}

void init_fpu() {
    mpfr_clear_flags();
    memset(&fpu.FPCR, 0, sizeof(M68881::fpcr_t));
    memset(&fpu.FPSR, 0, sizeof(M68881::fpsr_t));
    mpfr_set_default_prec(64);
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    for(int i = 0; i < 8; ++i) {
        fpu.fp[i].reset();
    }
    fpu.fpiar = 0;
}

void load_single(uint32_t addr, fpvalue &dst) {
    uint32_t rv = read32(addr);
    float v = std::bit_cast<float>(rv);
    if(isnan(v)) {
        dst.set_nan(static_cast<uint64_t>(rv & 0x7fffff) << 40);
    } else {
        dst = v;
    }
}

bool test_nan(fpvalue &v) {
    constexpr uint64_t SNAN_BIT = 1LLU << 62;
    if(!v.is_nan()) {
        return false;
    }
    if(!(v.get_payload() & SNAN_BIT)) {
        // SNAN
        fpu.FPSR.snan = true;
        if(!fpu.FPCR.snan) {
            v.set_payload(v.get_payload() | SNAN_BIT);
        }
    }
    return true;
}

void store_single(uint32_t addr, const fpvalue &src) {
    uint32_t v;
    if(src.is_nan()) {
        v = 0x7FC00000 | (src.get_payload() >> 40);
    } else {
        v = bit_cast<uint32_t>(static_cast<float>(src));
        fpu.FPSR.inex2 = mpfr_inexflag_p();
    }
    write32(addr, v);
}
void load_double(uint32_t addr, fpvalue &dst) {
    uint64_t rv = read64(addr);
    double v = bit_cast<double>(rv);
    if(isnan(v)) {
        dst.set_nan((rv & ((1LLU << 52) - 1)) << 11);
    } else {
        dst = v;
    }
}

void store_double(uint32_t addr, const fpvalue &src) {
    uint64_t v;
    if(src.is_nan()) {
        v = 0x7ff8000000000000LLU | (src.get_payload() >> 11);
    } else {
        v = bit_cast<uint64_t>(static_cast<double>(src));
        fpu.FPSR.inex2 = mpfr_inexflag_p();
    }
    write64(addr, v);
}

void load_ext(uint32_t addr, fpvalue &dst) {
    uint16_t exp_i = read16(addr);
    uint64_t mantissa = read64(addr + 4);
    bool s = ((exp_i >> 15) & 1);
    int exp_v = exp_i & 0x7fff;
    if(exp_v == 0x7fff) {
        if(mantissa == 0) {
            // INF
            dst.set_inf(s);
        } else {
            // NAN
            dst.set_nan(mantissa);
        }
        return;
    }
    dst = fpvalue(s, mantissa, exp_v - 0x3fff);
}

void store_ext(uint32_t addr, const fpvalue &src) {
    if(!src.is_nan()) {
        auto [s, m, e] = src.get_zexp();
        write32(addr, s << 31 | e << 16);
        write64(addr + 4, m);
    } else {
        write32(addr, 0x7fff0000);
        write64(addr + 4, src.get_payload());
    }
}

void load_packed(uint32_t addr, fpvalue &dst) {
    uint32_t v1 = read32(addr);
    uint64_t vd = read32(addr + 4);
    vd = vd << 32 | read32(addr + 8);
    bool sm = (v1 & 1 << 31);
    bool se = v1 & 1 << 30;
    uint8_t yy = v1 >> 28 & 3;
    if(se && yy == 3 && ((v1 >> 16) & 0xfff) == 0xfff) {
        if(vd == 0) {
            dst.set_inf(sm);
            return;
        } else {
            dst.set_nan(vd);
            return;
        }
    }
    if(vd == 0) {
        dst.set_zero(sm);
        return;
    }
    char digit[26] = {0};
    digit[0] = sm ? '-' : '+';
    digit[1] = (v1 & 0xf) + '0';
    digit[2] = '.';
    for(int i = 0; i < 16; ++i) {
        digit[3 + i] = ((vd >> (60 - 4 * i)) & 0xf) + '0';
    }
    digit[19] = 'e';
    digit[20] = se ? '-' : '+';
    digit[21] = ((v1 >> 24) & 0xf) + '0';
    digit[22] = ((v1 >> 20) & 0xf) + '0';
    digit[23] = ((v1 >> 16) & 0xf) + '0';
    dst = digit;
    if(dst.mpfr_ret != 0) {
        fpu.FPSR.inex1 = true;
    }
}

void store_packed(uint32_t addr, const fpvalue &value, int8_t k) {
    bool s = value.signbit();
    if(value.is_nan()) {
        write32(addr, 0x7fff0000);
        write64(addr + 4, value.get_payload());
        return;
    }
    if(value.is_inf()) {
        write32(addr, s << 31 | 0x7fff0000);
        write32(addr + 4, 0);
        write32(addr + 8, 0);
        return;
    }
    if(value.is_zero()) {
        write32(addr, s << 31);
        write32(addr + 4, 0);
        write32(addr + 8, 0);
        return;
    }
    auto tp = value.abs();
    if(k <= 0) {
        auto tmp2 = value.log10(MPFR_RNDZ);
        int mp = static_cast<int64_t>(tmp2);
        k = mp - k + 1;
    }
    // SM-SE-0-0 EEE-XXXD DDDDDDDD DDDDDDDD
    auto [buf, e] = tp.get_str(k);
    e--;
    uint32_t exp = s << 31 | ((e < 0 ? 1 : 0) << 30);
    e = abs(e);
    exp |= (e / 100) << 24 | ((e / 10) % 10) << 20 | (e % 10) << 16 |
           (buf[0] - '0');
    uint64_t mant = 0;
    for(int i = 0; i < k - 1; ++i) {
        mant |= static_cast<uint64_t>(buf[i + 1] - '0') << (60 - 4 * i);
    }
    write32(addr, exp);
    write64(addr + 4, mant);
}

void fpu_checkexception() {
    // update acc flags
    fpu.FPSR.ac_iop |= fpu.FPSR.snan | fpu.FPSR.operr;
    fpu.FPSR.ac_ovfl |= fpu.FPSR.ovfl;
    fpu.FPSR.ac_unfl |= fpu.FPSR.unfl | fpu.FPSR.inex2;
    fpu.FPSR.ac_dz |= fpu.FPSR.dz;
    fpu.FPSR.ac_inex |= fpu.FPSR.inex1 | fpu.FPSR.inex2 | fpu.FPSR.ovfl;
    // raise excpetion
    if(fpu.FPCR.bsun && fpu.FPSR.bsun) {
        return FP_UNCND();
    }
    if(fpu.FPCR.snan && fpu.FPSR.snan) {
        return FP_SNAN();
    }
    if(fpu.FPCR.operr && fpu.FPSR.operr) {
        return FP_OPERR();
    }
    if(fpu.FPCR.ovfl && fpu.FPSR.ovfl) {
        return FP_OVFR();
    }
    if(fpu.FPCR.unfl && fpu.FPSR.unfl) {
        return FP_UNFL();
    }
    if(fpu.FPCR.dz && fpu.FPSR.dz) {
        return FP_DIV0();
    }
    if((fpu.FPCR.inex2 && fpu.FPSR.inex2) ||
       (fpu.FPCR.inex1 && fpu.FPSR.inex1)) {
        return FP_INEX();
    }
}
void to_sgl(fpvalue &d) {
    if(d.is_number()) {
        int omi = mpfr_get_emin();
        int omx = mpfr_get_emax();
        d.set_precision(24);
        mpfr_set_emax(127);
        mpfr_set_emin(-126 - 24 + 1);
        d.mpfr_ret = mpfr_check_range(d.ptr(), d.mpfr_ret,
                                      mpfr_get_default_rounding_mode());
        d.subnormalize();
        fpu.FPSR.ovfl = mpfr_overflow_p();
        fpu.FPSR.unfl = mpfr_underflow_p();
        fpu.FPSR.inex2 = mpfr_inexflag_p();
        // reset range
        mpfr_set_emin(omi);
        mpfr_set_emax(omx);
        d.set_precision(64);
    }
}
void to_dbl(fpvalue &d) {
    if(d.is_number()) {
        int omi = mpfr_get_emin();
        int omx = mpfr_get_emax();
        d.set_precision(53);
        mpfr_set_emax(1023);
        mpfr_set_emin(-1022 - 53 + 1);
        d.mpfr_ret = mpfr_check_range(d.ptr(), d.mpfr_ret,
                                      mpfr_get_default_rounding_mode());
        d.subnormalize();
        fpu.FPSR.ovfl = mpfr_overflow_p();
        fpu.FPSR.unfl = mpfr_underflow_p();
        fpu.FPSR.inex2 = mpfr_inexflag_p();
        // reset range
        mpfr_set_emin(omi);
        mpfr_set_emax(omx);
        d.set_precision(64);
    }
}

void to_ext(fpvalue &d) {
    if(d.is_number()) {
        int omi = mpfr_get_emin();
        int omx = mpfr_get_emax();
        d.set_precision(64);
        mpfr_set_emax(16383);
        mpfr_set_emin(-16383 - 64 + 1);
        d.mpfr_ret = mpfr_check_range(d.ptr(), d.mpfr_ret,
                                      mpfr_get_default_rounding_mode());
        d.subnormalize();
        fpu.FPSR.ovfl = mpfr_overflow_p();
        fpu.FPSR.unfl = mpfr_underflow_p();
        fpu.FPSR.inex2 = mpfr_inexflag_p();
        // reset range
        mpfr_set_emin(omi);
        mpfr_set_emax(omx);
        d.set_precision(64);
    }
}
void fpu_postprocess(fpvalue &d, RND_PREC round = RND_PREC::ROUND_DEF) {
    // rounding
    if(round == RND_PREC::ROUND_DEF) {
        round = fpu.FPCR.prec;
    }
    switch(round) {
    case RND_PREC::ROUND_SGL:
        to_sgl(d);
        break;
    case RND_PREC::ROUND_DBL:
        to_dbl(d);
        break;
    case RND_PREC::ROUND_EXT:
        to_ext(d);
        break;
    default:
        break;
    }
    fpu.FPSR.ovfl |= mpfr_overflow_p();
    fpu.FPSR.unfl |= mpfr_underflow_p();
    fpu.FPSR.dz = mpfr_divby0_p();
    fpu.FPSR.operr = mpfr_nanflag_p() || mpfr_erangeflag_p();
    fpu.FPSR.inex2 = mpfr_inexflag_p();

    // update normal flag
    fpu.FPSR.n = d.signbit();
    fpu.FPSR.z = d.is_zero();
    fpu.FPSR.i = d.is_inf();
    fpu.FPSR.nan = d.is_nan();

    if(fpu.FPSR.operr) {
        d.set_payload();
    }

    fpu_checkexception();
}
using fpu_op_t = void (*)(fpvalue &src, fpvalue &dst);
void fmove_cr(uint8_t op, fpvalue &v) {
    switch(op) {
    case 0:
        v = fpvalue::const_pi(); // M_PI
        break;
    case 0xb: // LOG10(2)
        v = 2.0;
        v = v.log10();
        break;
    case 0xc: // E
        v = 1.0;
        v = v.exp();
        break;
    case 0xd: // log2(e)
        v = 1.0;
        v = v.exp();
        v = v.log2();
        break;
    case 0xe: // log10(e)
        v = 1.0;
        v = v.exp();
        v = v.log10();
        break;
    case 0xf: // 0
        v = 0.0;
        break;
    case 0x30: // loge(2)
        v = fpvalue::const_log2();
        break;
    case 0x31: // loge(10)
        v = 10.0;
        v = v.log();
        break;
    case 0x32: // 1e0
        v = fpvalue::pow10_i(0);
        break;
    case 0x33: // 1e1
        v = fpvalue::pow10_i(1);
        break;
    case 0x34: // 1e2
        v = fpvalue::pow10_i(2);
        break;
    case 0x35: // 1e4
        v = fpvalue::pow10_i(4);
        break;
    case 0x36: // 1e8
        v = fpvalue::pow10_i(8);
        break;
    case 0x37: // 1e16
        v = fpvalue::pow10_i(16);
        break;
    case 0x38: // 1e32
        v = fpvalue::pow10_i(32);
        break;
    case 0x39: // 1e64
        v = fpvalue::pow10_i(64);
        break;
    case 0x3a: // 1e128
        v = fpvalue::pow10_i(128);
        break;
    case 0x3b: // 1e256
        v = fpvalue::pow10_i(256);
        break;
    case 0x3c: // 1e512
        v = fpvalue::pow10_i(512);
        break;
    case 0x3d: // 1e1024
        v = fpvalue::pow10_i(1024);
        break;
    case 0x3e: // 1e2048
        v = fpvalue::pow10_i(2048);
        break;
    case 0x3f: // 1e4096
        v = fpvalue::pow10_i(4096);
        break;
    default:
        v = 0.0;
    }
}
#define OP_F(name) void op_##name(fpvalue &src, fpvalue &dst)
OP_F(fmove_from) { fpu_postprocess(dst = src); }

OP_F(fint) { fpu_postprocess(dst = src.rint()); }

OP_F(fsinh) { fpu_postprocess(dst = src.sinh()); }

OP_F(fintrz) { fpu_postprocess(dst = src.trunc()); }

OP_F(fsqrt) { fpu_postprocess(dst = src.sqrt()); }

OP_F(flognp1) { fpu_postprocess(dst = src.log1p()); }

OP_F(fetoxm1) { fpu_postprocess(dst = src.expm1()); }

OP_F(ftanh) { fpu_postprocess(dst = src.tanh()); }

OP_F(fatan) { fpu_postprocess(dst = src.atan()); }

OP_F(fasin) { fpu_postprocess(dst = src.asin()); }

OP_F(fatanh) { fpu_postprocess(dst = src.atanh()); }

OP_F(fsin) { fpu_postprocess(dst = src.sin()); }

OP_F(ftan) { fpu_postprocess(dst = src.tan()); }

OP_F(fetox) { fpu_postprocess(dst = src.exp()); }

OP_F(ftwotox) { fpu_postprocess(dst = src.exp2()); }
OP_F(ftentox) { fpu_postprocess(dst = src.exp10()); }

OP_F(flogn) { fpu_postprocess(dst = src.log()); }

OP_F(flog10) { fpu_postprocess(dst = src.log10()); }

OP_F(flog2) { fpu_postprocess(dst = src.log2()); }

OP_F(fabs) { fpu_postprocess(dst = src.abs()); }

OP_F(fcosh) { fpu_postprocess(dst = src.cosh()); }

OP_F(fneg) { fpu_postprocess(dst = (-src)); }

OP_F(facos) { fpu_postprocess(dst = src.acos()); }

OP_F(fcos) { fpu_postprocess(dst = src.cos()); }

OP_F(fgetexp) {
    if(src.is_inf()) {
        mpfr_set_erangeflag();
        dst.set_nan();
    } else if(src.is_zero()) {
        dst = src;
    } else {
        dst = static_cast<int64_t>((src.get_exp() - 1));
    }
    fpu_postprocess(dst);
}

OP_F(fgetman) {
    if(src.is_inf()) {
        mpfr_set_erangeflag();
        dst.set_nan();
    } else if(src.is_zero()) {
        dst = src;
    } else {
        std::tie(dst, std::ignore) = src.frexp();
        dst *= fpvalue(2.0);
    }
    fpu_postprocess(dst);
}

OP_F(fdiv) { fpu_postprocess(dst /= src); }

OP_F(fmod) {
    long q;
    std::tie(dst, q) = dst.modquo(src);
    if(dst.is_number()) {
        fpu.FPSR.quotient = (static_cast<int8_t>(q) % 0x40);
    }
    fpu_postprocess(dst);
}

OP_F(fadd) { fpu_postprocess(dst += src); }

OP_F(fmul) { fpu_postprocess(dst *= src); }

OP_F(fsgldiv) {
    dst.set_precision(24);
    src.set_precision(24);
    dst /= src;
    dst.set_precision(64);
    src.set_precision(64);
    fpu_postprocess(dst, RND_PREC::ROUND_SGL);
}

OP_F(frem) {
    long q;
    std::tie(dst, q) = dst.remquo(src);
    if(dst.is_number()) {
        fpu.FPSR.quotient = (static_cast<int8_t>(q) % 0x40);
    }
    fpu_postprocess(dst);
}

OP_F(fscale) {
    if(!src.is_number()) {
        fpu.FPSR.operr = true;
        dst.set_nan();
    } else {
        long exp = mpfr_get_si(src.ptr(), MPFR_RNDZ);
        dst = dst.mul_2(exp);
    }
    fpu_postprocess(dst);
}

OP_F(fsglmul) {
    dst.set_precision(24);
    src.set_precision(24);
    dst *= src;
    dst.set_precision(64);
    src.set_precision(64);
    fpu_postprocess(dst, RND_PREC::ROUND_SGL);
}

OP_F(fsub) { fpu_postprocess(dst -= src); }

template <int i> OP_F(fsincos) {
    if(dst.ptr() != fpu.fp[i].ptr()) {
        std::tie(dst, fpu.fp[i]) = src.sin_cos();
        if(fpu.fp[i].is_nan()) {
            fpu.fp[i].set_payload();
        }
    } else {
        dst = src.sin();
    }
    fpu_postprocess(dst);
}

OP_F(fcmp) {
    fpu.FPSR.nan = (test_nan(src) || test_nan(dst));
    if(!fpu.FPSR.nan) {
        if(src.is_inf() && dst.is_inf()) {
            fpu.FPSR.n = dst.signbit();
            fpu.FPSR.z = src.signbit() == dst.signbit();
        } else {
            int x = dst.cmp(src);
            fpu.FPSR.n = x < 0;
            fpu.FPSR.z = x == 0;
        }
        fpu.FPSR.i = false;
        fpu.FPSR.nan = false;
    }
    fpu_checkexception();
}

OP_F(ftst) {
    if(!(fpu.FPSR.nan = test_nan(src))) {
        fpu.FPSR.n = src.signbit();
        fpu.FPSR.z = src.is_zero();
        fpu.FPSR.i = src.is_inf();
    }
    fpu_checkexception();
}

OP_F(fmove) { fpu_postprocess(dst = src); }

OP_F(fsmove) { fpu_postprocess(dst = src, RND_PREC::ROUND_SGL); }

OP_F(fssqrt) { fpu_postprocess(dst = src.sqrt(), RND_PREC::ROUND_SGL); }

OP_F(fdmove) { fpu_postprocess(dst = src, RND_PREC::ROUND_DBL); }

OP_F(fdsqrt) { fpu_postprocess(dst = src.sqrt(), RND_PREC::ROUND_DBL); }

OP_F(fsabs) { fpu_postprocess(dst = src.abs(), RND_PREC::ROUND_SGL); }

OP_F(fsneg) { fpu_postprocess(dst = -src, RND_PREC::ROUND_SGL); }

OP_F(fdabs) { fpu_postprocess(dst = src.abs(), RND_PREC::ROUND_DBL); }

OP_F(fdneg) { fpu_postprocess(dst = -src, RND_PREC::ROUND_DBL); }

OP_F(fsdiv) { fpu_postprocess(dst /= src, RND_PREC::ROUND_SGL); }

OP_F(fsadd) { fpu_postprocess(dst += src, RND_PREC::ROUND_SGL); }

OP_F(fsmul) { fpu_postprocess(dst *= src, RND_PREC::ROUND_SGL); }

OP_F(fddiv) { fpu_postprocess(dst /= src, RND_PREC::ROUND_DBL); }

OP_F(fdadd) { fpu_postprocess(dst += src, RND_PREC::ROUND_DBL); }

OP_F(fdmul) { fpu_postprocess(dst *= src, RND_PREC::ROUND_DBL); }

OP_F(fssub) { fpu_postprocess(dst -= src, RND_PREC::ROUND_SGL); }

OP_F(fdsub) { fpu_postprocess(dst -= src, RND_PREC::ROUND_DBL); }

static fpu_op_t fpu_op_t_table1[0x80];
static fpu_op_t fpu_op_t_table2[0x80];
void fmove_to(int type, int reg, int d_type, uint8_t k, const fpvalue &value) {
    regs.i_ea = 0;
    switch(d_type) {
    case 0: {
        fpu.FPSR.operr = !value.fits_int32();
        int32_t v = static_cast<int64_t>(value);
        fpu.FPSR.inex2 = mpfr_inexflag_p();
        EA_WRITE32(type, reg, v);
        break;
    }
    case 1: {
        uint32_t addr = EA_Addr(type, reg, 4, true);
        store_single(addr, value);
        break;
    }
    case 2: {
        uint32_t addr = EA_Addr(type, reg, 12, true);
        store_ext(addr, value);
        break;
    }
    case 3: {
        uint32_t addr = EA_Addr(type, reg, 12, true);
        store_packed(addr, value, static_cast<int8_t>(k << 1) >> 1);
        break;
    }
    case 4: {
        fpu.FPSR.operr = !value.fits_int16();
        int16_t v = static_cast<int64_t>(value);
        fpu.FPSR.inex2 = mpfr_inexflag_p();
        EA_WRITE16(type, reg, v);
        break;
    }
    case 5: {
        uint32_t addr = EA_Addr(type, reg, 8, true);
        store_double(addr, value);
        break;
    }
    case 6: {
        fpu.FPSR.operr = !value.fits_int8();
        int8_t v = static_cast<int64_t>(value);
        fpu.FPSR.inex2 = mpfr_inexflag_p();
        EA_WRITE8(type, reg, v);
        break;
    }
    case 7: {
        uint32_t addr = EA_Addr(type, reg, 12, true);
        store_packed(addr, value, regs.d[k >> 4]);
        break;
    }
    }
}

uint32_t get_fpcr() {
    RND_MODE rnd = RND_MODE::ROUND_RN;
    switch(mpfr_get_default_rounding_mode()) {
    case MPFR_RNDN:
        rnd = RND_MODE::ROUND_RN;
        break;
    case MPFR_RNDZ:
        rnd = RND_MODE::ROUND_RZ;
        break;
    case MPFR_RNDD:
        rnd = RND_MODE::ROUND_RM;
        break;
    case MPFR_RNDU:
        rnd = RND_MODE::ROUND_RP;
        break;
    default:
        break;
    }
    return static_cast<int>(rnd) << 4 | static_cast<int>(fpu.FPCR.prec) << 6 |
           fpu.FPCR.inex1 << 8 | fpu.FPCR.inex2 << 9 | fpu.FPCR.dz << 10 |
           fpu.FPCR.unfl << 11 | fpu.FPCR.ovfl << 12 | fpu.FPCR.operr << 13 |
           fpu.FPCR.snan << 14 | fpu.FPCR.bsun << 15;
}

void set_fpcr(uint32_t v) {
    fpu.FPCR.bsun = v >> 15 & 1;
    fpu.FPCR.snan = v >> 14 & 1;
    fpu.FPCR.operr = v >> 13 & 1;
    fpu.FPCR.ovfl = v >> 12 & 1;
    fpu.FPCR.unfl = v >> 11 & 1;
    fpu.FPCR.dz = v >> 10 & 1;
    fpu.FPCR.inex2 = v >> 9 & 1;
    fpu.FPCR.inex1 = v >> 8 & 1;
    fpu.FPCR.prec = static_cast<RND_PREC>(v >> 6 & 3);
    switch(static_cast<RND_MODE>(v >> 4 & 3)) {
    case RND_MODE::ROUND_RN:
        mpfr_set_default_rounding_mode(MPFR_RNDN);
        break;
    case RND_MODE::ROUND_RZ:
        mpfr_set_default_rounding_mode(MPFR_RNDZ);
        break;
    case RND_MODE::ROUND_RM:
        mpfr_set_default_rounding_mode(MPFR_RNDD);
        break;
    case RND_MODE::ROUND_RP:
        mpfr_set_default_rounding_mode(MPFR_RNDU);
        break;
    }
}

uint32_t get_fpsr() {
    return fpu.FPSR.n << 27 | fpu.FPSR.z << 26 | fpu.FPSR.i << 25 |
           fpu.FPSR.nan << 24 | (fpu.FPSR.quotient & 0xff) << 16 |
           fpu.FPSR.bsun << 15 | fpu.FPSR.snan << 14 | fpu.FPSR.operr << 13 |
           fpu.FPSR.ovfl << 12 | fpu.FPSR.unfl << 11 | fpu.FPSR.dz << 10 |
           fpu.FPSR.inex2 << 9 | fpu.FPSR.inex1 << 8 | fpu.FPSR.ac_iop << 7 |
           fpu.FPSR.ac_ovfl << 6 | fpu.FPSR.ac_unfl << 5 | fpu.FPSR.ac_dz << 4 |
           fpu.FPSR.ac_inex << 3;
}
void set_fpsr(uint32_t v) {
    fpu.FPSR.n = v >> 27 & 1;
    fpu.FPSR.z = v >> 26 & 1;
    fpu.FPSR.i = v >> 25 & 1;
    fpu.FPSR.nan = v >> 24 & 1;
    int8_t qut = v >> 16 & 0xff;
    fpu.FPSR.quotient = qut;
    fpu.FPSR.bsun = v >> 15 & 1;
    fpu.FPSR.snan = v >> 14 & 1;
    fpu.FPSR.operr = v >> 13 & 1;
    fpu.FPSR.ovfl = v >> 12 & 1;
    fpu.FPSR.unfl = v >> 11 & 1;
    fpu.FPSR.dz = v >> 10 & 1;
    fpu.FPSR.inex2 = v >> 9 & 1;
    fpu.FPSR.inex1 = v >> 8 & 1;
    fpu.FPSR.ac_iop = v >> 7 & 1;
    fpu.FPSR.ac_ovfl = v >> 6 & 1;
    fpu.FPSR.ac_unfl = v >> 5 & 1;
    fpu.FPSR.ac_dz = v >> 4 & 1;
    fpu.FPSR.ac_inex = v >> 3 & 1;
}

void fmovem_from_mem(uint32_t *addr, int mask) {
    for(int i = 0; i < 8; ++i) {
        if(mask & 1 << (7 - i)) {
            load_ext(*addr, fpu.fp[i]);
            *addr += 12;
        }
    }
}

void fmovem_to_mem(uint32_t addr, int mask) {
    for(int i = 0; i < 8; ++i) {
        if(mask & 1 << (7 - i)) {
            store_ext(addr, fpu.fp[i]);
            addr += 12;
        }
    }
}

void fmovem_to_mem_decr(uint32_t *addr, int mask) {
    for(int i = 7; i >= 0; --i) {
        if(mask & 1 << i) {
            store_ext(*addr -= 12, fpu.fp[i]);
        }
    }
}

void fmove_to_cr(int type, int reg, uint16_t op2) {
    // FMOVE(M) to CR
    uint32_t ea;

    switch(op2 >> 10 & 7) {
    case 0:
        FP_UNDEF();
        break;
    case 1:
        fpu.fpiar = EA_READ32(type, reg);
        break;
    case 2:
        if(type == 1) {
            FP_UNDEF();
        }
        set_fpsr(EA_READ32(type, reg));
        break;
    case 3:
        ea = EA_Addr(type, reg, 8, false);
        set_fpsr(read32(ea));
        fpu.fpiar = read32(ea + 4);
        break;
    case 4:
        if(type == 1) {
            FP_UNDEF();
        }
        set_fpcr(EA_READ16(type, reg));
        break;
    case 5:
        ea = EA_Addr(type, reg, 8, false);
        set_fpcr(read32(ea));
        fpu.fpiar = read32(ea + 4);
        break;
    case 6:
        ea = EA_Addr(type, reg, 8, false);
        set_fpcr(read32(ea));
        set_fpsr(read32(ea + 4));
        break;
    case 7:
        ea = EA_Addr(type, reg, 12, false);
        set_fpcr(read32(ea));
        set_fpsr(read32(ea + 4));
        fpu.fpiar = read32(ea + 8);
        break;
    }
    return;
}

void fmove_from_cr(int type, int reg, uint16_t op2) {
    regs.i_ea = 0;
    uint32_t ea;
    switch(op2 >> 10 & 7) {
    case 0:
        FP_UNDEF();
        break;
    case 1:
        EA_WRITE32(type, reg, fpu.fpiar);
        break;
    case 2:
        if(type == 1) {
            FP_UNDEF();
        }
        EA_WRITE32(type, reg, get_fpsr());
        break;
    case 3:
        ea = EA_Addr(type, reg, 8, true);
        write32(ea, get_fpsr());
        write32(ea + 4, fpu.fpiar);
        break;
    case 4:
        if(type == 1) {
            FP_UNDEF();
        }
        EA_WRITE32(type, reg, get_fpcr());
        break;
    case 5:
        ea = EA_Addr(type, reg, 8, true);
        write32(ea, get_fpcr());
        write32(ea + 4, fpu.fpiar);
        break;
    case 6:
        ea = EA_Addr(type, reg, 8, true);
        write32(ea, get_fpcr());
        write32(ea + 4, get_fpsr());
        break;
    case 7:
        ea = EA_Addr(type, reg, 12, true);
        write32(ea, get_fpcr());
        write32(ea + 4, get_fpsr());
        write32(ea + 8, fpu.fpiar);
        break;
    }
}
OP(fpu_op) {
    mpfr_clear_flags();
    uint16_t op2 = FETCH();
    if(op2 >> 15 & 1) {
        switch(op2 >> 13 & 3) {
        case 0:
            fmove_to_cr(type, reg, op2);
            return;
        case 1:
            // FMOVE(M) from CR
            fmove_from_cr(type, reg, op2);
            return;
        case 2: {
            // FMOVEM FROM MEM
            int mode = op2 >> 11 & 3;
            regs.traced = true;
            uint8_t reglist;
            switch(mode) {
            case 0:
            case 1:
                FP_UNDEF();
                return;
            case 2:
                reglist = op2 & 0xff;
                break;
            case 3:
                reglist = regs.d[op2 >> 4 & 7];
                break;
            }
            if(type == 3) {
                fmovem_from_mem(&regs.a[reg], reglist);
            } else {
                uint32_t v = EA_Addr(type, reg, 0, false);
                fmovem_from_mem(&v, reglist);
            }
            return;
        }
        case 3: {
            // FMOVEM TO MEM
            int mode = op2 >> 11 & 3;
            regs.traced = true;
            switch(mode) {
            case 0:
                if(type != 4) {
                    ILLEGAL_INST();
                    return;
                }
                fmovem_to_mem_decr(&regs.a[reg], op2 & 0xff);
                return;
            case 1:
                if(type != 4) {
                    ILLEGAL_INST();
                    return;
                }
                fmovem_to_mem_decr(&regs.a[reg], regs.d[op2 >> 4 & 7]);
                return;
            case 2: {
                uint32_t addr = EA_Addr(type, reg, 0, true);
                fmovem_to_mem(addr, op2 & 0xff);
                return;
            }
            case 3: {
                uint32_t addr = EA_Addr(type, reg, 0, true);
                fmovem_to_mem(addr, regs.d[op2 >> 4 & 7]);
                return;
            }
            }
        }
            return;
        }
    }
    fpu.fpiar = regs.opc;
    bool rm = op2 >> 14 & 1;
    int src_s = op2 >> 10 & 7;
    int dst_r = op2 >> 7 & 7;
    int opc2 = op2 & 0x7f;
    if(!(op2 >> 13 & 1)) {
        fpvalue src;
        if(rm) {
            switch(src_s) {
            case 0:
                src = static_cast<int64_t>(
                    static_cast<int32_t>(EA_READ32(type, reg)));
                break;
            case 1: {
                uint32_t addr = EA_Addr(type, reg, 4, false);
                load_single(addr, src);
                break;
            }
            case 2: {
                uint32_t addr = EA_Addr(type, reg, 12, false);
                load_ext(addr, src);
                break;
            }
            case 3: {
                uint32_t addr = EA_Addr(type, reg, 12, false);
                load_packed(addr, src);
                break;
            }
            case 4:
                src = static_cast<int64_t>(
                    static_cast<int16_t>(EA_READ16(type, reg)));
                break;
            case 5: {
                uint32_t addr = EA_Addr(type, reg, 8, false);
                load_double(addr, src);
                break;
            }
            case 6:
                src = static_cast<int64_t>(
                    static_cast<int8_t>(EA_READ8(type, reg)));
                break;
            case 7:
                fmove_cr(opc2, fpu.fp[dst_r]);
                fpu_postprocess(fpu.fp[dst_r]);
                return;
            }
        } else {
            src = fpu.fp[src_s];
        }
        mpfr_clear_flags();
        fpu.FPSR.bsun = false;
        fpu.FPSR.operr = false;
        fpu.FPSR.ovfl = false;
        fpu.FPSR.unfl = false;
        fpu.FPSR.dz = false;
        fpu.FPSR.inex1 = false;
        fpu.FPSR.inex2 = false;
        switch(opc2) {
        case 0x38: // FCMP
            op_fcmp(src, fpu.fp[dst_r]);
            break;
        case 0x3A: // FTST
            op_ftst(src, fpu.fp[dst_r]);
            break;
        default:
            if(fpu_op_t_table1[opc2]) {
                if(test_nan(src)) {
                    fpu.fp[dst_r] = src;
                    fpu.FPSR.nan = true;
                    fpu_checkexception();
                } else {
                    fpu_op_t_table1[opc2](src, fpu.fp[dst_r]);
                }
            } else if(fpu_op_t_table2[opc2]) {
                if(test_nan(fpu.fp[dst_r])) {
                    fpu.FPSR.nan = true;
                    fpu_checkexception();
                } else if(test_nan(src)) {
                    fpu.fp[dst_r] = src;
                    fpu.FPSR.nan = true;
                    fpu_checkexception();
                } else {
                    fpu_op_t_table2[opc2](src, fpu.fp[dst_r]);
                }
            } else {
                FP_UNDEF();
            }
        }
    } else {
        // FMOVE TO
        fmove_to(type, reg, src_s, opc2, fpu.fp[dst_r]);
    }
}
bool fpcc(int c) {
    if(c & 0x20) {
        return false;
    }
    if((c & 0x10) && fpu.FPSR.nan) {
        fpu.FPSR.bsun = true;
    }

    switch(c & 0xf) {
    case 0:
        return false; // F
    case 1:
        return fpu.FPSR.z; // EQ
    case 2:
        return !(fpu.FPSR.nan | fpu.FPSR.z | fpu.FPSR.n); // OGT
    case 3:
        return fpu.FPSR.z | !(fpu.FPSR.nan | fpu.FPSR.n); // OGE
    case 4:
        return fpu.FPSR.n & !(fpu.FPSR.nan | fpu.FPSR.z); // OLT
    case 5:
        return fpu.FPSR.z | (fpu.FPSR.n & !fpu.FPSR.nan); // OLE
    case 6:
        return !(fpu.FPSR.nan | fpu.FPSR.z); // OGL
    case 7:
        return !fpu.FPSR.nan; // OR
    case 8:
        return fpu.FPSR.nan; // UN
    case 9:
        return fpu.FPSR.nan | fpu.FPSR.z; // UEQ
    case 10:
        return fpu.FPSR.nan | !(fpu.FPSR.n | fpu.FPSR.z); // UGT
    case 11:
        return fpu.FPSR.nan | fpu.FPSR.z | !fpu.FPSR.n; // UGE
    case 12:
        return fpu.FPSR.nan | (fpu.FPSR.n & !fpu.FPSR.z); // ULT
    case 13:
        return fpu.FPSR.nan | fpu.FPSR.z | fpu.FPSR.n; // ULE
    case 14:
        return !fpu.FPSR.z; // NE
    case 15:
        return true; // T
    }
    return false;
}
OP(fpu_fscc) {
    uint16_t op2 = FETCH();
    bool ret = fpcc(op2 & 0x3f);
    if(type == 1) {
        // FDBcc
        uint32_t pc = regs.pc;
        int16_t disp = FETCH();
        regs.traced = true;
        if(!ret) {
            int16_t v = regs.d[reg] - 1;
            WRITE_D16(reg, v);
            if(v != -1) {
                JUMP(pc + disp);
            }
        }
    } else if(type == 7) {
        // FTRAPcc
        switch(reg) {
        case 2:
            FETCH();
            break;
        case 3:
            FETCH32();
            break;
        case 4:
            break;
        default:
            FP_UNDEF();
            return;
        }
        if(ret) {
            TRPPcc();
        }
    } else {
        // FScc
        regs.i_ea = 0;
        EA_WRITE8(type, reg, ret ? 0xff : 0);
        fpu_checkexception();
    }
}

OP(fbcc_w) {
    int opc = xop & 0x3f;
    uint32_t pc = regs.pc;
    int32_t offset = static_cast<int16_t>(FETCH());
    if(fpcc(opc)) {
        regs.traced = true;
        JUMP(pc + offset);
    }
    fpu_checkexception();
}

OP(fbcc_l) {
    int opc = xop & 0x3f;
    uint32_t pc = regs.pc;
    int32_t offset = FETCH32();
    if(fpcc(opc)) {
        JUMP(pc + offset);
    }
}
OP(fsave) {
    // only idle frame
    if(!regs.S) {
        PRIV_ERROR();
    }
regs.traced = true;
    EA_WRITE32(type, reg, 0x41000000);
}
OP(frestore) {
    if(!regs.S) {
        PRIV_ERROR();
    }
regs.traced = true;
    uint32_t fw = EA_READ32(type, reg);
    if(fw >> 24 == 0) {
        init_fpu();
        return;
    }
    if(fw >> 24 != 0x41) {
        FORMAT_ERROR();
        return;
    }
    // idle frame
    int frame_type = (fw >> 16) & 0xff;
    if(frame_type == 0) {
        return;
    } else if(frame_type == 0x60) {
        // busy
        EA_Addr(type, reg, 0x60, false);
    } else if(frame_type == 0x30) {
        // busy
        EA_Addr(type, reg, 0x30, false);
    } else {
        FORMAT_ERROR();
    }
}

void init_fpu_opc() {
    opc_map[01710] = op_fpu_op;
    opc_map[01711] = op_fpu_fscc;
    opc_map[01712] = op_fbcc_w;
    opc_map[01713] = op_fbcc_l;
    opc_map[01714] = op_fsave;
    opc_map[01715] = op_frestore;

    fpu_op_t_table1[0x00] = op_fmove;
    fpu_op_t_table1[0x01] = op_fint;
    fpu_op_t_table1[0x02] = op_fsinh;
    fpu_op_t_table1[0x03] = op_fintrz;
    fpu_op_t_table1[0x04] = op_fsqrt;
    fpu_op_t_table1[0x06] = op_flognp1;
    fpu_op_t_table1[0x08] = op_fetoxm1;
    fpu_op_t_table1[0x09] = op_ftanh;
    fpu_op_t_table1[0x0A] = op_fatan;
    fpu_op_t_table1[0x0C] = op_fasin;
    fpu_op_t_table1[0x0D] = op_fatanh;
    fpu_op_t_table1[0x0E] = op_fsin;
    fpu_op_t_table1[0x0F] = op_ftan;
    fpu_op_t_table1[0x10] = op_fetox;
    fpu_op_t_table1[0x11] = op_ftwotox;
    fpu_op_t_table1[0x12] = op_ftentox;
    fpu_op_t_table1[0x14] = op_flogn;
    fpu_op_t_table1[0x15] = op_flog10;
    fpu_op_t_table1[0x16] = op_flog2;
    fpu_op_t_table1[0x18] = op_fabs;
    fpu_op_t_table1[0x19] = op_fcosh;
    fpu_op_t_table1[0x1A] = op_fneg;
    fpu_op_t_table1[0x1C] = op_facos;
    fpu_op_t_table1[0x1D] = op_fcos;
    fpu_op_t_table1[0x1E] = op_fgetexp;
    fpu_op_t_table1[0x1F] = op_fgetman;

    fpu_op_t_table1[0x30] = op_fsincos<0>;
    fpu_op_t_table1[0x31] = op_fsincos<1>;
    fpu_op_t_table1[0x32] = op_fsincos<2>;
    fpu_op_t_table1[0x33] = op_fsincos<3>;
    fpu_op_t_table1[0x34] = op_fsincos<4>;
    fpu_op_t_table1[0x35] = op_fsincos<5>;
    fpu_op_t_table1[0x36] = op_fsincos<6>;
    fpu_op_t_table1[0x37] = op_fsincos<7>;

    fpu_op_t_table1[0x40] = op_fsmove;
    fpu_op_t_table1[0x41] = op_fssqrt;
    fpu_op_t_table1[0x44] = op_fdmove;
    fpu_op_t_table1[0x45] = op_fdsqrt;
    fpu_op_t_table1[0x58] = op_fsabs;
    fpu_op_t_table1[0x5A] = op_fsneg;
    fpu_op_t_table1[0x5C] = op_fdabs;
    fpu_op_t_table1[0x5E] = op_fdneg;

    fpu_op_t_table2[0x20] = op_fdiv;
    fpu_op_t_table2[0x21] = op_fmod;
    fpu_op_t_table2[0x22] = op_fadd;
    fpu_op_t_table2[0x23] = op_fmul;
    fpu_op_t_table2[0x24] = op_fsgldiv;
    fpu_op_t_table2[0x25] = op_frem;
    fpu_op_t_table2[0x26] = op_fscale;
    fpu_op_t_table2[0x27] = op_fsglmul;
    fpu_op_t_table2[0x28] = op_fsub;
    fpu_op_t_table2[0x60] = op_fsdiv;
    fpu_op_t_table2[0x62] = op_fsadd;
    fpu_op_t_table2[0x63] = op_fsmul;
    fpu_op_t_table2[0x64] = op_fddiv;
    fpu_op_t_table2[0x66] = op_fdadd;
    fpu_op_t_table2[0x67] = op_fdmul;
    fpu_op_t_table2[0x68] = op_fssub;
    fpu_op_t_table2[0x6C] = op_fdsub;
}
