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
#include <thread>
#include <unordered_map>
#include <vector>
#pragma fenv_access on
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536];

bool is_snan(float f) {
    return isnanf(f) && !(bit_cast<uint32_t>(f) & 0x400000);
}
bool is_snan(double d) {
    return isnanf(d) && !(bit_cast<uint64_t>(d) & 0x8000000000000);
}
float load_single(uint32_t addr) {
    uint32_t raw = read32(addr);
    float v = bit_cast<float>(raw);
    if(is_snan(v)) {
        regs.FPSR.snan = true;
    }
    return v;
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
            uint64_t fltp = static_cast<uint64_t>(s) << 63 |
                            static_cast<uint64_t>(0x7ff) << 52 |
                            ((mantissa & ~(1ll << 63)) >> 11);
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

    uint64_t fltp = static_cast<uint64_t>(s) << 63 |
                    static_cast<uint64_t>(exp_v - 16383 + 1023) << 52 |
                    ((mantissa & ~(1ll << 63)) >> 11);
    regs.FPSR.inex2 = true;
    return bit_cast<double>(fltp);
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
void fpu_postprocess(int fpn, double d) {
    // rounding
    if(regs.FPCR.prec == ROUND_SGL) {
        feclearexcept(FE_INEXACT);
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
        d = static_cast<float>(d);
        regs.FPSR.inex2 = fetestexcept(FE_INEXACT);
    }

    // update acc flags
    regs.FPSR.ac_iop |= regs.FPSR.snan | regs.FPSR.operr;
    regs.FPSR.ac_ovfl |= regs.FPSR.ovfl;
    regs.FPSR.ac_unfl |= regs.FPSR.unfl | regs.FPSR.inex2;
    regs.FPSR.ac_dz |= regs.FPSR.dz;
    regs.FPSR.ac_inex |= regs.FPSR.inex1 | regs.FPSR.inex2 | regs.FPSR.ovfl;

    // update normal flag
    regs.FPSR.n = !!(signbit(d));
    regs.FPSR.z = d == 0.0;
    regs.FPSR.i = isinf(d);
    regs.FPSR.nan = isnan(d);

    // update register
    regs.fp[fpn] = d;

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
using fpu_op_t = double (*)(double src, double dst);
double fmove_cr(uint8_t op) { return 0.0; }
#define OP_F(name)                                                             \
    double op_##name(double src, double __attribute__((unused)) dst)
double fmove_from(double src, double) {
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
static fpu_op_t fpu_op_t_table[0x80] = {
    fmove_from, op_fint,    op_fsinh,   op_fintrz,  op_fsqrt,
    nullptr,    op_flognp1, nullptr,    op_fetoxm1, op_ftanh,
    op_fatan,   nullptr,    op_fasin,   op_fatanh,  op_fsin,
    op_ftan,    op_fetox,   op_ftwotox, op_ftentox,

};

OP(fpu_op) {
    regs.fpiar = regs.opc;
    uint16_t op2 = FETCH();
    bool rm = op2 >> 14 & 1;
    int src_s = op2 >> 10 & 7;
    int dst_r = op2 >> 7 & 7;
    int opc2 = op2 & 0x7f;
    double src, dst = regs.fp[dst_r];
    feclearexcept(FE_ALL_EXCEPT);
    if(rm) {
        switch(src_s) {
        case 0:
            src =
                static_cast<double>(static_cast<int32_t>(EA_READ32(type, reg)));
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
            src =
                static_cast<double>(static_cast<int16_t>(EA_READ16(type, reg)));
            break;
        case 5: {
            uint32_t addr = EA_Addr(type, reg, 8, false);
            src = load_double(addr);
            break;
        }
        case 6:
            src = static_cast<double>(static_cast<int8_t>(EA_READ8(type, reg)));
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
        fpu_postprocess(dst_r, dst);
    } else {
        FP_UNDEF();
    }
}
void init_fpu_opc() {
    for(int i = 0; i < 075; ++i) {
        opc_map[0171000 | i] = op_fpu_op;
    }
}