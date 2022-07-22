#include "cpu_emulation.h"
#include "main.h"
#include "sysdeps.h"
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>

#include "ea.h"
#include "exception.h"
#include "intop.h"
#include "newcpu.h"
#include "op.h"
OP(ori_b) {
    uint8_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        SET_CCR(GET_CCR() | v2);
    } else {
        EA_Update8(type, reg, [v2](auto v1) { return DO_OR_B(v1, v2); });
    }
}
OP(ori_w) {
    uint16_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        if(!regs.S) {
            PRIV_ERROR();
            return;
        }
        regs.traced = true;
        SET_SR(GET_SR() | v2);
    } else {
        EA_Update16(type, reg, [v2](auto v1) { return DO_OR_W(v1, v2); });
    }
}

OP(ori_l) {
    uint32_t v2 = FETCH32();
    EA_Update32(type, reg, [v2](auto v1) { return DO_OR_L(v1, v2); });
}

OP(andi_b) {
    uint8_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        SET_CCR(GET_CCR() & v2);
    } else {
        EA_Update8(type, reg, [v2](auto v1) { return DO_AND_B(v1, v2); });
    }
}

OP(andi_w) {
    uint16_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        if(!regs.S) {
            PRIV_ERROR();
            return;
        }
        regs.traced = true;
        SET_SR(GET_SR() & v2);
    } else {
        EA_Update16(type, reg, [v2](auto v1) { return DO_AND_W(v1, v2); });
    }
}

OP(andi_l) {
    uint32_t v2 = FETCH32();
    EA_Update32(type, reg, [v2](auto v1) { return DO_AND_L(v1, v2); });
}

OP(btst_imm) {
    if(type == 0) {
        int bn = FETCH() & 31;
        uint32_t v1 = regs.d[reg];
        regs.z = DO_BTST(v1, bn);
    } else {
        int bn = FETCH() & 7;
        uint8_t v1 = EA_READ8(type, reg);
        regs.z = DO_BTST(v1, bn);
    }
}

OP(bchg_imm) {
    if(type == 0) {
        int bn = FETCH() & 31;
        uint32_t v1 = regs.d[reg];
        regs.d[reg] = DO_BCHG_L(v1, bn);
    } else {
        int bn = FETCH() & 7;
        EA_Update8(type, reg, [bn](auto v1) { return DO_BCHG_B(v1, bn); });
    }
}

OP(bclr_imm) {
    if(type == 0) {
        int bn = FETCH() & 31;
        uint32_t v1 = regs.d[reg];
        regs.d[reg] = DO_BCLR_L(v1, bn);
    } else {
        int bn = FETCH() & 7;
        EA_Update8(type, reg, [bn](auto v1) { return DO_BCLR_B(v1, bn); });
    }
}

OP(bset_imm) {
    if(type == 0) {
        int bn = FETCH() & 31;
        uint32_t v1 = regs.d[reg];
        regs.z = DO_BTST(v1, bn);
        regs.d[reg] |= (1 << bn);
    } else {
        int bn = FETCH() & 7;
        EA_Update8(type, reg, [bn](auto v1) {
            regs.z = DO_BTST(v1, bn);
            return v1 | (1 << bn);
        });
    }
}
OP(eori_b) {
    uint8_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        SET_CCR(GET_CCR() ^ v2);
    } else {
        EA_Update8(type, reg, [v2](auto v1) { return DO_EOR_B(v1, v2); });
    }
}


OP(eori_w) {
    uint16_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        if(!regs.S) {
            PRIV_ERROR();
            return;
        }
        regs.traced = true;
        SET_SR(GET_SR() ^ v2);
    } else {
        EA_Update16(type, reg, [v2](auto v1) { return DO_EOR_W(v1, v2); });
    }
}

OP(eori_l) {
    uint32_t v2 = FETCH32();
    EA_Update32(type, reg, [v2](auto v1) { return DO_EOR_L(v1, v2); });
}

OP(btst_dm) {
    if(type == 0) {
        uint32_t v1 = regs.d[reg];
        int bn = regs.d[dm] & 31;
        regs.z = DO_BTST(v1, bn);
    } else if(type == 1) {
        op_movep_w_from(dm, reg);
    } else {
        uint8_t v1 = EA_READ8(type, reg);
        int bn = regs.d[dm] & 7;
        regs.z = DO_BTST(v1, bn);
    }
}

OP(bchg_dm) {
    if(type == 0) {
        int bn = regs.d[dm] & 31;
        uint32_t v1 = regs.d[reg];
        regs.d[reg] = DO_BCHG_L(v1, bn);
    } else if(type == 1) {
        op_movep_l_from(dm, reg);
    } else {
        int bn = regs.d[dm] & 7;
        EA_Update8(type, reg, [bn](auto v1) { return DO_BCHG_B(v1, bn); });
    }
}

OP(bclr_dm) {
    if(type == 0) {
        uint32_t v1 = regs.d[reg];
        int bn = regs.d[dm] & 31;
        regs.d[reg] = DO_BCLR_L(v1, bn);
    } else if(type == 1) {
        op_movep_w_to(dm, reg);
    } else {
        int bn = regs.d[dm] & 7;
        EA_Update8(type, reg, [bn](auto v1) { return DO_BCLR_B(v1, bn); });
    }
}

OP(bset_dm) {
    if(type == 0) {
        uint32_t v1 = regs.d[reg];
        int bn = regs.d[dm] & 31;
        regs.z = DO_BTST(v1, bn);
        regs.d[reg] |= (1 << bn);
    } else if(type == 1) {
        op_movep_l_to(dm, reg);
    } else {
        int bn = regs.d[dm] & 7;
        EA_Update8(type, reg, [bn](auto v1) {
            regs.z = DO_BTST(v1, bn);
            return v1 | (1 << bn);
        });
    }
}

OP(not_b) {
    EA_Update8(type, reg, [](auto v1) { return DO_NOT_B(v1); });
}

OP(not_w) {
    EA_Update16(type, reg, [](auto v1) { return DO_NOT_W(v1); });
}

OP(not_l) {
    EA_Update32(type, reg, [](auto v1) { return DO_NOT_L(v1); });
}

void op_swap(int reg) {
    uint32_t v = regs.d[reg];
    v = (v >> 16) | (v << 16);
    TEST_NZ32(v);
    regs.v = regs.c = false;
    regs.d[reg] = v;
}

OP(tst_b) {
    uint8_t v = EA_READ8(type, reg);
    TEST_NZ8(v);
    regs.v = false;
    regs.c = false;
}

OP(tst_w) {
    uint16_t v = EA_READ16(type, reg);
    TEST_NZ16(v);
    regs.v = false;
    regs.c = false;
}

OP(tst_l) {
    uint32_t v = EA_READ32(type, reg);
    TEST_NZ32(v);
    regs.v = false;
    regs.c = false;
}

OP(tas) {
    if(type == 7 && reg == 4) {
        ILLEGAL_INST();
    } else {
        EA_Update8(type, reg, [](uint8_t v) {
            TEST_NZ8(v);
            regs.v = false;
            regs.c = false;
            return v | 0x80;
        });
    }
}

OP(or_b) {
    uint8_t v2 = EA_READ8(type, reg);
    WRITE_D8(dm, DO_OR_B(regs.d[dm], v2));
}

OP(or_w) {
    uint16_t v2 = EA_READ16(type, reg);
    WRITE_D16(dm, DO_OR_W(regs.d[dm], v2));
}

OP(or_l) {
    uint32_t v2 = EA_READ32(type, reg);
    regs.d[dm] = DO_OR_L(regs.d[dm], v2);
}

OP(or_to_ea_b) {
    if(type == 0) {
        op_sbcd_d(dm, reg);
    } else if(type == 1) {
        op_sbcd_a(dm, reg);
    } else {
        uint8_t v2 = regs.d[dm];
        EA_Update8(type, reg, [v2](auto v1) { return DO_OR_B(v1, v2); });
    }
}
OP(or_to_ea_w) {
    if(type == 0) {
        op_pack_d(dm, reg);
    } else if(type == 1) {
        op_pack_a(dm, reg);
    } else {
        uint16_t v2 = regs.d[dm];
        EA_Update16(type, reg, [v2](auto v1) { return DO_OR_W(v1, v2); });
    }
}
OP(or_to_ea_l) {
    if(type == 0) {
        op_unpk_d(dm, reg);
    } else if(type == 1) {
        op_unpk_a(dm, reg);
    } else {
        uint32_t v2 = regs.d[dm];
        EA_Update32(type, reg, [v2](auto v1) { return DO_OR_L(v1, v2); });
    }
}

OP(eor_b) {
    if(type == 1) {
        op_cmpm_b(reg, dm);
    } else {
        uint8_t v2 = regs.d[dm];
        EA_Update8(type, reg, [v2](auto v1) { return DO_EOR_B(v1, v2); });
    }
}
OP(eor_w) {
    if(type == 1) {
        op_cmpm_w(reg, dm);
    } else {
        uint16_t v2 = regs.d[dm];
        EA_Update16(type, reg, [v2](auto v1) { return DO_EOR_W(v1, v2); });
    }
}
OP(eor_l) {
    if(type == 1) {
        op_cmpm_l(reg, dm);
    } else {
        uint32_t v2 = regs.d[dm];
        EA_Update32(type, reg, [v2](auto v1) { return DO_EOR_L(v1, v2); });
    }
}

OP(and_b) {
    uint8_t v2 = EA_READ8(type, reg);
    WRITE_D8(dm, DO_AND_B(regs.d[dm], v2));
}

OP(and_w) {
    uint16_t v2 = EA_READ16(type, reg);
    WRITE_D16(dm, DO_AND_W(regs.d[dm], v2));
}

OP(and_l) {
    uint32_t v2 = EA_READ32(type, reg);
    regs.d[dm] = DO_AND_L(regs.d[dm], v2);
}

OP(and_to_ea_b) {
    if(type == 0) {
        op_abcd_d(dm, reg);
    } else if(type == 1) {
        op_abcd_a(dm, reg);
    } else {
        uint8_t v2 = regs.d[dm];
        EA_Update8(type, reg, [v2](auto v1) { return DO_AND_B(v1, v2); });
    }
}

OP(and_to_ea_w) {
    if(type == 0) {
        op_exg_d(dm, reg);
    } else if(type == 1) {
        op_exg_a(dm, reg);
    } else {
        uint16_t v2 = regs.d[dm];
        EA_Update16(type, reg, [v2](auto v1) { return DO_AND_W(v1, v2); });
    }
}
OP(and_to_ea_l) {
    if(type == 0) {
        ILLEGAL_INST();
    } else if(type == 1) {
        op_exg_da(dm, reg);
    } else {
        uint32_t v2 = regs.d[dm];
        EA_Update32(type, reg, [v2](auto v1) { return DO_AND_L(v1, v2); });
    }
}


void op_asr_b_imm(int dm, int reg) {
    WRITE_D8(reg, DO_ASR_B(regs.d[reg], dm ? dm : 8));
}
void op_lsr_b_imm(int dm, int reg) {
    WRITE_D8(reg, DO_LSR_B(regs.d[reg], dm ? dm : 8));
}
void op_roxr_b_imm(int dm, int reg) {
    WRITE_D8(reg, DO_ROXR_B(regs.d[reg], dm ? dm : 8));
}
void op_ror_b_imm(int dm, int reg) {
    WRITE_D8(reg, DO_ROR_B(regs.d[reg], dm ? dm : 8));
}
void op_asr_b_reg(int dm, int reg) {
    WRITE_D8(reg, DO_ASR_B(regs.d[reg], regs.d[dm] & 63));
}
void op_lsr_b_reg(int dm, int reg) {
    WRITE_D8(reg, DO_LSR_B(regs.d[reg], regs.d[dm] & 63));
}
void op_roxr_b_reg(int dm, int reg) {
    WRITE_D8(reg, DO_ROXR_B(regs.d[reg], regs.d[dm] & 63));
}
void op_ror_b_reg(int dm, int reg) {
    WRITE_D8(reg, DO_ROR_B(regs.d[reg], regs.d[dm] & 63));
}
OP(shr_b) {
    switch(type) {
    case 0:
        op_asr_b_imm(dm, reg);
        break;
    case 1:
        op_lsr_b_imm(dm, reg);
        break;
    case 2:
        op_roxr_b_imm(dm, reg);
        break;
    case 3:
        op_ror_b_imm(dm, reg);
        break;
    case 4:
        op_asr_b_reg(dm, reg);
        break;
    case 5:
        op_lsr_b_reg(dm, reg);
        break;
    case 6:
        op_roxr_b_reg(dm, reg);
        break;
    case 7:
        op_ror_b_reg(dm, reg);
        break;
    }
}
void op_asr_w_imm(int dm, int reg) {
    WRITE_D16(reg, DO_ASR_W(regs.d[reg], dm ? dm : 8));
}
void op_lsr_w_imm(int dm, int reg) {
    WRITE_D16(reg, DO_LSR_W(regs.d[reg], dm ? dm : 8));
}
void op_roxr_w_imm(int dm, int reg) {
    WRITE_D16(reg, DO_ROXR_W(regs.d[reg], dm ? dm : 8));
}
void op_ror_w_imm(int dm, int reg) {
    WRITE_D16(reg, DO_ROR_W(regs.d[reg], dm ? dm : 8));
}
void op_asr_w_reg(int dm, int reg) {
    WRITE_D16(reg, DO_ASR_W(regs.d[reg], regs.d[dm] & 63));
}
void op_lsr_w_reg(int dm, int reg) {
    WRITE_D16(reg, DO_LSR_W(regs.d[reg], regs.d[dm] & 63));
}
void op_roxr_w_reg(int dm, int reg) {
    WRITE_D16(reg, DO_ROXR_W(regs.d[reg], regs.d[dm] & 63));
}
void op_ror_w_reg(int dm, int reg) {
    WRITE_D16(reg, DO_ROR_W(regs.d[reg], regs.d[dm] & 63));
}
OP(shr_w) {
    switch(type) {
    case 0:
        op_asr_w_imm(dm, reg);
        break;
    case 1:
        op_lsr_w_imm(dm, reg);
        break;
    case 2:
        op_roxr_w_imm(dm, reg);
        break;
    case 3:
        op_ror_w_imm(dm, reg);
        break;
    case 4:
        op_asr_w_reg(dm, reg);
        break;
    case 5:
        op_lsr_w_reg(dm, reg);
        break;
    case 6:
        op_roxr_w_reg(dm, reg);
        break;
    case 7:
        op_ror_w_reg(dm, reg);
        break;
    }
}
void op_asr_l_imm(int dm, int reg) {
    regs.d[reg] = DO_ASR_L(regs.d[reg], dm ? dm : 8);
}
void op_lsr_l_imm(int dm, int reg) {
    regs.d[reg] = DO_LSR_L(regs.d[reg], dm ? dm : 8);
}
void op_roxr_l_imm(int dm, int reg) {
    regs.d[reg] = DO_ROXR_L(regs.d[reg], dm ? dm : 8);
}
void op_ror_l_imm(int dm, int reg) {
    regs.d[reg] = DO_ROR_L(regs.d[reg], dm ? dm : 8);
}
void op_asr_l_reg(int dm, int reg) {
    regs.d[reg] = DO_ASR_L(regs.d[reg], regs.d[dm] & 63);
}
void op_lsr_l_reg(int dm, int reg) {
    regs.d[reg] = DO_LSR_L(regs.d[reg], regs.d[dm] & 63);
}
void op_roxr_l_reg(int dm, int reg) {
    regs.d[reg] = DO_ROXR_L(regs.d[reg], regs.d[dm] & 63);
}
void op_ror_l_reg(int dm, int reg) {
    regs.d[reg] = DO_ROR_L(regs.d[reg], regs.d[dm] & 63);
}
OP(shr_l) {
    switch(type) {
    case 0:
        op_asr_l_imm(dm, reg);
        break;
    case 1:
        op_lsr_l_imm(dm, reg);
        break;
    case 2:
        op_roxr_l_imm(dm, reg);
        break;
    case 3:
        op_ror_l_imm(dm, reg);
        break;
    case 4:
        op_asr_l_reg(dm, reg);
        break;
    case 5:
        op_lsr_l_reg(dm, reg);
        break;
    case 6:
        op_roxr_l_reg(dm, reg);
        break;
    case 7:
        op_ror_l_reg(dm, reg);
        break;
    }
}

OP(asr_ea) {
    EA_Update16(type, reg, [](auto v) { return DO_ASR_W(v, 1); });
}
OP(lsr_ea) {
    EA_Update16(type, reg, [](auto v) { return DO_LSR_W(v, 1); });
}
OP(roxr_ea) {
    EA_Update16(type, reg, [](auto v) { return DO_ROXR_W(v, 1); });
}
OP(ror_ea) {
    EA_Update16(type, reg, [](auto v) { return DO_ROR_W(v, 1); });
}

void op_asl_b_imm(int dm, int reg) {
    WRITE_D8(reg, DO_ASL_B(regs.d[reg], dm ? dm : 8));
}
void op_lsl_b_imm(int dm, int reg) {
    WRITE_D8(reg, DO_LSL_B(regs.d[reg], dm ? dm : 8));
}
void op_roxl_b_imm(int dm, int reg) {
    WRITE_D8(reg, DO_ROXL_B(regs.d[reg], dm ? dm : 8));
}
void op_rol_b_imm(int dm, int reg) {
    WRITE_D8(reg, DO_ROL_B(regs.d[reg], dm ? dm : 8));
}
void op_asl_b_reg(int dm, int reg) {
    WRITE_D8(reg, DO_ASL_B(regs.d[reg], regs.d[dm] & 63));
}
void op_lsl_b_reg(int dm, int reg) {
    WRITE_D8(reg, DO_LSL_B(regs.d[reg], regs.d[dm] & 63));
}
void op_roxl_b_reg(int dm, int reg) {
    WRITE_D8(reg, DO_ROXL_B(regs.d[reg], regs.d[dm] & 63));
}
void op_rol_b_reg(int dm, int reg) {
    WRITE_D8(reg, DO_ROL_B(regs.d[reg], regs.d[dm] & 63));
}
OP(shl_b) {
    switch(type) {
    case 0:
        op_asl_b_imm(dm, reg);
        break;
    case 1:
        op_lsl_b_imm(dm, reg);
        break;
    case 2:
        op_roxl_b_imm(dm, reg);
        break;
    case 3:
        op_rol_b_imm(dm, reg);
        break;
    case 4:
        op_asl_b_reg(dm, reg);
        break;
    case 5:
        op_lsl_b_reg(dm, reg);
        break;
    case 6:
        op_roxl_b_reg(dm, reg);
        break;
    case 7:
        op_rol_b_reg(dm, reg);
        break;
    }
}
void op_asl_w_imm(int dm, int reg) {
    WRITE_D16(reg, DO_ASL_W(regs.d[reg], dm ? dm : 8));
}
void op_lsl_w_imm(int dm, int reg) {
    WRITE_D16(reg, DO_LSL_W(regs.d[reg], dm ? dm : 8));
}
void op_roxl_w_imm(int dm, int reg) {
    WRITE_D16(reg, DO_ROXL_W(regs.d[reg], dm ? dm : 8));
}
void op_rol_w_imm(int dm, int reg) {
    WRITE_D16(reg, DO_ROL_W(regs.d[reg], dm ? dm : 8));
}
void op_asl_w_reg(int dm, int reg) {
    WRITE_D16(reg, DO_ASL_W(regs.d[reg], regs.d[dm] & 63));
}
void op_lsl_w_reg(int dm, int reg) {
    WRITE_D16(reg, DO_LSL_W(regs.d[reg], regs.d[dm] & 63));
}
void op_roxl_w_reg(int dm, int reg) {
    WRITE_D16(reg, DO_ROXL_W(regs.d[reg], regs.d[dm] & 63));
}
void op_rol_w_reg(int dm, int reg) {
    WRITE_D16(reg, DO_ROL_W(regs.d[reg], regs.d[dm] & 63));
}

OP(shl_w) {
    switch(type) {
    case 0:
        op_asl_w_imm(dm, reg);
        break;
    case 1:
        op_lsl_w_imm(dm, reg);
        break;
    case 2:
        op_roxl_w_imm(dm, reg);
        break;
    case 3:
        op_rol_w_imm(dm, reg);
        break;
    case 4:
        op_asl_w_reg(dm, reg);
        break;
    case 5:
        op_lsl_w_reg(dm, reg);
        break;
    case 6:
        op_roxl_w_reg(dm, reg);
        break;
    case 7:
        op_rol_w_reg(dm, reg);
        break;
    }
}
void op_asl_l_imm(int dm, int reg) {
    regs.d[reg] = DO_ASL_L(regs.d[reg], dm ? dm : 8);
}
void op_lsl_l_imm(int dm, int reg) {
    regs.d[reg] = DO_LSL_L(regs.d[reg], dm ? dm : 8);
}
void op_roxl_l_imm(int dm, int reg) {
    regs.d[reg] = DO_ROXL_L(regs.d[reg], dm ? dm : 8);
}
void op_rol_l_imm(int dm, int reg) {
    regs.d[reg] = DO_ROL_L(regs.d[reg], dm ? dm : 8);
}
void op_asl_l_reg(int dm, int reg) {
    regs.d[reg] = DO_ASL_L(regs.d[reg], regs.d[dm] & 63);
}
void op_lsl_l_reg(int dm, int reg) {
    regs.d[reg] = DO_LSL_L(regs.d[reg], regs.d[dm] & 63);
}
void op_roxl_l_reg(int dm, int reg) {
    regs.d[reg] = DO_ROXL_L(regs.d[reg], regs.d[dm] & 63);
}
void op_rol_l_reg(int dm, int reg) {
    regs.d[reg] = DO_ROL_L(regs.d[reg], regs.d[dm] & 63);
}

OP(shl_l) {
    switch(type) {
    case 0:
        op_asl_l_imm(dm, reg);
        break;
    case 1:
        op_lsl_l_imm(dm, reg);
        break;
    case 2:
        op_roxl_l_imm(dm, reg);
        break;
    case 3:
        op_rol_l_imm(dm, reg);
        break;
    case 4:
        op_asl_l_reg(dm, reg);
        break;
    case 5:
        op_lsl_l_reg(dm, reg);
        break;
    case 6:
        op_roxl_l_reg(dm, reg);
        break;
    case 7:
        op_rol_l_reg(dm, reg);
        break;
    }
}
OP(asl_ea) {
    EA_Update16(type, reg, [](auto v) { return DO_ASL_W(v, 1); });
}
OP(lsl_ea) {
    EA_Update16(type, reg, [](auto v) { return DO_LSL_W(v, 1); });
}
OP(roxl_ea) {
    EA_Update16(type, reg, [](auto v) { return DO_ROXL_W(v, 1); });
}
OP(rol_ea) {
    EA_Update16(type, reg, [](auto v) { return DO_ROL_W(v, 1); });
}
