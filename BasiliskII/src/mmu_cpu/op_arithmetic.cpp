#include "cpu_emulation.h"
#include "main.h"
#include "sysdeps.h"
#include <limits.h>

#include "ea.h"
#include "exception.h"
#include "intop.h"
#include "newcpu.h"

OP(cmp2_chk2_b) {
    uint16_t op2 = FETCH();
    regs.i_ea = EA_Addr(type, reg, 0, false);
    uint8_t low = read8(regs.i_ea);
    uint8_t high = read8(regs.i_ea + 1);
    if(op2 & 0x8000) {
        // An vs Rn
        int32_t lowa = (int8_t)low;
        int32_t higha = (int8_t)high;
        int32_t vx = regs.a[op2 >> 12 & 7];
        regs.z = vx == lowa || vx == higha;
        regs.c = vx < lowa || vx > higha;
    } else {
        uint8_t v = regs.d[op2 >> 12 & 7];
        regs.z = v == low || v == high;
        regs.c = v < low || v > high;
    }
    if((op2 & 0x800) && regs.c) {
        CHK_ERROR();
    }
}

OP(cmp2_chk2_w) {
    uint16_t op2 = FETCH();
    regs.i_ea = EA_Addr(type, reg, 0, false);
    uint16_t low = read16(regs.i_ea);
    uint16_t high = read16(regs.i_ea + 2);
    if(op2 & 0x8000) {
        // An vs Rn
        int32_t lowa = (int16_t)low;
        int32_t higha = (int16_t)high;
        int32_t vx = regs.a[op2 >> 12 & 7];
        regs.z = vx == lowa || vx == higha;
        regs.c = vx < lowa || vx > higha;
    } else {
        uint16_t v = regs.d[op2 >> 12 & 7];
        regs.z = v == low || v == high;
        regs.c = v < low || v > high;
    }
    if((op2 & 0x800) && regs.c) {
        CHK_ERROR();
    }
}

OP(subi_b) {
    uint8_t v2 = FETCH();
    EA_Update8(type, reg, [v2](auto v1) { return DO_SUB_B(v1, v2); });
}

OP(subi_w) {
    uint16_t v2 = FETCH();
    EA_Update16(type, reg, [v2](auto v1) { return DO_SUB_W(v1, v2); });
}

OP(subi_l) {
    uint32_t v2 = FETCH32();
    EA_Update32(type, reg, [v2](auto v1) { return DO_SUB_L(v1, v2); });
}

OP(cmp2_chk2_l) {
    uint32_t op2 = FETCH();
    regs.i_ea = EA_Addr(type, reg, 0, false);
    uint32_t low = read32(regs.i_ea);
    uint32_t high = read32(regs.i_ea + 4);
    if(op2 & 0x8000) {
        // An vs Rn
        int32_t lowa = low;
        int32_t higha = high;
        int32_t vx = regs.a[op2 >> 12 & 7];
        regs.z = vx == lowa || vx == higha;
        regs.c = vx < lowa || vx > higha;
    } else {
        uint32_t v = regs.d[op2 >> 12 & 7];
        regs.z = v == low || v == high;
        regs.c = v < low || v > high;
    }
    if((op2 & 0x800) && regs.c) {
        CHK_ERROR();
    }
}

OP(addi_b) {
    uint8_t v2 = FETCH();
    EA_Update8(type, reg, [v2](auto v1) { return DO_ADD_B(v1, v2); });
}

OP(addi_w) {
    uint16_t v2 = FETCH();
    EA_Update16(type, reg, [v2](auto v1) { return DO_ADD_W(v1, v2); });
}

OP(addi_l) {
    uint32_t v2 = FETCH32();
    EA_Update32(type, reg, [v2](auto v1) { return DO_ADD_L(v1, v2); });
}

OP(cas_b) {
    uint16_t op2 = FETCH();
    regs.traced = true;
    regs.i_ea = EA_Addr(type, reg, 1, true);
    uint8_t d = read8(regs.i_ea);
    DO_CMP_B(d, regs.d[op2 & 7]);
    if(regs.z) {
        write8(regs.i_ea, regs.d[op2 >> 6 & 7]);
    } else {
        WRITE_D8(op2 & 7, d);
    }
}

OP(cas_w) {
    if(type == 7 && reg == 4) {
        uint16_t op2 = FETCH();
        uint16_t op3 = FETCH();
        uint32_t rn1 =
            op2 >> 15 ? regs.a[op2 >> 12 & 7] : regs.d[op2 >> 12 & 7];
        uint32_t rn2 =
            op3 >> 15 ? regs.a[op3 >> 12 & 7] : regs.d[op3 >> 12 & 7];
        int du1 = op2 >> 6 & 7;
        int dc1 = op2 & 7;
        int du2 = op3 >> 6 & 7;
        int dc2 = op3 & 7;
        regs.traced = true;
        uint16_t v1 = read16(rn1);
        uint16_t v2 = read16(rn2);
        DO_CMP_W(v1, regs.d[dc1]);
        if(regs.z) {
            DO_CMP_W(v2, regs.d[dc2]);
            if(regs.z) {
                write16(rn1, regs.d[du1]);
                write16(rn2, regs.d[du2]);
                return;
            }
        }
        WRITE_D16(dc1, v1);
        WRITE_D16(dc2, v2);
    } else {
        uint16_t op2 = FETCH();
        regs.traced = true;
        regs.i_ea = EA_Addr(type, reg, 2, true);
        uint16_t d = read16(regs.i_ea);
        DO_CMP_W(d, regs.d[op2 & 7]);
        if(regs.z) {
            write16(regs.i_ea, regs.d[op2 >> 6 & 7]);
        } else {
            WRITE_D16(op2 & 7, d);
        }
    }
}

OP(cas_l) {
    if(type == 7 && reg == 4) {
        uint16_t op2 = FETCH();
        uint16_t op3 = FETCH();
        uint32_t rn1 = regs.r[op2 >> 12 & 15];
        uint32_t rn2 = regs.r[op3 >> 12 & 15];
        int du1 = op2 >> 6 & 7;
        int dc1 = op2 & 7;
        int du2 = op3 >> 6 & 7;
        int dc2 = op3 & 7;
        regs.traced = true;
        uint32_t v1 = read32(rn1);
        uint32_t v2 = read32(rn2);
        DO_CMP_L(v1, regs.d[dc1]);
        if(regs.z) {
            DO_CMP_L(v2, regs.d[dc2]);
            if(regs.z) {
                write32(rn1, regs.d[du1]);
                write32(rn2, regs.d[du2]);
                return;
            }
        }
        regs.d[dc1] = v1;
        regs.d[dc2] = v2;
    } else {
        uint16_t op2 = FETCH();
        regs.traced = true;
        regs.i_ea = EA_Addr(type, reg, 4, true);
        uint32_t d = read32(regs.i_ea);
        DO_CMP_L(d, regs.d[op2 & 7]);
        if(regs.z) {
            write32(regs.i_ea, regs.d[op2 >> 6 & 7]);
        } else {
            EA_WRITE32(0, op2 & 7, d);
        }
    }
}

OP(cmpi_b) {
    uint8_t v2 = FETCH();
    uint8_t v1 = EA_READ8(type, reg);
    DO_CMP_B(v1, v2);
}

OP(cmpi_w) {
    uint16_t v2 = FETCH();
    uint16_t v1 = EA_READ16(type, reg);
    DO_CMP_W(v1, v2);
}

OP(cmpi_l) {
    uint32_t v2 = FETCH32();
    uint32_t v1 = EA_READ32(type, reg);
    DO_CMP_L(v1, v2);
}

OP(negx_b) {
    EA_Update8(type, reg, [](auto v1) {
        bool tp = regs.z;
        uint8_t v = DO_SUB_B(0, v1 + regs.x);
        if(v) {
            regs.z = false;
        } else {
            regs.z = tp;
        }
        return v;
    });
}

OP(negx_w) {
    EA_Update16(type, reg, [](auto v1) {
        bool tp = regs.z;
        uint16_t v = DO_SUB_W(0, v1 + regs.x);
        if(v) {
            regs.z = false;
        } else {
            regs.z = tp;
        }
        return v;
    });
}

OP(negx_l) {
    EA_Update32(type, reg, [](auto v1) {
        bool tp = regs.z;
        uint32_t v = DO_SUB_L(0, v1 + regs.x);
        if(v) {
            regs.z = false;
        } else {
            regs.z = tp;
        }
        return v;
    });
}

OP(clr_b) {
    EA_WRITE8(type, reg, 0);
    regs.n = regs.v = regs.c = false;
    regs.z = true;
}

OP(clr_w) {
    EA_WRITE16(type, reg, 0);
    regs.n = regs.v = regs.c = false;
    regs.z = true;
}

OP(clr_l) {
    EA_WRITE32(type, reg, 0);
    regs.n = regs.v = regs.c = false;
    regs.z = true;
}

OP(neg_b) {
    EA_Update8(type, reg, [](auto v1) { return DO_SUB_B(0, v1); });
}

OP(neg_w) {
    EA_Update16(type, reg, [](auto v1) { return DO_SUB_W(0, v1); });
}

OP(neg_l) {
    EA_Update32(type, reg, [](auto v1) { return DO_SUB_L(0, v1); });
}

void op_ext_w(int reg) {
    int8_t v = regs.d[reg];
    regs.v = regs.c = false;
    TEST_NZ16(v);
    WRITE_D16(reg, DO_EXT_W(v));
}

void op_ext_l(int reg) {
    int16_t v = regs.d[reg];
    regs.v = regs.c = false;
    TEST_NZ32(v);
    EA_WRITE32(0, reg, DO_EXT_L(v));
}

OP(mul_l) {
    uint16_t op2 = FETCH();
    int dl = op2 >> 12 & 7;
    int dh = op2 & 7;
    regs.c = false;
    if(op2 & 1 << 11) {
        // muls
        int64_t v1 = static_cast<int32_t>(EA_READ32(type, reg));
        int64_t v2 = static_cast<int32_t>(regs.d[dl]);
        int64_t ret = v1 * v2;
        regs.n = ret < 0;
        regs.z = ret == 0;
        regs.d[dl] = static_cast<uint32_t>(ret & 0xffffffff);
        int32_t high = ret >> 32;
        if(op2 & 1 << 10) {
            regs.d[dh] = high;
        } else {
            regs.v = ret > INT32_MAX || ret < INT32_MIN;
        }
    } else {
        // mulu
        uint64_t v1 = EA_READ32(type, reg);
        uint64_t v2 = regs.d[dl];
        uint64_t ret = v1 * v2;
        regs.n = ret >> 63 & 1;
        regs.z = ret == 0;
        regs.d[dl] = static_cast<uint32_t>(ret);
        uint32_t high = ret >> 32;
        if(op2 & 1 << 10) {
            regs.d[dh] = high;
        } else {
            regs.v = high != 0;
        }
    }
}

uint64_t do_divs(int32_t ah, uint32_t al, int32_t b) {
    int64_t a = static_cast<int64_t>(ah) << 32 | al;
    if(b == 0) {
        DIV0_ERROR();
        return a;
    }
    int64_t ret1 = a / b;
    int64_t ret2 = a % b;
    regs.n = ret1 < 0;
    regs.z = ret1 == 0;
    if(ret1 > INT32_MAX || ret1 < INT32_MIN) {
        regs.v = true;
        return a;
    } else {
        regs.v = false;
        return static_cast<uint64_t>(ret1 & 0xffffffff) |
               static_cast<uint64_t>(ret2 & 0xffffffff) << 32;
    }
}

uint64_t do_divu(uint32_t ah, uint32_t al, uint32_t b) {
    uint64_t a = static_cast<uint64_t>(ah) << 32 | al;
    if(b == 0) {
        DIV0_ERROR();
        return a;
    }
    uint64_t ret1 = a / b;
    uint64_t ret2 = a % b;
    regs.n = ret1 >> 31;
    regs.z = ret1 == 0;
    if(ret1 > 0xffffffff) {
        regs.v = true;
        return a;
    } else {
        regs.v = false;
        return (ret1 & 0xffffffff) | (ret2 & 0xffffffff) << 32;
    }
}
OP(div_l) {
    uint16_t op2 = FETCH();
    int dq = op2 >> 12 & 7;
    int dr = op2 & 7;
    regs.c = false;
    uint32_t vd = EA_READ32(type, reg);
    uint64_t ret;
    if(op2 & 1 << 11) {
        // divs
        int32_t ah = (op2 & 1 << 10) ? regs.d[dr] : (static_cast<int32_t>(regs.d[dq]) >> 31); 
        ret = do_divs(ah, regs.d[dq], vd);
    } else {
        int32_t ah = (op2 & 1 << 10) ? regs.d[dr] : 0; 
        ret = do_divu(ah, regs.d[dq], vd);
    }
    regs.d[dq] = ret & 0xffffffff;
    if(dq != dr) {
        regs.d[dr] = ret >> 32;
    }
}

OP(chk_w) {
    int16_t limit = EA_READ16(type, reg);
    int16_t v = regs.d[dm];
    if(v < 0) {
        regs.n = true;
        CHK_ERROR();
    }
    if(v > limit) {
        regs.n = false;
        CHK_ERROR();
    }
}

OP(chk_l) {
    int32_t limit = EA_READ32(type, reg);
    int32_t v = regs.d[dm];
    if(v < 0) {
        regs.n = true;
        CHK_ERROR();
    }
    if(v > limit) {
        regs.n = false;
        CHK_ERROR();
    }
}

void op_extb(int reg) {
    uint32_t v = DO_EXTB_L(regs.d[reg]);
    regs.v = regs.c = false;
    TEST_NZ32(v);
    EA_WRITE32(0, reg, v);
}

OP(addq_b) {
    int v2 = dm ? dm : 8;
    EA_Update8(type, reg, [v2](auto v1) { return DO_ADD_B(v1, v2); });
}

OP(addq_w) {
    int v2 = dm ? dm : 8;
    if(type == 1) {
        regs.a[reg] += v2;
    } else {
        EA_Update16(type, reg, [v2](auto v1) { return DO_ADD_W(v1, v2); });
    }
}

OP(addq_l) {
    int v2 = dm ? dm : 8;
    if(type == 1) {
        regs.a[reg] += v2;
    } else {
        EA_Update32(type, reg, [v2](auto v1) { return DO_ADD_L(v1, v2); });
    }
}

OP(subq_b) {
    int v2 = dm ? dm : 8;
    EA_Update8(type, reg, [v2](auto v1) { return DO_SUB_B(v1, v2); });
}

OP(subq_w) {
    int v2 = dm ? dm : 8;
    if(type == 1) {
        regs.a[reg] -= v2;
    } else {
        EA_Update16(type, reg, [v2](auto v1) { return DO_SUB_W(v1, v2); });
    }
}

OP(subq_l) {
    int v2 = dm ? dm : 8;
    if(type == 1) {
        regs.a[reg] -= v2;
    } else {
        EA_Update32(type, reg, [v2](auto v1) { return DO_SUB_L(v1, v2); });
    }
}

OP(divu_w) {
    uint32_t dx = regs.d[dm];
    uint32_t dy = EA_READ16(type, reg);
    if(dy == 0) {
        DIV0_ERROR();
        return;
    }
    regs.c = false;
    uint32_t q = dx / dy;
    uint32_t r = dx % dy;
    regs.v = (q >> 16) != 0;
    regs.d[dm] = (q & 0xffff) | (r << 16);
    regs.n = q >> 15;
    regs.z = q == 0;
}

OP(divs_w) {
    int32_t dx = regs.d[dm];
    int32_t dy = DO_EXT_L(EA_READ16(type, reg));
    if(dy == 0) {
        DIV0_ERROR();
        return;
    }
    regs.c = false;
    int32_t q = dx / dy;
    int32_t r = dx % dy;
    regs.v = q > std::numeric_limits<short>::max() ||
             q < std::numeric_limits<short>::min();
    regs.d[dm] = (q & 0xffff) | static_cast<uint16_t>(r) << 16;
    regs.n = q < 0;
    regs.z = q == 0;
}

OP(sub_b) {
    uint8_t v2 = EA_READ8(type, reg);
    WRITE_D8(dm, DO_SUB_B(regs.d[dm], v2));
}

OP(sub_w) {
    uint16_t v2 = EA_READ16(type, reg);
    WRITE_D16(dm, DO_SUB_W(regs.d[dm], v2));
}

OP(sub_l) {
    uint32_t v2 = EA_READ32(type, reg);
    regs.d[dm] = DO_SUB_L(regs.d[dm], v2);
}

OP(suba_w) {
    int16_t v2 = EA_READ16(type, reg);
    WRITE_A16(dm, regs.a[dm] - v2);
}

void subx_b_d(int dst_r, int src_r) {
    uint8_t src = regs.d[src_r];
    uint8_t dst = regs.d[dst_r];
    bool oldz = regs.z;
    uint8_t v = DO_SUB_B(dst, src + regs.x);
    if(v) {
        regs.z = false;
    } else {
        regs.z = oldz;
    }
    WRITE_D8(dst_r, v);
}

void subx_b_a(int dst_r, int src_r) {
    uint8_t src = read8(--regs.a[src_r]);
    uint8_t dst = read8(--regs.a[dst_r]);
    bool oldz = regs.z;
    uint8_t v = DO_SUB_B(dst, src + regs.x);
    if(v) {
        regs.z = false;
    } else {
        regs.z = oldz;
    }
    write8(regs.a[dst_r], v);
}
OP(sub_to_ea_b) {
    if(type == 0) {
        // SUBX.B
        subx_b_d(dm, reg);
    } else if(type == 1) {
        // SUBX.B
        subx_b_a(dm, reg);
    } else {
        uint8_t v2 = regs.d[dm];
        EA_Update8(type, reg, [v2](auto v1) { return DO_SUB_B(v1, v2); });
    }
}
void subx_w_d(int dst_r, int src_r) {
    uint16_t src = regs.d[src_r];
    uint16_t dst = regs.d[dst_r];
    bool oldz = regs.z;
    uint16_t v = DO_SUB_W(dst, src + regs.x);
    if(v) {
        regs.z = false;
    } else {
        regs.z = oldz;
    }
    WRITE_D16(dst_r, v);
}

void subx_w_a(int dst_r, int src_r) {
    uint16_t src = read16(regs.a[src_r] -= 2);
    uint16_t dst = read16(regs.a[dst_r] -= 2);
    bool oldz = regs.z;
    uint16_t v = DO_SUB_W(dst, src + regs.x);
    if(v) {
        regs.z = false;
    } else {
        regs.z = oldz;
    }
    write16(regs.a[dst_r], v);
}

OP(sub_to_ea_w) {
    if(type == 0) {
        // SUBX.W
        subx_w_d(dm, reg);
    } else if(type == 1) {
        // SUBX.W
        subx_w_a(dm, reg);
    } else {
        uint16_t v2 = regs.d[dm];
        EA_Update16(type, reg, [v2](auto v1) { return DO_SUB_W(v1, v2); });
    }
}

void subx_l_d(int dst_r, int src_r) {
    uint32_t src = regs.d[src_r];
    uint32_t dst = regs.d[dst_r];
    bool oldz = regs.z;
    uint32_t v = DO_SUB_L(dst, src + regs.x);
    if(v) {
        regs.z = false;
    } else {
        regs.z = oldz;
    }
    regs.d[dst_r] = v;
}

void subx_l_a(int dst_r, int src_r) {
    uint32_t src = read32(regs.a[src_r] -= 4);
    uint32_t dst = read32(regs.a[dst_r] -= 4);
    bool oldz = regs.z;
    uint32_t v = DO_SUB_L(dst, src + regs.x);
    if(v) {
        regs.z = false;
    } else {
        regs.z = oldz;
    }
    write32(regs.a[dst_r], v);
}

OP(sub_to_ea_l) {
    if(type == 0) {
        // SUBX.L
        subx_l_d(dm, reg);
    } else if(type == 1) {
        // SUBX.L
        subx_l_a(dm, reg);
    } else {
        uint32_t v2 = regs.d[dm];
        EA_Update32(type, reg, [v2](auto v1) { return DO_SUB_L(v1, v2); });
    }
}

OP(suba_l) {
    int32_t v2 = EA_READ32(type, reg);
    regs.a[dm] -= v2;
}

OP(cmp_b) {
    uint8_t v2 = EA_READ8(type, reg);
    DO_CMP_B(regs.d[dm], v2);
}

OP(cmp_w) {
    uint16_t v2 = EA_READ16(type, reg);
    DO_CMP_W(regs.d[dm], v2);
}

OP(cmp_l) {
    uint32_t v2 = EA_READ32(type, reg);
    DO_CMP_L(regs.d[dm], v2);
}

OP(cmpa_w) {
    uint16_t v2 = EA_READ16(type, reg);
    DO_CMP_W(regs.a[dm], v2);
}

OP(cmpa_l) {
    uint32_t v2 = EA_READ32(type, reg);
    DO_CMP_L(regs.a[dm], v2);
}

void op_cmpm_b(int y, int x) {
    uint8_t v_y = read8(regs.a[y]);
    uint8_t v_x = read8(regs.a[x]);
    DO_CMP_B(v_x, v_y);
    ++regs.a[y];
    ++regs.a[x];
}
void op_cmpm_w(int y, int x) {
    uint16_t v_y = read16(regs.a[y]);
    uint16_t v_x = read16(regs.a[x]);
    DO_CMP_W(v_x, v_y);
    regs.a[y] += 2;
    regs.a[x] += 2;
}
void op_cmpm_l(int y, int x) {
    uint32_t v_y = read32(regs.a[y]);
    uint32_t v_x = read32(regs.a[x]);
    DO_CMP_L(v_x, v_y);
    regs.a[y] += 4;
    regs.a[x] += 4;
}
OP(mulu_w) {
    uint32_t v2 = EA_READ16(type, reg);
    uint32_t v1 = regs.d[dm] & 0xffff;
    uint32_t re = v1 * v2;
    TEST_NZ32(re);
    regs.v = false;
    regs.c = false;
    regs.d[dm] = re;
}

OP(muls_w) {
    int32_t v2 = DO_EXT_L(EA_READ16(type, reg));
    int32_t v1 = DO_EXT_L(regs.d[dm] & 0xffff);
    int32_t re = v1 * v2;
    TEST_NZ32(re);
    regs.v = false;
    regs.c = false;
    regs.d[dm] = re;
}

OP(add_b) {
    uint8_t v2 = EA_READ8(type, reg);
    WRITE_D8(dm, DO_ADD_B(regs.d[dm], v2));
}

OP(add_w) {
    uint16_t v2 = EA_READ16(type, reg);
    WRITE_D16(dm, DO_ADD_W(regs.d[dm], v2));
}

OP(add_l) {
    uint32_t v2 = EA_READ32(type, reg);
    regs.d[dm] = DO_ADD_L(regs.d[dm], v2);
}

OP(adda_w) {
    int16_t v2 = EA_READ16(type, reg);
    regs.a[dm] += v2;
}
uint8_t do_addx_b(uint8_t a, uint8_t b) {
    bool oldz = regs.z;
    uint8_t v = DO_ADD_B(a, b + regs.x);
    if(v) {
        regs.z = false;
    } else {
        regs.z = oldz;
    }
    return v;
}
void addx_b_d(int dm, int reg) {
    uint8_t src = regs.d[reg];
    uint8_t dst = regs.d[dm];
    WRITE_D8(dm, do_addx_b(src, dst));
}

void addx_b_a(int dm, int reg) {
    uint8_t src = read8(--regs.a[reg]);
    uint8_t dst = read8(--regs.a[dm]);
    write8(regs.a[dm], do_addx_b(src, dst));
}

uint16_t do_addx_w(uint16_t a, uint16_t b) {
    bool oldz = regs.z;
    uint16_t v = DO_ADD_W(a, b + regs.x);
    if(v) {
        regs.z = false;
    } else {
        regs.z = oldz;
    }
    return v;
}
void addx_w_d(int dm, int reg) {
    uint16_t src = regs.d[reg];
    uint16_t dst = regs.d[dm];
    WRITE_D16(dm, do_addx_w(src, dst));
}

void addx_w_a(int dm, int reg) {
    uint16_t src = read16(regs.a[reg] -= 2);
    uint16_t dst = read16(regs.a[dm] -= 2);
    write16(regs.a[dm], do_addx_w(src, dst));
}

uint32_t do_addx_l(uint32_t a, uint32_t b) {
    bool oldz = regs.z;
    uint32_t v = DO_ADD_L(a, b + regs.x);
    if(v) {
        regs.z = false;
    } else {
        regs.z = oldz;
    }
    return v;
}
void addx_l_d(int dm, int reg) {
    uint32_t src = regs.d[reg];
    uint32_t dst = regs.d[dm];
    regs.d[dm] = do_addx_l(dst, src);
}

void addx_l_a(int dm, int reg) {
    uint32_t src = read32(regs.a[reg] -= 4);
    uint32_t dst = read32(regs.a[dm] -= 4);
    write32(regs.a[dm], do_addx_l(dst, src));
}

OP(add_to_ea_b) {
    if(type == 0) {
        addx_b_d(dm, reg);
    } else if(type == 1) {
        addx_b_a(dm, reg);
    } else {
        uint8_t v2 = regs.d[dm];
        EA_Update8(type, reg, [v2](auto v1) { return DO_ADD_B(v1, v2); });
    }
}

OP(add_to_ea_w) {
    if(type == 0) {
        addx_w_d(dm, reg);
    } else if(type == 1) {
        addx_w_a(dm, reg);
    } else {
        uint16_t v2 = regs.d[dm];
        EA_Update16(type, reg, [v2](auto v1) { return DO_ADD_W(v1, v2); });
    }
}

OP(add_to_ea_l) {
    if(type == 0) {
        addx_l_d(dm, reg);
    } else if(type == 1) {
        addx_l_a(dm, reg);
    } else {
        uint32_t v2 = regs.d[dm];
        EA_Update32(type, reg, [v2](auto v1) { return DO_ADD_L(v1, v2); });
    }
}

OP(adda_l) {
    int32_t v2 = EA_READ32(type, reg);
    regs.a[dm] += v2;
}
