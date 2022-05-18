#include "fpu/fpu_mpfr.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "exception.h"
#include "intop.h"
#include "main.h"
#include "newcpu.h"
#include "op.h"
#include "sysdeps.h"
#include <fenv.h>
#include <functional>
#include <limits>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536 >> 6];
bool is_snan(float f) { return isnan(f) && (bit_cast<uint32_t>(f) & 1 << 22); }
bool is_snan(double f) {
    return isnan(f) && (bit_cast<uint64_t>(f) & 1LL << 51);
}

uint64_t read64(uint32_t addr) {
    uint64_t v = read32(addr);
    v = v << 32 | read32(addr + 4);
    return v;
}
void write64(uint32_t addr, uint64_t v) {
    write32(addr, v >> 32);
    write32(addr+4, v);
}
void init_fpu() {
    mpfr_clear_flags();
    memset(&regs.fpu.FPCR, 0, sizeof(M68881::fpcr_t));
    memset(&regs.fpu.FPSR, 0, sizeof(M68881::fpsr_t));
    mpfr_set_default_prec(64);
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    for(int i = 0; i < 8; ++i) {
        mpfr_init2(regs.fpu.fp[i], 64);
    }
    regs.fpu.fpiar = 0;
}

void update_round() {}
void load_single(uint32_t addr, mpfr_ptr dst) {
    float v = bit_cast<float>(read32(addr));
    if(is_snan(v)) {
        regs.fpu.FPSR.snan = true;
    }
    mpfr_set_d(dst, v, regs.fpu.rnd_mode);
}

void store_single(uint32_t addr, mpfr_ptr src) {
    uint32_t v = bit_cast<uint32_t>(mpfr_get_flt(src, regs.fpu.rnd_mode));
    regs.fpu.FPSR.inex2 = mpfr_inexflag_p();
    write32(addr, v);
}
void load_double(uint32_t addr, mpfr_ptr dst) {
    double v = bit_cast<double>(read64(addr));
    if(is_snan(v)) {
        regs.fpu.FPSR.snan = true;
    }
    mpfr_set_d(dst, v, regs.fpu.rnd_mode);
}

void store_double(uint32_t addr, mpfr_ptr src) {
    double v = mpfr_get_d(src, regs.fpu.rnd_mode);
    regs.fpu.FPSR.inex2 = mpfr_inexflag_p();
    write64(addr, bit_cast<uint64_t>(v));
}

void load_ext(uint32_t addr, mpfr_ptr dst) {
    uint16_t exp_i = read16(addr);
    uint64_t mantissa = read64(addr + 4);
    bool s = (exp_i >> 15) & 1;
    int exp_v = exp_i & 0x7fff;
    if(exp_v == 0x7fff) {
        if(mantissa == 0) {
            // INF
            mpfr_set_inf(dst, s);
            return;
        } else {
            // NAN
            if(!(mantissa >> 63 & 1)) {
                regs.fpu.FPSR.snan = true;
            }
            mpfr_set_nan(dst);
        }
    }
    mpfr_set_uj_2exp(dst, mantissa, exp_v - 0x3ffe - 64, regs.fpu.rnd_mode);
    mpfr_setsign(dst, dst, s, regs.fpu.rnd_mode);
}

void store_ext(uint32_t addr, mpfr_ptr src) {
    bool s = mpfr_signbit(src);
    MPFR_DECL_INIT(tp, 64);
    mpfr_set(tp, src, regs.fpu.rnd_mode);
    mpfr_abs(tp, tp, regs.fpu.rnd_mode);
    mpz_t rop;
    mpz_init(rop);
    int exp = mpfr_get_z_2exp(rop, tp)+63;
    uint64_t m = mpz_get_ui(rop);
    int e = exp + 0x3fff;
    mpz_clear(rop);

    write32(addr, s << 31 | e << 16);
    write64(addr + 4, m);
}

void load_packed(uint32_t addr, mpfr_ptr dst) {
    uint32_t v1 = read32(addr);
    uint64_t vd = read32(addr + 4);
    vd = vd << 32 | read32(addr + 8);
    double sm = (v1 & 1 << 31) ? -1.0 : 1.0;
    bool se = v1 & 1 << 30;
    uint8_t yy = v1 >> 28 & 3;
    if(se && yy == 3 && ((v1 >> 16) & 0xfff) == 0xfff) {
        if(vd == 0) {
            mpfr_set_inf(dst, sm);
            return;
        } else {
            mpfr_set_nan(dst);
            return;
        }
    }
    if(vd == 0) {
        mpfr_set_zero(dst, sm);
        return;
    }
    char digit[26] = {0};
    digit[0] = sm == -1.0 ? '-' : '+';
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
    mpfr_set_str(dst, digit, 10, regs.fpu.rnd_mode);
}

void store_packed(uint32_t addr, mpfr_ptr value, int8_t k) {
    bool s = mpfr_signbit(value);
    if(mpfr_nan_p(value)) {
        write32(addr, 0x7fff0000);
        write32(addr + 4, 0x80000000);
        write32(addr + 8, 0);
        return;
    }
    if(mpfr_inf_p(value)) {
        write32(addr, s << 31 | 0x7fff0000);
        write32(addr + 4, 0);
        write32(addr + 8, 0);
        return;
    }
    if(mpfr_zero_p(value)) {
        write32(addr, s << 31);
        write32(addr + 4, 0);
        write32(addr + 8, 0);
        return;
    }
    MPFR_DECL_INIT(tp, 64);
    mpfr_set(tp, value, regs.fpu.rnd_mode);
    mpfr_abs(tp, tp, regs.fpu.rnd_mode);
    char buf[64];
    if(k <= 0) {
        MPFR_DECL_INIT(tmp2, 64);
        mpfr_log10(tmp2, value, MPFR_RNDZ);
        int mp = mpfr_get_si(tmp2, MPFR_RNDZ);
        k = mp - k + 1;
    }
    // SM-SE-0-0 EEE-XXXD DDDDDDDD DDDDDDDD
    mpfr_exp_t e;
    mpfr_get_str(buf, &e, 10, k, tp, regs.fpu.rnd_mode);
    e--;
    uint32_t exp = s << 31 | ((e < 0 ? 1 : 0) << 30);
    e = abs(e);
    exp |= (e / 100) << 24 | ((e / 10) % 10) << 20 | (e % 10) << 16 |
           (buf[0] - '0');
    uint64_t mant = 0;
    for(int i = 0; i < k-1; ++i) {
        mant |= static_cast<uint64_t>(buf[i+1] - '0') << (60 - 4 * i);
    }
    write32(addr, exp);
    write64(addr + 4, mant);
}

void fpu_checkexception() {
    // update acc flags
    regs.fpu.FPSR.ac_iop |= regs.fpu.FPSR.snan | regs.fpu.FPSR.operr;
    regs.fpu.FPSR.ac_ovfl |= regs.fpu.FPSR.ovfl;
    regs.fpu.FPSR.ac_unfl |= regs.fpu.FPSR.unfl | regs.fpu.FPSR.inex2;
    regs.fpu.FPSR.ac_dz |= regs.fpu.FPSR.dz;
    regs.fpu.FPSR.ac_inex |=
        regs.fpu.FPSR.inex1 | regs.fpu.FPSR.inex2 | regs.fpu.FPSR.ovfl;
    // raise excpetion
    if(regs.fpu.FPCR.bsun && regs.fpu.FPSR.bsun) {
        return FP_UNCND();
    }
    if(regs.fpu.FPCR.snan && regs.fpu.FPSR.snan) {
        return FP_SNAN();
    }
    if(regs.fpu.FPCR.operr && regs.fpu.FPSR.operr) {
        return FP_OPERR();
    }
    if(regs.fpu.FPCR.ovfl && regs.fpu.FPSR.ovfl) {
        return FP_OVFR();
    }
    if(regs.fpu.FPCR.unfl && regs.fpu.FPSR.unfl) {
        return FP_UNFL();
    }
    if(regs.fpu.FPCR.dz && regs.fpu.FPSR.dz) {
        return FP_DIV0();
    }
    if((regs.fpu.FPCR.inex2 && regs.fpu.FPSR.inex2) ||
       (regs.fpu.FPCR.inex1 && regs.fpu.FPSR.inex1)) {
        return FP_INEX();
    }
}
void to_sgl(mpfr_ptr d) {
    mpfr_prec_round(d, 24, regs.fpu.rnd_mode);
    if(mpfr_regular_p(d)) {
        regs.fpu.FPSR.inex2 = mpfr_inexflag_p();
        if(mpfr_get_exp(d) > 0x7e) {
            mpfr_set_inf(d, mpfr_signbit(d));
            regs.fpu.FPSR.ovfl = true;
        } else if(mpfr_get_exp(d) < -0x7e) {
            mpfr_set_zero(d, mpfr_signbit(d));
            regs.fpu.FPSR.unfl = true;
        }
    }
}
void to_dbl(mpfr_ptr d) {
    mpfr_prec_round(d, 53, regs.fpu.rnd_mode);
    if(mpfr_regular_p(d)) {
        regs.fpu.FPSR.inex2 = mpfr_inexflag_p();
        if(mpfr_get_exp(d) > 0x3fe) {
            mpfr_set_inf(d, mpfr_signbit(d));
            regs.fpu.FPSR.ovfl = true;
        } else if(mpfr_get_exp(d) < -0x3fe) {
            mpfr_set_zero(d, mpfr_signbit(d));
            regs.fpu.FPSR.unfl = true;
        }
    }
}
void to_ext(mpfr_ptr d) {
    if(mpfr_regular_p(d)) {
        if(mpfr_get_exp(d) > 0x3ffe) {
            mpfr_set_inf(d, mpfr_signbit(d));
            regs.fpu.FPSR.ovfl = true;
        } else if(mpfr_get_exp(d) < -0x3fff) {
            mpfr_set_zero(d, mpfr_signbit(d));
            regs.fpu.FPSR.unfl = true;
        }
    }
}
void fpu_postprocess(mpfr_ptr d, bool round = true) {
    // rounding
    if(round) {
        switch(regs.fpu.FPCR.prec) {
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
    }
    regs.fpu.FPSR.ovfl |= mpfr_overflow_p();
    regs.fpu.FPSR.unfl |= mpfr_underflow_p();
    regs.fpu.FPSR.dz = mpfr_divby0_p();
    regs.fpu.FPSR.operr = mpfr_nanflag_p() || mpfr_erangeflag_p();
    regs.fpu.FPSR.inex1 = mpfr_inexflag_p();

    // update normal flag
    regs.fpu.FPSR.n = !!mpfr_signbit(d);
    regs.fpu.FPSR.z = !!mpfr_zero_p(d);
    regs.fpu.FPSR.i = !!mpfr_inf_p(d);
    regs.fpu.FPSR.nan = !!mpfr_nan_p(d);

    fpu_checkexception();
}
using fpu_op_t = void (*)(mpfr_ptr src, mpfr_ptr dst);
void fmove_cr(uint8_t op, mpfr_ptr v) {
    switch(op) {
    case 0:
        mpfr_const_pi(v, regs.fpu.rnd_mode); // M_PI
        break;
    case 0xb: // LOG10(2)
        mpfr_set_ui(v, 2.0, regs.fpu.rnd_mode);
        mpfr_log10(v, v, regs.fpu.rnd_mode);
        break;
    case 0xc: // E
        mpfr_set_ui(v, 1.0, regs.fpu.rnd_mode);
        mpfr_exp(v, v, regs.fpu.rnd_mode);
        break;
    case 0xd: // log2(e)
        mpfr_set_ui(v, 1.0, regs.fpu.rnd_mode);
        mpfr_exp(v, v, regs.fpu.rnd_mode);
        mpfr_log2(v, v, regs.fpu.rnd_mode);
        break;
    case 0xe: // log10(e)
        mpfr_set_ui(v, 1.0, regs.fpu.rnd_mode);
        mpfr_exp(v, v, regs.fpu.rnd_mode);
        mpfr_log10(v, v, regs.fpu.rnd_mode);
        break;
    case 0xf: // 0
        mpfr_set_d(v, 0.0, regs.fpu.rnd_mode);
        break;
    case 0x30: // loge(2)
        mpfr_const_log2(v, regs.fpu.rnd_mode);
        break;
    case 0x31: // loge(10)
        mpfr_set_d(v, 10.0, regs.fpu.rnd_mode);
        mpfr_log(v, v, regs.fpu.rnd_mode);
        break;
    case 0x32: // 1e0
        mpfr_ui_pow_ui(v, 10, 0, regs.fpu.rnd_mode);
        break;
    case 0x33: // 1e1
        mpfr_ui_pow_ui(v, 10, 1, regs.fpu.rnd_mode);
        break;
    case 0x34: // 1e2
        mpfr_ui_pow_ui(v, 10, 2, regs.fpu.rnd_mode);
        break;
    case 0x35: // 1e4
        mpfr_ui_pow_ui(v, 10, 4, regs.fpu.rnd_mode);
        break;
    case 0x36: // 1e8
        mpfr_ui_pow_ui(v, 10, 8, regs.fpu.rnd_mode);
        break;
    case 0x37: // 1e16
        mpfr_ui_pow_ui(v, 10, 16, regs.fpu.rnd_mode);
        break;
    case 0x38: // 1e32
        mpfr_ui_pow_ui(v, 10, 32, regs.fpu.rnd_mode);
        break;
    case 0x39: // 1e64
        mpfr_ui_pow_ui(v, 10, 64, regs.fpu.rnd_mode);
        break;
    case 0x3a: // 1e128
        mpfr_ui_pow_ui(v, 10, 128, regs.fpu.rnd_mode);
        break;
    case 0x3b: // 1e256
        mpfr_ui_pow_ui(v, 10, 256, regs.fpu.rnd_mode);
        break;
    case 0x3c: // 1e512
        mpfr_ui_pow_ui(v, 10, 512, regs.fpu.rnd_mode);
        break;
    case 0x3d: // 1e1024
        mpfr_ui_pow_ui(v, 10, 1024, regs.fpu.rnd_mode);
        break;
    case 0x3e: // 1e2048
        mpfr_ui_pow_ui(v, 10, 2048, regs.fpu.rnd_mode);
        break;
    case 0x3f: // 1e4096
        mpfr_ui_pow_ui(v, 10, 4096, regs.fpu.rnd_mode);
        break;
    default:
        mpfr_set_d(v, 0.0, regs.fpu.rnd_mode);
    }
}
#define OP_F(name) void op_##name(mpfr_ptr src, mpfr_ptr dst)
OP_F(fmove_from) {
    mpfr_set(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fint) {
    mpfr_rint(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fsinh) {
    mpfr_sinh(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fintrz) {
    mpfr_trunc(dst, src);
    fpu_postprocess(dst);
}

OP_F(fsqrt) {
    mpfr_sqrt(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(flognp1) {
    mpfr_log1p(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fetoxm1) {
    mpfr_expm1(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(ftanh) {
    mpfr_tanh(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fatan) {
    mpfr_atan(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fasin) {
    mpfr_asin(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fatanh) {
    mpfr_atanh(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fsin) {
    mpfr_sin(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(ftan) {
    mpfr_tan(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fetox) {
    mpfr_exp(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(ftwotox) {
    mpfr_exp2(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}
OP_F(ftentox) {
    mpfr_exp10(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(flogn) {
    mpfr_log(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(flog10) {
    mpfr_log10(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(flog2) {
    mpfr_log2(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fabs) {
    mpfr_abs(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fcosh) {
    mpfr_cosh(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fneg) {
    mpfr_neg(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(facos) {
    mpfr_acos(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fcos) {
    mpfr_cos(dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fgetexp) {
    if(mpfr_nan_p(src)) {
        mpfr_set_nan(dst);
    } else if(mpfr_inf_p(src)) {
        mpfr_set_erangeflag();
        mpfr_set_nan(dst);
    } else if(mpfr_zero_p(src)) {
        mpfr_set(dst, src, regs.fpu.rnd_mode);
    } else {
        mpfr_exp_t e = mpfr_get_exp(src);
        mpfr_set_si(dst, e - 1, regs.fpu.rnd_mode);
    }
    fpu_postprocess(dst);
}

OP_F(fgetman) {
    if(mpfr_nan_p(src)) {
        mpfr_set_nan(dst);
    } else if(mpfr_inf_p(src)) {
        mpfr_set_erangeflag();
        mpfr_set_nan(dst);
    } else if(mpfr_zero_p(src)) {
        mpfr_set(dst, src, regs.fpu.rnd_mode);
    } else {
        mpfr_exp_t exp;
        mpfr_frexp(&exp, dst, src, regs.fpu.rnd_mode);
        mpfr_mul_2si(dst, dst, 1, regs.fpu.rnd_mode);
    }
    fpu_postprocess(dst);
}

OP_F(fdiv) {
    mpfr_div(dst, dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fmod) {
    long q;
    mpfr_fmodquo(dst, &q, dst, src, regs.fpu.rnd_mode);
    if(mpfr_number_p(dst)) {
        regs.fpu.FPSR.quotient = (static_cast<int8_t>(q) % 0x40);
    }
    fpu_postprocess(dst);
}

OP_F(fadd) {
    mpfr_add(dst, dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fmul) {
    mpfr_mul(dst, dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fsgldiv) {
    MPFR_DECL_INIT(src_s, 24);
    MPFR_DECL_INIT(dst_s, 24);
    mpfr_set(src_s, src, regs.fpu.rnd_mode);
    mpfr_set(dst_s, dst, regs.fpu.rnd_mode);
    mpfr_div(dst, dst_s, src_s, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst, false);
}

OP_F(frem) {
    long q;
    mpfr_remquo(dst, &q, dst, src, regs.fpu.rnd_mode);
    if(mpfr_number_p(dst)) {
        regs.fpu.FPSR.quotient = (static_cast<int8_t>(q) % 0x40);
    }
    fpu_postprocess(dst);
}

OP_F(fscale) {
    if(!mpfr_number_p(src)) {
        regs.fpu.FPSR.operr = true;
        mpfr_set_nan(dst);
    }
    long exp = mpfr_get_si(src, MPFR_RNDZ);
    mpfr_mul_2si(dst, dst, exp, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

OP_F(fsglmul) {
    MPFR_DECL_INIT(src_s, 24);
    MPFR_DECL_INIT(dst_s, 24);
    mpfr_set(src_s, src, regs.fpu.rnd_mode);
    mpfr_set(dst_s, dst, regs.fpu.rnd_mode);
    mpfr_mul(dst, dst_s, src_s, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst, false);
}

OP_F(fsub) {
    mpfr_sub(dst, dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(dst);
}

template <int i> OP_F(fsincos) {
    if(dst != regs.fpu.fp[i]) {
        mpfr_prec_round(regs.fpu.fp[i], 64, regs.fpu.rnd_mode);
        mpfr_sin_cos(dst, regs.fpu.fp[i], src, regs.fpu.rnd_mode);
    } else {
        mpfr_sin(dst, src, regs.fpu.rnd_mode);
    }
    fpu_postprocess(dst);
}

OP_F(fcmp) {
    MPFR_DECL_INIT(tmp, 64);
    mpfr_sub(tmp, dst, src, regs.fpu.rnd_mode);
    fpu_postprocess(tmp);
}

OP_F(ftst) { fpu_postprocess(src); }

OP_F(fsmove) {
    mpfr_set(dst, src, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst);
}

OP_F(fssqrt) {
    mpfr_sqrt(dst, src, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst);
}

OP_F(fdmove) {
    mpfr_set(dst, src, regs.fpu.rnd_mode);
    to_dbl(dst);
    fpu_postprocess(dst);
}

OP_F(fdsqrt) {
    mpfr_sqrt(dst, src, regs.fpu.rnd_mode);
    to_dbl(dst);
    fpu_postprocess(dst);
}

OP_F(fsabs) {
    mpfr_abs(dst, src, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst);
}

OP_F(fsneg) {
    mpfr_neg(dst, src, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst);
}

OP_F(fdabs) {
    mpfr_abs(dst, src, regs.fpu.rnd_mode);
    to_dbl(dst);
    fpu_postprocess(dst);
}

OP_F(fdneg) {
    mpfr_neg(dst, src, regs.fpu.rnd_mode);
    to_dbl(dst);
    fpu_postprocess(dst);
}

OP_F(fsdiv) {
    mpfr_div(dst, dst, src, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst);
}

OP_F(fsadd) {
    mpfr_add(dst, dst, src, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst);
}

OP_F(fsmul) {
    mpfr_mul(dst, dst, src, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst);
}
OP_F(fddiv) {
    mpfr_div(dst, dst, src, regs.fpu.rnd_mode);
    to_dbl(dst);
    fpu_postprocess(dst);
}

OP_F(fdadd) {
    mpfr_add(dst, dst, src, regs.fpu.rnd_mode);
    to_dbl(dst);
    fpu_postprocess(dst);
}

OP_F(fdmul) {
    mpfr_mul(dst, dst, src, regs.fpu.rnd_mode);
    to_dbl(dst);
    fpu_postprocess(dst);
}

OP_F(fssub) {
    mpfr_sub(dst, dst, src, regs.fpu.rnd_mode);
    to_sgl(dst);
    fpu_postprocess(dst);
}

OP_F(fdsub) {
    mpfr_sub(dst, dst, src, regs.fpu.rnd_mode);
    to_dbl(dst);
    fpu_postprocess(dst);
}

static fpu_op_t fpu_op_t_table[0x80] = {
    op_fmove_from, op_fint,       op_fsinh,      op_fintrz,     op_fsqrt,
    nullptr,       op_flognp1,    nullptr,       op_fetoxm1,    op_ftanh,
    op_fatan,      nullptr,       op_fasin,      op_fatanh,     op_fsin,
    op_ftan,       op_fetox,      op_ftwotox,    op_ftentox,    nullptr,
    op_flogn,      op_flog10,     op_flog2,      nullptr,       op_fabs,
    op_fcosh,      op_fneg,       nullptr,       op_facos,      op_fcos,
    op_fgetexp,    op_fgetman,    op_fdiv,       op_fmod,       op_fadd,
    op_fmul,       op_fsgldiv,    op_frem,       op_fscale,     op_fsglmul,
    op_fsub,       nullptr,       nullptr,       nullptr,       nullptr,
    nullptr,       nullptr,       nullptr,       op_fsincos<0>, op_fsincos<1>,
    op_fsincos<2>, op_fsincos<3>, op_fsincos<4>, op_fsincos<5>, op_fsincos<6>,
    op_fsincos<7>, op_fcmp,       nullptr,       op_ftst,       nullptr,
    nullptr,       nullptr,       nullptr,       nullptr,       op_fsmove,
    op_fssqrt,     nullptr,       nullptr,       op_fdmove,     op_fdsqrt,
    nullptr,       nullptr,       nullptr,       nullptr,       nullptr,
    nullptr,       nullptr,       nullptr,       nullptr,       nullptr,
    nullptr,       nullptr,       nullptr,       nullptr,       nullptr,
    nullptr,       nullptr,       nullptr,       op_fsabs,      nullptr,
    op_fsneg,      nullptr,       op_fdabs,      nullptr,       op_fdneg,
    nullptr,       op_fsdiv,      nullptr,       op_fsadd,      op_fsmul,
    op_fddiv,      nullptr,       op_fdadd,      op_fdmul,      op_fssub,
    nullptr,       nullptr,       nullptr,       op_fdsub};

void fmove_to(int type, int reg, int d_type, uint8_t k, mpfr_ptr value) {
    regs.i_ea = 0;
    switch(d_type) {
    case 0: {
        regs.fpu.FPSR.operr = mpfr_fits_sint_p(value, regs.fpu.rnd_mode);
        int32_t v = mpfr_get_sj(value, regs.fpu.rnd_mode);
        regs.fpu.FPSR.inex2 = mpfr_inexflag_p();
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
        regs.fpu.FPSR.operr = mpfr_fits_sshort_p(value, regs.fpu.rnd_mode);

        int16_t v = mpfr_get_sj(value, regs.fpu.rnd_mode);
        regs.fpu.FPSR.inex2 = mpfr_inexflag_p();
        EA_WRITE16(type, reg, v);
        break;
    }
    case 5: {
        uint32_t addr = EA_Addr(type, reg, 8, true);
        store_double(addr, value);
        break;
    }
    case 6: {
        int16_t v = mpfr_get_sj(value, regs.fpu.rnd_mode);
        regs.fpu.FPSR.inex2 = mpfr_inexflag_p();
        regs.fpu.FPSR.operr = v > 127 || v < -128;
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
    switch(regs.fpu.rnd_mode) {
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
    return static_cast<int>(rnd) << 4 |
           static_cast<int>(regs.fpu.FPCR.prec) << 6 |
           regs.fpu.FPCR.inex1 << 8 | regs.fpu.FPCR.inex2 << 9 |
           regs.fpu.FPCR.dz << 10 | regs.fpu.FPCR.unfl << 11 |
           regs.fpu.FPCR.ovfl << 12 | regs.fpu.FPCR.operr << 13 |
           regs.fpu.FPCR.snan << 14 | regs.fpu.FPCR.bsun << 15;
}

void set_fpcr(uint32_t v) {
    regs.fpu.FPCR.bsun = v >> 15 & 1;
    regs.fpu.FPCR.snan = v >> 14 & 1;
    regs.fpu.FPCR.operr = v >> 13 & 1;
    regs.fpu.FPCR.ovfl = v >> 12 & 1;
    regs.fpu.FPCR.unfl = v >> 11 & 1;
    regs.fpu.FPCR.dz = v >> 10 & 1;
    regs.fpu.FPCR.inex2 = v >> 9 & 1;
    regs.fpu.FPCR.inex1 = v >> 8 & 1;
    regs.fpu.FPCR.prec = static_cast<RND_PREC>(v >> 6 & 3);
    switch(static_cast<RND_MODE>(v >> 4 & 3)) {
    case RND_MODE::ROUND_RN:
        mpfr_set_default_rounding_mode(regs.fpu.rnd_mode = MPFR_RNDN);
        break;
    case RND_MODE::ROUND_RZ:
        mpfr_set_default_rounding_mode(regs.fpu.rnd_mode = MPFR_RNDZ);
        break;
    case RND_MODE::ROUND_RM:
        mpfr_set_default_rounding_mode(regs.fpu.rnd_mode = MPFR_RNDD);
        break;
    case RND_MODE::ROUND_RP:
        mpfr_set_default_rounding_mode(regs.fpu.rnd_mode = MPFR_RNDU);
        break;
    }
}

uint32_t get_fpsr() {
    return regs.fpu.FPSR.n << 27 | regs.fpu.FPSR.z << 26 |
           regs.fpu.FPSR.i << 25 | regs.fpu.FPSR.nan << 24 |
           (regs.fpu.FPSR.quotient & 0xff) << 16 | regs.fpu.FPSR.bsun << 15 |
           regs.fpu.FPSR.snan << 14 | regs.fpu.FPSR.operr << 13 |
           regs.fpu.FPSR.ovfl << 12 | regs.fpu.FPSR.unfl << 11 |
           regs.fpu.FPSR.dz << 10 | regs.fpu.FPSR.inex2 << 9 |
           regs.fpu.FPSR.inex1 << 8 | regs.fpu.FPSR.ac_iop << 7 |
           regs.fpu.FPSR.ac_ovfl << 6 | regs.fpu.FPSR.ac_unfl << 5 |
           regs.fpu.FPSR.ac_dz << 4 | regs.fpu.FPSR.ac_inex << 3;
}
void set_fpsr(uint32_t v) {
    regs.fpu.FPSR.n = v >> 27 & 1;
    regs.fpu.FPSR.z = v >> 26 & 1;
    regs.fpu.FPSR.i = v >> 25 & 1;
    regs.fpu.FPSR.nan = v >> 24 & 1;
    int8_t qut = v >> 16 & 0xff;
    regs.fpu.FPSR.quotient = qut;
    regs.fpu.FPSR.bsun = v >> 15 & 1;
    regs.fpu.FPSR.snan = v >> 14 & 1;
    regs.fpu.FPSR.operr = v >> 13 & 1;
    regs.fpu.FPSR.ovfl = v >> 12 & 1;
    regs.fpu.FPSR.unfl = v >> 11 & 1;
    regs.fpu.FPSR.dz = v >> 10 & 1;
    regs.fpu.FPSR.inex2 = v >> 9 & 1;
    regs.fpu.FPSR.inex1 = v >> 8 & 1;
    regs.fpu.FPSR.ac_iop = v >> 7 & 1;
    regs.fpu.FPSR.ac_ovfl = v >> 6 & 1;
    regs.fpu.FPSR.ac_unfl = v >> 5 & 1;
    regs.fpu.FPSR.ac_dz = v >> 4 & 1;
    regs.fpu.FPSR.ac_inex = v >> 3 & 1;
}

void fmovem_from_mem(uint32_t *addr, int mask) {
    for(int i = 0; i < 8; ++i) {
        if(mask & 1 << (7 - i)) {
            load_ext(*addr, regs.fpu.fp[i]);
            *addr += 12;
        }
    }
}

void fmovem_to_mem(uint32_t addr, int mask) {
    for(int i = 0; i < 8; ++i) {
        if(mask & 1 << (7 - i)) {
            store_ext(addr, regs.fpu.fp[i]);
            addr += 12;
        }
    }
}

void fmovem_to_mem_decr(uint32_t *addr, int mask) {
    for(int i = 7; i >= 0; --i) {
        if(mask & 1 << i) {
            store_ext(*addr -= 12, regs.fpu.fp[i]);
        }
    }
}

OP(fpu_op) {
    uint16_t op2 = FETCH();
    uint32_t ea;
    if(op2 >> 15 & 1) {
        switch(op2 >> 13 & 3) {
        case 0:
            // FMOVE(M) to CR
            switch(op2 >> 10 & 7) {
            case 0:
                FP_UNDEF();
                break;
            case 1:
                regs.fpu.fpiar = EA_READ32(type, reg);
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
                regs.fpu.fpiar = read32(ea + 4);
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
                regs.fpu.fpiar = read32(ea + 4);
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
                regs.fpu.fpiar = read32(ea + 8);
                break;
            }
            return;
        case 1:
            // FMOVE(M) from CR
            regs.i_ea = 0;
            switch(op2 >> 10 & 7) {
            case 0:
                FP_UNDEF();
                break;
            case 1:
                EA_WRITE32(type, reg, regs.fpu.fpiar);
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
                write32(ea + 4, regs.fpu.fpiar);
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
                write32(ea + 4, regs.fpu.fpiar);
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
                write32(ea + 8, regs.fpu.fpiar);
                break;
            }
            return;
        case 2: {
            // FMOVEM FROM MEM
            int mode = op2 >> 11 & 3;
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
    regs.fpu.fpiar = regs.opc;
    bool rm = op2 >> 14 & 1;
    int src_s = op2 >> 10 & 7;
    int dst_r = op2 >> 7 & 7;
    int opc2 = op2 & 0x7f;
    if(!(op2 >> 13 & 1)) {
        MPFR_DECL_INIT(src, 64);
        if(rm) {
            switch(src_s) {
            case 0:
                mpfr_set_si(src, static_cast<int32_t>(EA_READ32(type, reg)),
                            regs.fpu.rnd_mode);
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
                mpfr_set_si(src, static_cast<int16_t>(EA_READ16(type, reg)),
                            regs.fpu.rnd_mode);
                break;
            case 5: {
                uint32_t addr = EA_Addr(type, reg, 8, false);
                load_double(addr, src);
                break;
            }
            case 6:
                mpfr_set_si(src, static_cast<int8_t>(EA_READ8(type, reg)),
                            regs.fpu.rnd_mode);
                break;
            case 7:
                mpfr_prec_round(regs.fpu.fp[dst_r], 64, regs.fpu.rnd_mode);
                fmove_cr(opc2, regs.fpu.fp[dst_r]);
                fpu_postprocess(regs.fpu.fp[dst_r]);
                return;
            }
        } else {
            mpfr_set(src, regs.fpu.fp[src_s], regs.fpu.rnd_mode);
        }
        if(fpu_op_t_table[opc2]) {
            regs.fpu.FPSR.bsun = false;
            regs.fpu.FPSR.operr = false;
            regs.fpu.FPSR.ovfl = false;
            regs.fpu.FPSR.unfl = false;
            regs.fpu.FPSR.dz = false;
            mpfr_prec_round(regs.fpu.fp[dst_r], 64, regs.fpu.rnd_mode);
            fpu_op_t_table[opc2](src, regs.fpu.fp[dst_r]);
        } else {
            FP_UNDEF();
        }
    } else {
        // FMOVE TO
        fmove_to(type, reg, src_s, opc2, regs.fpu.fp[dst_r]);
    }
}
bool fpcc(int c) {
    if(c & 0x20) {
        return false;
    }
    if((c & 0x10) && regs.fpu.FPSR.nan) {
        regs.fpu.FPSR.bsun = true;
    }

    switch(c & 0xf) {
    case 0:
        return false; // F
    case 1:
        return regs.fpu.FPSR.z; // EQ
    case 2:
        return !(regs.fpu.FPSR.nan | regs.fpu.FPSR.z | regs.fpu.FPSR.n); // OGT
    case 3:
        return regs.fpu.FPSR.z | !(regs.fpu.FPSR.nan | regs.fpu.FPSR.n); // OGE
    case 4:
        return regs.fpu.FPSR.n & !(regs.fpu.FPSR.nan | regs.fpu.FPSR.z); // OLT
    case 5:
        return regs.fpu.FPSR.z | (regs.fpu.FPSR.n & !regs.fpu.FPSR.nan); // OLE
    case 6:
        return !(regs.fpu.FPSR.nan | regs.fpu.FPSR.z); // OGL
    case 7:
        return !regs.fpu.FPSR.nan; // OR
    case 8:
        return regs.fpu.FPSR.nan; // UN
    case 9:
        return regs.fpu.FPSR.nan | regs.fpu.FPSR.z; // UEQ
    case 10:
        return regs.fpu.FPSR.nan | !(regs.fpu.FPSR.n | regs.fpu.FPSR.z); // UGT
    case 11:
        return regs.fpu.FPSR.nan | regs.fpu.FPSR.z | !regs.fpu.FPSR.n; // UGE
    case 12:
        return regs.fpu.FPSR.nan | (regs.fpu.FPSR.n & !regs.fpu.FPSR.z); // ULT
    case 13:
        return regs.fpu.FPSR.nan | regs.fpu.FPSR.z | regs.fpu.FPSR.n; // ULE
    case 14:
        return !regs.fpu.FPSR.z; // NE
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
        JUMP(pc + offset);
    }
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
    EA_WRITE32(type, reg, 0x41000000);
}
OP(frestore) {
    if(!regs.S) {
        PRIV_ERROR();
    }
    uint32_t fw = EA_READ32(type, reg);
    if(fw >> 24 == 0) {
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
}
