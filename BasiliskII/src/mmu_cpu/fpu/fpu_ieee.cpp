#include "fpu/fpu_ieee.h"
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
#pragma fenv_access on
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536];

bool is_snan(float f) {
    return isnanf(f) && !(bit_cast<uint32_t>(f) & 0x400000);
}
bool is_snan(double d) {
    return isnanf(d) && !(bit_cast<uint64_t>(d) & 0x8000000000000);
}

void update_round() {
    switch(regs.FPCR.rnd) {
    case ROUND_RN:
        fesetround(FE_TONEAREST);
        break;
    case ROUND_RZ:
        fesetround(FE_TOWARDZERO);
        break;
    case ROUND_RM:
        fesetround(FE_DOWNWARD);
        break;
    case ROUND_RP:
        fesetround(FE_UPWARD);
        break;
    }
}
float load_single(uint32_t addr) {
    uint32_t raw = read32(addr);
    float v = bit_cast<float>(raw);
    if(is_snan(v)) {
        regs.FPSR.snan = true;
    }
    return v;
}

void store_single(uint32_t addr, float raw) {
    uint32_t v = bit_cast<uint32_t>(raw);
    write32(addr, v);
}
double load_double(uint32_t addr) {
    uint64_t raw;
    raw = read32(addr);
    raw = raw << 32 | read32(addr + 4);
    double v = bit_cast<double>(raw);
    if(is_snan(v)) {
        regs.FPSR.snan = true;
    }
    return v;
}

void store_double(uint32_t addr, double raw) {
    uint64_t v = bit_cast<uint64_t>(raw);
    write32(addr, v >> 32);
    write32(addr + 4, v);
}

struct ieee_double {
    unsigned long long fraction : 52;
    unsigned int exp : 11;
    bool sign : 1;
};

double load_ext(uint32_t addr) {
    uint16_t exp_i = read16(addr);
    uint64_t mantissa = read32(addr + 4);
    mantissa = mantissa << 32 | read32(addr + 8);
    bool s = exp_i & 0x8000;
    int exp_v = exp_i & 0x7fff;
    if(exp_v == 0x7fff) {
        if(mantissa == 0) {
            return copysign(INFINITY, s ? -1.0 : 1.0);
        } else {
            // NAN
            if(!(mantissa >> 63 & 1)) {
                regs.FPSR.snan = true;
            }
            ieee_double fltp;
            fltp.sign = s;
            fltp.exp = 0x7ff;
            fltp.fraction = (mantissa << 1) >> 12;
        }
    }
    if(exp_v - 16383 > 1023) {
        // force to infinity
        exp_v = 1024;
        mantissa = 0;
    }
    if(exp_v - 16383 < -1022) {
        // force to zero
        exp_v = -1023;
        mantissa = 0;
    }
    ieee_double fltp;
    fltp.sign = s;
    fltp.exp = exp_v - 16383 + 1023;
    fltp.fraction = (mantissa << 1) >> 12;
    regs.FPSR.inex2 = true;
    return bit_cast<double>(fltp);
}

void store_ext(uint32_t addr, double d) {
    ieee_double dv = bit_cast<ieee_double>(d);
    bool s = dv.sign;
    int exp = dv.exp;
    uint64_t mant = dv.fraction << 11;
    switch(fpclassify(d)) {
    case FP_NAN:
    case FP_INFINITE:
        write32(addr, (s << 31) | 0x7ff0000);
        break;
    case FP_ZERO:
    case FP_SUBNORMAL:
        write32(addr, (s << 31));
        break;
    default: // FP_NORAML:
        write32(addr, (s << 31) | (exp + 0x3c00) << 16);
        mant |= 1ll << 63;
        break;
    }
    write32(addr + 4, mant >> 32);
    write32(addr + 8, mant);
}

double load_packed(uint32_t addr) {
    uint32_t v1 = read32(addr);
    uint64_t vd = read32(addr + 4);
    vd = vd << 32 | read32(addr + 8);
    double sm = (v1 & 1 << 31) ? -1.0 : 1.0;
    bool se = v1 & 1 << 30;
    uint8_t yy = v1 >> 28 & 3;
    if(se && yy == 3 && ((v1 >> 16) & 0xfff) == 0xfff) {
        if(vd == 0) {
            return copysign(INFINITY, sm);
        } else {
            return copysign(NAN, sm);
        }
    }
    if(vd == 0) {
        return copysign(0.0, sm);
    }
    int16_t exp =
        100 * ((v1 >> 24) & 0xf) + 10 * ((v1 >> 20) & 0xf) + ((v1 >> 16) & 0xf);
    if(se) {
        exp = -exp;
    }
    double c = .1;
    double xs = (v1 & 0xf);
    for(int i = 0; i < 16; ++i, c /= 10.0) {
        xs += ((vd >> (60 - i * 4)) & 0xf) * c;
    }
    regs.FPSR.inex1 = true;
    return copysign(xs * pow(10.0, exp), sm);
}

void store_packed(uint32_t addr, double value, int8_t k) {
    bool s = signbit(value);
    uint32_t exp = s << 31;
    value = fabs(value);
    switch(fpclassify(value)) {
    case FP_NAN:
    case FP_INFINITE: {
        uint64_t mant = (bit_cast<uint64_t>(value) & 0xFFFFFFFFFFFFF) << 11;
        write32(addr, exp | 0x7fff0000);
        write32(addr + 4, mant >> 32);
        write32(addr + 8, mant);
        return;
    }
    case FP_ZERO:
        write32(addr, exp);
        write32(addr + 4, 0);
        write32(addr + 8, 0);
        break;
    }
    // FP_SUBNORMAL / FP_NORAML:
    if(k > 17) {
        regs.FPSR.operr = true;
        k = 17;
    }
    char buf[64], *cp = buf;
    update_round();
    uint64_t mant = 0;
    if(k <= 0) {
        int mp = log10(value);
        k = mp - k;
    } else {
        k -= 1;
    }
    // SM-SE-0-0 EEE-XXXD DDDDDDDD DDDDDDDD
    snprintf(buf, 64, "%.*e", k, value);
    exp |= *(cp++) - '0'; // DIGIT16
    if(*cp == '.') {
        ++cp;
        for(int i = 0; i < k; ++i) {
            if(*cp != 'e') {
                mant |= static_cast<uint64_t>(*(cp++) - '0')
                        << (64 - (i + 1) * 4);
            }
        }
    }
    // omit 'e'
    ++cp;
    if(*(cp++) == '-') {
        exp |= 1 << 30;
    }
    if(cp[2]) {
        // over 99
        exp |= (cp[0] - '0') << 24 | (cp[1] - '0') << 20 | (cp[2] - '0') << 16;
    } else {
        exp |= (cp[0] - '0') << 20 | (cp[1] - '0') << 16;
    }
    write32(addr, exp);
    write32(addr + 4, mant >> 32);
    write32(addr + 8, mant);
}

void fpu_checkexception() {
    // update acc flags
    regs.FPSR.ac_iop |= regs.FPSR.snan | regs.FPSR.operr;
    regs.FPSR.ac_ovfl |= regs.FPSR.ovfl;
    regs.FPSR.ac_unfl |= regs.FPSR.unfl | regs.FPSR.inex2;
    regs.FPSR.ac_dz |= regs.FPSR.dz;
    regs.FPSR.ac_inex |= regs.FPSR.inex1 | regs.FPSR.inex2 | regs.FPSR.ovfl;
    // raise excpetion
    if(regs.FPCR.bsun && regs.FPSR.bsun) {
        return FP_UNCND();
    }
    if(regs.FPCR.snan && regs.FPSR.snan) {
        return FP_SNAN();
    }
    if(regs.FPCR.operr && regs.FPSR.operr) {
        return FP_OPERR();
    }
    if(regs.FPCR.ovfl && regs.FPSR.ovfl) {
        return FP_OVFR();
    }
    if(regs.FPCR.unfl && regs.FPSR.unfl) {
        return FP_UNFL();
    }
    if(regs.FPCR.dz && regs.FPSR.dz) {
        return FP_DIV0();
    }
    if((regs.FPCR.inex2 && regs.FPSR.inex2) ||
       (regs.FPCR.inex1 && regs.FPSR.inex1)) {
        return FP_INEX();
    }
}
void fpu_postprocess(int fpn, double d, bool tst_only = false) {
    // rounding
    if(regs.FPCR.prec == ROUND_SGL) {
        feclearexcept(FE_INEXACT);
        update_round();
        d = static_cast<float>(d);
        regs.FPSR.inex2 = fetestexcept(FE_INEXACT);
    }

    // update normal flag
    regs.FPSR.n = !!(signbit(d));
    regs.FPSR.z = d == 0.0;
    regs.FPSR.i = isinf(d);
    regs.FPSR.nan = isnan(d);

    if(!tst_only) {
        // update register
        regs.fp[fpn] = d;
    }

    fpu_checkexception();
}
using fpu_op_t = double (*)(double src, double dst);
// taken from glibc math.h
double fmove_cr(uint8_t op) {
    switch(op) {
    case 0:
        return 3.14159265358979323846; // M_PI
    case 0xb:
        return log(2) / log(10); // LOG10(2)
    case 0xc:
        return 2.7182818284590452354; // E
    case 0xd:
        return 1.4426950408889634074; // log2(e)
    case 0xe:
        return 0.43429448190325182765; // log10(e)
    case 0xf:
        return 0.0;
    case 0x30:
        return 0.69314718055994530942; // loge(2)
    case 0x31:
        return 2.30258509299404568402; // loge(10)
    case 0x32:
        return 1e0;
    case 0x33:
        return 1e1;
    case 0x34:
        return 1e2;
    case 0x35:
        return 1e4;
    case 0x36:
        return 1e8;
    case 0x37:
        return 1e16;
    case 0x38:
        return 1e32;
    case 0x39:
        return 1e64;
    case 0x3a:
        return 1e128;
    case 0x3b:
        return 1e256;
    case 0x3c:
        return INFINITY; // actually 1e512;
    case 0x3d:
        return INFINITY; // actually 1e1024;
    case 0x3e:
        return INFINITY; // actually 1e2048;
    case 0x3f:
        return INFINITY; // actually 1e4096;
    default:
        return 0.0;
    }
}
#define OP_F(name)                                                             \
    double op_##name(double src, double __attribute__((unused)) dst)
OP_F(fmove_from) {
    regs.FPSR.unfl = fpclassify(src) == FP_SUBNORMAL;
    return src;
}

OP_F(fint) {
    switch(regs.FPCR.rnd) {
    case ROUND_RN:
        fesetround(FE_TONEAREST);
        break;
    case ROUND_RZ:
        fesetround(FE_TOWARDZERO);
        break;
    case ROUND_RM:
        fesetround(FE_DOWNWARD);
        break;
    case ROUND_RP:
        fesetround(FE_UPWARD);
        break;
    }
    return nearbyint(src);
}

OP_F(fsinh) {
    double re = sinh(src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    return re;
}

OP_F(fintrz) { return trunc(src); }

OP_F(fsqrt) {
    double v = sqrt(src);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(flognp1) {
    double v = log1p(src);
    regs.FPSR.dz = fetestexcept(FE_DIVBYZERO);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(fetoxm1) {
    double v = expm1(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(ftanh) {
    double v = tanh(src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(fatan) {
    double v = atan(src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(fasin) {
    double v = asin(src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fatanh) {
    double v = atanh(src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.dz = fetestexcept(FE_DIVBYZERO);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fsin) {
    double v = sin(src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(ftan) {
    double v = tan(src);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(fetox) {
    double v = exp(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(ftwotox) {
    double v = exp2(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}
OP_F(ftentox) {
    double v = pow(10.0, src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(flogn) {
    double v = log(src);
    regs.FPSR.dz = fetestexcept(FE_DIVBYZERO);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(flog10) {
    double v = log10(src);
    regs.FPSR.dz = fetestexcept(FE_DIVBYZERO);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(flog2) {
    double v = log2(src);
    regs.FPSR.dz = fetestexcept(FE_DIVBYZERO);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fabs) {
    double v = fabs(src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(fcosh) {
    double v = cosh(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    return v;
}

OP_F(fneg) {
    double v = -src;
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    return v;
}

OP_F(facos) {
    double v = acos(src);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fcos) {
    double v = cos(src);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fgetexp) {
    if(isnan(src)) {
        return src;
    } else if(isinf(src)) {
        regs.FPSR.operr = true;
        return NAN;
    } else if(src == 0.0) {
        return src;
    } else {
        return logb(src);
    }
}

OP_F(fgetman) {
    if(isnan(src)) {
        return src;
    } else if(isinf(src)) {
        regs.FPSR.operr = true;
        return NAN;
    } else if(src == 0.0) {
        return src;
    } else {
        int exp;
        return frexp(src, &exp) * 2;
    }
}

OP_F(fdiv) {
    double v = dst / src;
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.dz = fetestexcept(FE_DIVBYZERO);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fmod) {
    if(src != 0 && isfinite(dst)) {
        double qut = trunc(dst / src);
        regs.FPSR.qutinent = (static_cast<int8_t>(qut) % 0x40);
    }
    double v = fmod(dst, src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fadd) {
    double v = dst + src;
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fmul) {
    double v = dst * src;
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fsgldiv) {
    switch(regs.FPCR.rnd) {
    case ROUND_RN:
        fesetround(FE_TONEAREST);
        break;
    case ROUND_RZ:
        fesetround(FE_TOWARDZERO);
        break;
    case ROUND_RM:
        fesetround(FE_DOWNWARD);
        break;
    case ROUND_RP:
        fesetround(FE_UPWARD);
        break;
    }
    float v = static_cast<float>(dst / src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.dz = fetestexcept(FE_DIVBYZERO);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(frem) {
    if(src != 0 && isfinite(dst)) {
        double qut = nearbyint(dst / src);
        regs.FPSR.qutinent = (static_cast<int8_t>(qut) % 0x40);
    }
    double v = remainder(dst, src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fscale) {
    if(!isfinite(src)) {
        regs.FPSR.operr = true;
        return NAN;
    }
    double v = ldexp(dst, src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    return v;
}

OP_F(fsglmul) {
    switch(regs.FPCR.rnd) {
    case ROUND_RN:
        fesetround(FE_TONEAREST);
        break;
    case ROUND_RZ:
        fesetround(FE_TOWARDZERO);
        break;
    case ROUND_RM:
        fesetround(FE_DOWNWARD);
        break;
    case ROUND_RP:
        fesetround(FE_UPWARD);
        break;
    }
    float v = static_cast<float>(dst * src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fsub) {
    double v = dst - src;
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

template <int i> OP_F(fsincos) {
    double v = sin(src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    regs.fp[i] = cos(src);
    return v;
}

OP_F(fcmp) { return dst - src; }

OP_F(ftst) { return src; }

OP_F(fsmove) {
    float v = src;
    regs.FPSR.unfl = fpclassify(v) == FP_SUBNORMAL;
    return v;
}

OP_F(fssqrt) {
    float v = sqrtf(src);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fdmove) {
    double v = src;
    regs.FPSR.unfl = fpclassify(v) == FP_SUBNORMAL;
    return v;
}

OP_F(fdsqrt) {
    double v = sqrt(src);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fsabs) {
    float v = fabsf(static_cast<float>(src));
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(fsneg) {
    float v = -static_cast<float>(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    return v;
}

OP_F(fdabs) {
    double v = fabs(src);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    return v;
}

OP_F(fdneg) {
    double v = -src;
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    return v;
}

OP_F(fsdiv) {
    float v = static_cast<float>(dst) / static_cast<float>(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.dz = fetestexcept(FE_DIVBYZERO);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fsadd) {
    float v = static_cast<float>(dst) + static_cast<float>(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fsmul) {
    float v = static_cast<float>(dst) * static_cast<float>(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}
OP_F(fddiv) {
    double v = static_cast<double>(dst) / static_cast<double>(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.dz = fetestexcept(FE_DIVBYZERO);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fdadd) {
    double v = static_cast<double>(dst) + static_cast<double>(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fdmul) {
    double v = static_cast<double>(dst) * static_cast<double>(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fssub) {
    float v = static_cast<float>(dst) - static_cast<float>(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
}

OP_F(fdsub) {
    float v = static_cast<float>(dst) - static_cast<float>(src);
    regs.FPSR.ovfl = fetestexcept(FE_OVERFLOW);
    regs.FPSR.unfl = fetestexcept(FE_UNDERFLOW);
    regs.FPSR.operr = fetestexcept(FE_INVALID);
    return v;
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

void fmove_to(int type, int reg, int d_type, uint8_t k, double value) {
    regs.i_ea = 0;
    switch(d_type) {
    case 0: {
        int32_t v = static_cast<int32_t>(value);
        regs.FPSR.operr = fetestexcept(FE_INVALID);
        regs.FPSR.inex2 = fetestexcept(FE_INEXACT);
        EA_WRITE32(type, reg, v);
        break;
    }
    case 1: {
        uint32_t addr = EA_Addr(type, reg, 4, true);
        float v = static_cast<float>(value);
        regs.FPSR.operr = fetestexcept(FE_INVALID);
        regs.FPSR.inex2 = fetestexcept(FE_INEXACT);
        store_single(addr, v);
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
        int16_t v = static_cast<int16_t>(value);
        regs.FPSR.operr = fetestexcept(FE_INVALID);
        regs.FPSR.inex2 = fetestexcept(FE_INEXACT);
        EA_WRITE16(type, reg, v);
        break;
    }
    case 5: {
        uint32_t addr = EA_Addr(type, reg, 8, true);
        store_double(addr, value);
        break;
    }
    case 6: {
        int8_t v = static_cast<int8_t>(value);
        regs.FPSR.operr = fetestexcept(FE_INVALID);
        regs.FPSR.inex2 = fetestexcept(FE_INEXACT);
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
    return regs.FPCR.rnd << 4 | regs.FPCR.prec << 6 | regs.FPCR.inex1 << 8 |
           regs.FPCR.inex2 << 9 | regs.FPCR.dz << 10 | regs.FPCR.unfl << 11 |
           regs.FPCR.ovfl << 12 | regs.FPCR.operr << 13 | regs.FPCR.snan << 14 |
           regs.FPCR.bsun << 15;
}

void set_fpcr(uint32_t v) {
    regs.FPCR.bsun = v >> 15 & 1;
    regs.FPCR.snan = v >> 14 & 1;
    regs.FPCR.operr = v >> 13 & 1;
    regs.FPCR.ovfl = v >> 12 & 1;
    regs.FPCR.unfl = v >> 11 & 1;
    regs.FPCR.dz = v >> 10 & 1;
    regs.FPCR.inex2 = v >> 9 & 1;
    regs.FPCR.inex1 = v >> 8 & 1;
    regs.FPCR.prec = v >> 6 & 3;
    regs.FPCR.rnd = v >> 4 & 3;
}

uint32_t get_fpsr() {
    return regs.FPSR.n << 27 | regs.FPSR.z << 26 | regs.FPSR.i << 25 |
           regs.FPSR.nan << 24 | (regs.FPSR.qutinent & 0xff) << 16 |
           regs.FPSR.bsun << 15 | regs.FPSR.snan << 14 | regs.FPSR.operr << 13 |
           regs.FPSR.ovfl << 12 | regs.FPSR.unfl << 11 | regs.FPSR.dz << 10 |
           regs.FPSR.inex2 << 9 | regs.FPSR.inex1 << 8 | regs.FPSR.ac_iop << 7 |
           regs.FPSR.ac_ovfl << 6 | regs.FPSR.ac_unfl << 5 |
           regs.FPSR.ac_dz << 4 | regs.FPSR.ac_inex << 3;
}
void set_fpsr(uint32_t v) {
    regs.FPSR.n = v >> 27 & 1;
    regs.FPSR.z = v >> 26 & 1;
    regs.FPSR.i = v >> 25 & 1;
    regs.FPSR.nan = v >> 24 & 1;
    int8_t qut = v >> 16 & 0xff;
    regs.FPSR.qutinent = qut;
    regs.FPSR.bsun = v >> 15 & 1;
    regs.FPSR.snan = v >> 14 & 1;
    regs.FPSR.operr = v >> 13 & 1;
    regs.FPSR.ovfl = v >> 12 & 1;
    regs.FPSR.unfl = v >> 11 & 1;
    regs.FPSR.dz = v >> 10 & 1;
    regs.FPSR.inex2 = v >> 9 & 1;
    regs.FPSR.inex1 = v >> 8 & 1;
    regs.FPSR.ac_iop = v >> 7 & 1;
    regs.FPSR.ac_ovfl = v >> 6 & 1;
    regs.FPSR.ac_unfl = v >> 5 & 1;
    regs.FPSR.ac_dz = v >> 4 & 1;
    regs.FPSR.ac_inex = v >> 3 & 1;
}

void fmovem_from_mem(uint32_t *addr, int mask) {
    for(int i = 0; i < 8; ++i) {
        if(mask & 1 << (7 - i)) {
            regs.fp[i] = load_ext(*addr);
            *addr += 12;
        }
    }
}

void fmovem_to_mem(uint32_t addr, int mask) {
    for(int i = 0; i < 8; ++i) {
        if(mask & 1 << (7 - i)) {
            store_ext(addr, regs.fp[i]);
            addr += 12;
        }
    }
}

void fmovem_to_mem_decr(uint32_t *addr, int mask) {
    for(int i = 7; i >= 0; --i) {
        if(mask & 1 << i) {
            store_ext(*addr -= 12, regs.fp[i]);
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
                regs.fpiar = EA_READ32(type, reg);
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
                regs.fpiar = read32(ea + 4);
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
                regs.fpiar = read32(ea + 4);
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
                regs.fpiar = read32(ea + 8);
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
                EA_WRITE32(type, reg, regs.fpiar);
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
                write32(ea + 4, regs.fpiar);
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
                write32(ea + 4, regs.fpiar);
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
                write32(ea + 8, regs.fpiar);
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
            uint8_t reglist;
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
    regs.fpiar = regs.opc;
    bool rm = op2 >> 14 & 1;
    int src_s = op2 >> 10 & 7;
    int dst_r = op2 >> 7 & 7;
    int opc2 = op2 & 0x7f;
    if(!(op2 >> 13 & 1)) {
        double src, dst = regs.fp[dst_r];
        feclearexcept(FE_ALL_EXCEPT);
        if(rm) {
            switch(src_s) {
            case 0:
                src = static_cast<double>(
                    static_cast<int32_t>(EA_READ32(type, reg)));
                break;
            case 1: {
                uint32_t addr = EA_Addr(type, reg, 4, false);
                src = load_single(addr);
                break;
            }
            case 2: {
                uint32_t addr = EA_Addr(type, reg, 12, false);
                src = load_ext(addr);
                break;
            }
            case 3: {
                uint32_t addr = EA_Addr(type, reg, 12, false);
                src = load_packed(addr);
                break;
            }
            case 4:
                src = static_cast<double>(
                    static_cast<int16_t>(EA_READ16(type, reg)));
                break;
            case 5: {
                uint32_t addr = EA_Addr(type, reg, 8, false);
                src = load_double(addr);
                break;
            }
            case 6:
                src = static_cast<double>(
                    static_cast<int8_t>(EA_READ8(type, reg)));
                break;
            case 7:
                dst = fmove_cr(opc2);
                fpu_postprocess(dst_r, dst);
                return;
            }
        } else {
            src = regs.fp[src_s];
        }
        if(fpu_op_t_table[opc2]) {
            regs.FPSR.bsun = false;
            regs.FPSR.operr = false;
            regs.FPSR.ovfl = false;
            regs.FPSR.unfl = false;
            regs.FPSR.dz = false;
            dst = fpu_op_t_table[opc2](src, dst);
            fpu_postprocess(dst_r, dst, opc2 == 0x3a || opc2 == 0x38);
        } else {
            FP_UNDEF();
        }
    } else {
        // FMOVE TO
        fmove_to(type, reg, src_s, opc2, regs.fp[dst_r]);
    }
}
bool fpcc(int c) {
    if(c & 0x20) {
        return false;
    }
    if((c & 0x10) && regs.FPSR.nan) {
        regs.FPSR.bsun = true;
    }

    switch(c & 0xf) {
    case 0:
        return false; // F
    case 1:
        return regs.FPSR.z; // EQ
    case 2:
        return !(regs.FPSR.nan | regs.FPSR.z | regs.FPSR.n); // OGT
    case 3:
        return regs.FPSR.z | !(regs.FPSR.nan | regs.FPSR.n); // OGE
    case 4:
        return regs.FPSR.n & !(regs.FPSR.nan | regs.FPSR.z); // OLT
    case 5:
        return regs.FPSR.z | (regs.FPSR.n & !regs.FPSR.nan); // OLE
    case 6:
        return !(regs.FPSR.nan | regs.FPSR.z); // OGL
    case 7:
        return !regs.FPSR.nan; // OR
    case 8:
        return regs.FPSR.nan; // UN
    case 9:
        return regs.FPSR.nan | regs.FPSR.z; // UEQ
    case 10:
        return regs.FPSR.nan | !(regs.FPSR.n | regs.FPSR.z); // UGT
    case 11:
        return regs.FPSR.nan | regs.FPSR.z | !regs.FPSR.n; // UGE
    case 12:
        return regs.FPSR.nan | (regs.FPSR.n & !regs.FPSR.z); // ULT
    case 13:
        return regs.FPSR.nan | regs.FPSR.z | regs.FPSR.n; // ULE
    case 14:
        return !regs.FPSR.z; // NE
    case 15:
        return true; // T
    }
    return false;
}
OP(fpu_fscc) {
    if(type == 1) {
        // FDBcc
        uint16_t op2 = FETCH();
        uint32_t pc = regs.pc;
        int16_t disp = FETCH();
        if(!fpcc(op2 & 0x3f)) {
            int16_t v = regs.d[reg] - 1;
            WRITE_D16(reg, v);
            if(v != -1) {
                JUMP(pc + disp);
            }
        }
    } else {
        // FScc
        uint16_t op2 = FETCH();
        regs.i_ea = 0;
        EA_WRITE8(type, reg, fpcc(op2 & 0x3f) ? 0xff : 0);
        fpu_checkexception();
    }
}

OP(ftrapcc) {
    uint16_t op2 = FETCH();
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
    if(fpcc(op2 & 0x3f)) {
        TRPPcc();
    }
}

OP(fbcc) {
    int opc = xop & 0x3f;
    uint32_t pc = regs.pc;
    int32_t offset;
    if(xop & 1 << 6) {
        offset = FETCH32();
    } else {
        offset = static_cast<int16_t>(FETCH());
    }
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
    for(int i = 0; i < 075; ++i) {
        opc_map[0171000 | i] = op_fpu_op;
        opc_map[0171100 | i] = op_fpu_fscc;
    }
    for(int i = 2; i < 5; ++i) {
        opc_map[0171170 | i] = op_ftrapcc;
    }

    for(int i = 0; i < 0100; ++i) {
        opc_map[0171200 | i] = op_fbcc;
        opc_map[0171300 | i] = op_fbcc;
    }
    for(int i = 020; i < 074; ++i) {
        opc_map[0171400 | i] = op_fsave;
        opc_map[0171400 | i] = op_frestore;
    }
}
