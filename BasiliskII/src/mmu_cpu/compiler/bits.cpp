#include "compiler/compiler.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "ex_stack.h"
#include "exception.h"
#include "intop.h"
#include "newcpu.h"
std::function<void()> compile_ori_b(uint16_t op, int type, int reg, int dm) {
    uint8_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        return [v2]() { SET_CCR(GET_CCR() | v2); };
    } else {
        return EA_Update8(type, reg,
                          [v2](uint8_t v1) { return DO_OR_B(v1, v2); });
    }
}
void PRIV_ERROR();

std::function<void()> compile_ori_w(uint16_t op, int type, int reg, int dm) {
    uint16_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        return [v2]() {
            if(!regs.S) {
                PRIV_ERROR();
                return;
            }
            regs.traced = true;
            SET_SR(GET_SR() | v2);
        };
    } else {
        return EA_Update16(type, reg,
                           [v2](uint16_t v1) { return DO_OR_W(v1, v2); });
    }
}

std::function<void()> compile_ori_l(uint16_t op, int type, int reg, int dm) {
    uint32_t v2 = FETCH32();
    return EA_Update32(type, reg,
                       [v2](uint32_t v1) { return DO_OR_L(v1, v2); });
}

std::function<void()> compile_andi_b(uint16_t op, int type, int reg, int dm) {
    uint8_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        return [v2]() { SET_CCR(GET_CCR() & v2); };
    } else {
        return EA_Update8(type, reg,
                          [v2](uint8_t v1) { return DO_AND_B(v1, v2); });
    }
}

std::function<void()> compile_andi_w(uint16_t op, int type, int reg, int dm) {
    uint16_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        return [v2]() {
            if(!regs.S) {
                PRIV_ERROR();
                return;
            }
            regs.traced = true;
            SET_SR(GET_SR() & v2);
        };
    } else {
        return EA_Update16(type, reg,
                           [v2](uint16_t v1) { return DO_AND_W(v1, v2); });
    }
}

std::function<void()> compile_andi_l(uint16_t op, int type, int reg, int dm) {
    uint32_t v2 = FETCH32();
    return EA_Update32(type, reg,
                       [v2](uint32_t v1) { return DO_AND_L(v1, v2); });
}

std::function<void()> compile_btst_imm(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        int bn = FETCH() & 31;
        return [reg, bn]() {
            uint32_t v1 = regs.d[reg];
            regs.z = DO_BTST(v1, bn);
        };
    } else {
        int bn = FETCH() & 7;
        return EA_Read8(type, reg,
                        [bn](uint8_t v1) { regs.z = DO_BTST(v1, bn); });
    }
}
std::function<void()> compile_bchg_imm(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        int bn = FETCH() & 31;
        return [reg, bn]() {
            uint32_t v1 = regs.d[reg];
            regs.d[reg] = DO_BCHG_L(v1, bn);
        };
    } else {
        int bn = FETCH() & 7;
        return EA_Update8(type, reg,
                          [bn](uint8_t v1) { return DO_BCHG_B(v1, bn); });
    }
}
std::function<void()> compile_bclr_imm(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        int bn = FETCH() & 31;
        return [reg, bn]() {
            uint32_t v1 = regs.d[reg];
            regs.d[reg] = DO_BCLR_L(v1, bn);
        };
    } else {
        int bn = FETCH() & 7;
        return EA_Update8(type, reg,
                          [bn](uint8_t v1) { return DO_BCLR_B(v1, bn); });
    }
}
std::function<void()> compile_bset_imm(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        int bn = FETCH() & 31;
        return [reg, bn]() {
            uint32_t v1 = regs.d[reg];
            regs.d[reg] = DO_BSET_L(v1, bn);
        };
    } else {
        int bn = FETCH() & 7;
        return EA_Update8(type, reg,
                          [bn](uint8_t v1) { return DO_BSET_B(v1, bn); });
    }
}

std::function<void()> compile_eori_b(uint16_t op, int type, int reg, int dm) {
    uint8_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        return [v2]() { SET_CCR(GET_CCR() ^ v2); };
    } else {
        return EA_Update8(type, reg,
                          [v2](uint8_t v1) { return DO_EOR_B(v1, v2); });
    }
}

std::function<void()> compile_eori_w(uint16_t op, int type, int reg, int dm) {
    uint16_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        return [v2]() {
            if(!regs.S) {
                PRIV_ERROR();
                return;
            }
            regs.traced = true;
            SET_SR(GET_SR() ^ v2);
        };
    } else {
        return EA_Update16(type, reg,
                           [v2](uint16_t v1) { return DO_EOR_W(v1, v2); });
    }
}

std::function<void()> compile_eori_l(uint16_t op, int type, int reg, int dm) {
    uint32_t v2 = FETCH32();
    return EA_Update32(type, reg,
                       [v2](uint32_t v1) { return DO_EOR_L(v1, v2); });
}

std::function<void()> compile_clr_b(uint16_t op, int type, int reg, int dm) {

    return EA_Write8(type, reg, []() {
        regs.n = regs.v = regs.c = false;
        regs.z = true;
        return 0;
    });
}

std::function<void()> compile_clr_w(uint16_t op, int type, int reg, int dm) {
    return EA_Write16(type, reg, []() {
        regs.n = regs.v = regs.c = false;
        regs.z = true;
        return 0;
    });
}

std::function<void()> compile_clr_l(uint16_t op, int type, int reg, int dm) {
    return EA_Write32(type, reg, []() {
        regs.n = regs.v = regs.c = false;
        regs.z = true;
        return 0;
    });
}

std::function<void()> compile_not_b(uint16_t op, int type, int reg, int dm) {
    return EA_Update8(type, reg, [](uint8_t v1) { return DO_NOT_B(v1); });
}

std::function<void()> compile_not_w(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](uint16_t v1) { return DO_NOT_W(v1); });
}

std::function<void()> compile_not_l(uint16_t op, int type, int reg, int dm) {
    return EA_Update32(type, reg, [](uint32_t v1) { return DO_NOT_L(v1); });
}

std::function<void()> compile_tst_b(uint16_t op, int type, int reg, int dm) {
    return EA_Read8(type, reg, [](uint8_t v) {
        TEST_ALL8(v);
    });
}
std::function<void()> compile_tst_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [](uint16_t v) {
        TEST_ALL16(v);
    });
}
std::function<void()> compile_tst_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [](uint32_t v) {
        TEST_ALL32(v);
    });
}

std::function<void()> compile_tas(uint16_t op, int type, int reg, int dm) {
    if(type == 7 && reg == 4) {
        return ILLEGAL_INST;
    } else {
        return EA_Update8(type, reg, [](uint8_t v) {
            TEST_ALL8(v);
            return v | 0x80;
        });
    }
}

std::function<void()> compile_asr_ea(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](auto v) { return DO_ASR_W(v, 1); });
}
std::function<void()> compile_lsr_ea(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](auto v) { return DO_LSR_W(v, 1); });
}
std::function<void()> compile_roxr_ea(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](auto v) { return DO_ROXR_W(v, 1); });
}
std::function<void()> compile_ror_ea(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](auto v) { return DO_ROR_W(v, 1); });
}

std::function<void()> compile_asl_ea(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](auto v) { return DO_ASL_W(v, 1); });
}
std::function<void()> compile_lsl_ea(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](auto v) { return DO_LSL_W(v, 1); });
}
std::function<void()> compile_roxl_ea(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](auto v) { return DO_ROXL_W(v, 1); });
}
std::function<void()> compile_rol_ea(uint16_t op, int type, int reg, int dm) {
    return EA_Update16(type, reg, [](auto v) { return DO_ROL_W(v, 1); });
}

std::function<void()> compile_btst_dm(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        return [reg, dm]() {
            uint32_t v1 = regs.d[reg];
            int bn = regs.d[dm] & 31;
            regs.z = DO_BTST(v1, bn);
        };
    } else if(type == 1) {
        // MOVEP.W From
        int16_t disp = FETCH();
        return [reg, dm, disp]() {
            uint32_t addr = regs.a[reg] + disp;
            WRITE_D16(dm, read8(addr) << 8 | read8(addr + 2));
        };
    } else {
        return EA_Read8(type, reg, [dm](uint8_t v1) {
            int bn = regs.d[dm] & 7;
            regs.z = DO_BTST(v1, bn);
        });
    }
}
std::function<void()> compile_bchg_dm(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        return [reg, dm]() {
            uint32_t v1 = regs.d[reg];
            int bn = regs.d[dm] & 31;
            regs.d[reg] = DO_BCHG_L(v1, bn);
        };
    } else if(type == 1) {
        // MOVEP.L From
        int16_t disp = FETCH();
        return [reg, dm, disp]() {
            uint32_t addr = regs.a[reg] + disp;
            regs.d[dm] = read8(addr) << 24 | read8(addr + 2) << 16 |
                         read8(addr + 4) << 8 | read8(addr + 6);
        };
    } else {
        return EA_Update8(type, reg, [dm](uint8_t v1) {
            int bn = regs.d[dm] & 7;
            return DO_BCHG_B(v1, bn);
        });
    }
}

std::function<void()> compile_bclr_dm(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        return [reg, dm]() {
            uint32_t v1 = regs.d[reg];
            int bn = regs.d[dm] & 31;
            regs.d[reg] = DO_BCLR_L(v1, bn);
        };
    } else if(type == 1) {
        // MOVEP.W To
        int16_t disp = FETCH();
        return [reg, dm, disp]() {
            uint32_t addr = regs.a[reg] + disp;
            uint16_t v = regs.d[dm];
            write8(addr, v >> 8);
            write8(addr + 2, v);
        };
    } else {
        return EA_Update8(type, reg, [dm](uint8_t v1) {
            int bn = regs.d[dm] & 7;
            return DO_BCLR_B(v1, bn);
        });
    }
}
std::function<void()> compile_bset_dm(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        return [reg, dm]() {
            uint32_t v1 = regs.d[reg];
            int bn = regs.d[dm] & 31;
            regs.d[reg] = DO_BSET_L(v1, bn);
        };
    } else if(type == 1) {
        // MOVEP.L To
        int16_t disp = FETCH();
        return [reg, dm, disp]() {
            uint32_t addr = regs.a[reg] + disp;
            uint32_t v = regs.d[dm];
            write8(addr, v >> 24);
            write8(addr + 2, v >> 16);
            write8(addr + 4, v >> 8);
            write8(addr + 6, v);
        };
    } else {
        return EA_Update8(type, reg, [dm](uint8_t v1) {
            int bn = regs.d[dm] & 7;
            return DO_BSET_B(v1, bn);
        });
    }
}

std::function<void()> compile_or_b(uint16_t op, int type, int reg, int dm) {
    return EA_Read8(
        type, reg, [dm](uint8_t v2) { WRITE_D8(dm, DO_OR_B(regs.d[dm], v2)); });
}
std::function<void()> compile_or_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t v2) {
        WRITE_D16(dm, DO_OR_W(regs.d[dm], v2));
    });
}
std::function<void()> compile_or_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(
        type, reg, [dm](uint32_t v2) { regs.d[dm] = DO_OR_L(regs.d[dm], v2); });
}
uint8_t do_sbcd(uint8_t a, uint8_t b);
std::function<void()> compile_or_to_ea_b(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        return [dm, reg]() {
            uint8_t src = regs.d[reg];
            uint8_t dst = regs.d[dm];
            WRITE_D8(dm, do_sbcd(dst, src));
        };
    } else if(type == 1) {
        return [dm, reg]() {
            uint8_t src = read8(--regs.a[reg]);
            uint8_t dst = read8(--regs.a[dm]);
            write8(regs.a[dm], do_sbcd(dst, src));
        };
    } else {
        return EA_Update8(type, reg, [dm](uint8_t v1) {
            uint8_t v2 = regs.d[dm];
            return DO_OR_B(v1, v2);
        });
    }
}
uint8_t do_pack(uint16_t v);
uint16_t do_unpk(uint8_t v);
std::function<void()> compile_or_to_ea_w(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        uint16_t adjust = FETCH();
        return [dm, adjust, reg]() {
            uint16_t src = regs.d[reg];
            WRITE_D8(dm, do_pack(src + adjust));
        };
    } else if(type == 1) {
        uint16_t adjust = FETCH();
        return [dm, adjust, reg]() {
            uint8_t src1 = read8(--regs.a[reg]);
            uint8_t src2 = read8(--regs.a[reg]);
            uint16_t dst = (src1 | src2 << 8) + adjust;
            write8(--regs.a[dm], do_pack(dst));
        };
    } else {
        return EA_Update16(type, reg, [dm](uint16_t v1) {
            uint16_t v2 = regs.d[dm];
            return DO_OR_W(v1, v2);
        });
    }
}
std::function<void()> compile_or_to_ea_l(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        uint16_t adjust = FETCH();
        return [dm, adjust, reg]() {
            WRITE_D16(dm, do_unpk(regs.d[reg]) + adjust);
        };
    } else if(type == 1) {
        uint16_t adjust = FETCH();
        return [dm, adjust, reg]() {
            uint8_t src = read8(--regs.a[reg]);
            uint16_t dst = do_unpk(src) + adjust;
            write16(regs.a[dm] -= 2, dst);
        };
    } else {
        return EA_Update32(type, reg, [dm](uint32_t v1) {
            uint32_t v2 = regs.d[dm];
            return DO_OR_L(v1, v2);
        });
    }
}

std::function<void()> compile_eor_b(uint16_t op, int type, int reg, int dm) {
    if(type == 1) {
        return [reg, dm]() {
            uint8_t v_y = read8(regs.a[reg]);
            uint8_t v_x = read8(regs.a[dm]);
            DO_CMP_B(v_x, v_y);
            ++regs.a[reg];
            ++regs.a[dm];
        };
    } else {
        return EA_Update8(
            type, reg, [dm](uint8_t v1) { return DO_EOR_B(v1, regs.d[dm]); });
    }
}
std::function<void()> compile_eor_w(uint16_t op, int type, int reg, int dm) {
    if(type == 1) {
        return [reg, dm]() {
            uint16_t v_y = read16(regs.a[reg]);
            uint16_t v_x = read16(regs.a[dm]);
            DO_CMP_W(v_x, v_y);
            regs.a[reg] += 2;
            regs.a[dm] += 2;
        };
    } else {
        return EA_Update16(
            type, reg, [dm](uint16_t v1) { return DO_EOR_W(v1, regs.d[dm]); });
    }
}
std::function<void()> compile_eor_l(uint16_t op, int type, int reg, int dm) {
    if(type == 1) {
        return [reg, dm]() {
            uint32_t v_y = read32(regs.a[reg]);
            uint32_t v_x = read32(regs.a[dm]);
            DO_CMP_L(v_x, v_y);
            regs.a[reg] += 4;
            regs.a[dm] += 4;
        };
    } else {
        return EA_Update32(
            type, reg, [dm](uint32_t v1) { return DO_EOR_L(v1, regs.d[dm]); });
    }
}

std::function<void()> compile_and_b(uint16_t op, int type, int reg, int dm) {
    return EA_Read8(type, reg, [dm](uint8_t v2) {
        WRITE_D8(dm, DO_AND_B(regs.d[dm], v2));
    });
}
std::function<void()> compile_and_w(uint16_t op, int type, int reg, int dm) {
    return EA_Read16(type, reg, [dm](uint16_t v2) {
        WRITE_D16(dm, DO_AND_W(regs.d[dm], v2));
    });
}
std::function<void()> compile_and_l(uint16_t op, int type, int reg, int dm) {
    return EA_Read32(type, reg, [dm](uint32_t v2) {
        regs.d[dm] = DO_AND_L(regs.d[dm], v2);
    });
}

uint8_t do_abcd(uint8_t a, uint8_t b);

std::function<void()> compile_and_to_ea_b(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        return [dm, reg]() {
            uint8_t src = regs.d[reg];
            uint8_t dst = regs.d[dm];
            WRITE_D8(dm, do_abcd(dst, src));
        };
    } else if(type == 1) {
        return [dm, reg]() {
            uint8_t src = read8(--regs.a[reg]);
            uint8_t dst = read8(--regs.a[dm]);
            write8(regs.a[dm], do_abcd(dst, src));
        };
    } else {
        return EA_Update8(
            type, reg, [dm](uint8_t v1) { return DO_AND_B(v1, regs.d[dm]); });
    }
}
std::function<void()> compile_and_to_ea_w(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        return [dm, reg]() { std::swap(regs.d[reg], regs.d[dm]); };
    } else if(type == 1) {
        return [dm, reg]() { std::swap(regs.a[reg], regs.a[dm]); };
    } else {
        return EA_Update16(
            type, reg, [dm](uint16_t v1) { return DO_AND_W(v1, regs.d[dm]); });
    }
}
std::function<void()> compile_and_to_ea_l(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        ILLEGAL_INST();
    } else if(type == 1) {
        return [dm, reg]() { std::swap(regs.a[reg], regs.d[dm]); };
    } else {
        return EA_Update32(
            type, reg, [dm](uint32_t v1) { return DO_AND_L(v1, regs.d[dm]); });
    }
}

std::function<void()> compile_shr_b(uint16_t op, int type, int reg, int dm) {
    switch(type) {
    case 0:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D8(reg, DO_ASR_B(regs.d[reg], dm));
        };
    case 1:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D8(reg, DO_LSR_B(regs.d[reg], dm));
        };
    case 2:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D8(reg, DO_ROXR_B(regs.d[reg], dm));
        };
    case 3:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D8(reg, DO_ROR_B(regs.d[reg], dm));
        };
    case 4:
        return [reg, dm]() {
            WRITE_D8(reg, DO_ASR_B(regs.d[reg], regs.d[dm] & 63));
        };
    case 5:
        return [reg, dm]() {
            WRITE_D8(reg, DO_LSR_B(regs.d[reg], regs.d[dm] & 63));
        };
    case 6:
        return [reg, dm]() {
            WRITE_D8(reg, DO_ROXR_B(regs.d[reg], regs.d[dm] & 63));
        };
    case 7:
        return [reg, dm]() {
            WRITE_D8(reg, DO_ROR_B(regs.d[reg], regs.d[dm] & 63));
        };
    }
    ILLEGAL_INST();
}
std::function<void()> compile_shr_w(uint16_t op, int type, int reg, int dm) {
  switch(type) {
    case 0:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D16(reg, DO_ASR_W(regs.d[reg], dm));
        };
    case 1:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D16(reg, DO_LSR_W(regs.d[reg], dm));
        };
    case 2:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D16(reg, DO_ROXR_W(regs.d[reg], dm));
        };
    case 3:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D16(reg, DO_ROR_W(regs.d[reg], dm));
        };
    case 4:
        return [reg, dm]() {
            WRITE_D16(reg, DO_ASR_W(regs.d[reg], regs.d[dm] & 63));
        };
    case 5:
        return [reg, dm]() {
            WRITE_D16(reg, DO_LSR_W(regs.d[reg], regs.d[dm] & 63));
        };
    case 6:
        return [reg, dm]() {
            WRITE_D16(reg, DO_ROXR_W(regs.d[reg], regs.d[dm] & 63));
        };
    case 7:
        return [reg, dm]() {
            WRITE_D16(reg, DO_ROR_W(regs.d[reg], regs.d[dm] & 63));
        };
    }
    ILLEGAL_INST();
}
std::function<void()> compile_shr_l(uint16_t op, int type, int reg, int dm) {
     switch(type) {
    case 0:
        return [reg, dm = dm ? dm : 8]() {
            regs.d[reg] = DO_ASR_L(regs.d[reg], dm);
        };
    case 1:
        return [reg, dm = dm ? dm : 8]() {
            regs.d[reg] = DO_LSR_L(regs.d[reg], dm);
        };
    case 2:
        return [reg, dm = dm ? dm : 8]() {
            regs.d[reg] = DO_ROXR_L(regs.d[reg], dm);
        };
    case 3:
        return [reg, dm = dm ? dm : 8]() {
            regs.d[reg] = DO_ROR_L(regs.d[reg], dm);
        };
    case 4:
        return [reg, dm]() {
            regs.d[reg] = DO_ASR_L(regs.d[reg], regs.d[dm] & 63);
        };
    case 5:
        return [reg, dm]() {
            regs.d[reg] = DO_LSR_L(regs.d[reg], regs.d[dm] & 63);
        };
    case 6:
        return [reg, dm]() {
            regs.d[reg] = DO_ROXR_L(regs.d[reg], regs.d[dm] & 63);
        };
    case 7:
        return [reg, dm]() {
            regs.d[reg] = DO_ROR_L(regs.d[reg], regs.d[dm] & 63);
        };
    }
    ILLEGAL_INST();
}


std::function<void()> compile_shl_b(uint16_t op, int type, int reg, int dm) {
    switch(type) {
    case 0:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D8(reg, DO_ASL_B(regs.d[reg], dm));
        };
    case 1:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D8(reg, DO_LSL_B(regs.d[reg], dm));
        };
    case 2:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D8(reg, DO_ROXL_B(regs.d[reg], dm));
        };
    case 3:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D8(reg, DO_ROL_B(regs.d[reg], dm));
        };
    case 4:
        return [reg, dm]() {
            WRITE_D8(reg, DO_ASL_B(regs.d[reg], regs.d[dm] & 63));
        };
    case 5:
        return [reg, dm]() {
            WRITE_D8(reg, DO_LSL_B(regs.d[reg], regs.d[dm] & 63));
        };
    case 6:
        return [reg, dm]() {
            WRITE_D8(reg, DO_ROXL_B(regs.d[reg], regs.d[dm] & 63));
        };
    case 7:
        return [reg, dm]() {
            WRITE_D8(reg, DO_ROL_B(regs.d[reg], regs.d[dm] & 63));
        };
    }
    ILLEGAL_INST();
}
std::function<void()> compile_shl_w(uint16_t op, int type, int reg, int dm) {
  switch(type) {
    case 0:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D16(reg, DO_ASL_W(regs.d[reg], dm));
        };
    case 1:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D16(reg, DO_LSL_W(regs.d[reg], dm));
        };
    case 2:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D16(reg, DO_ROXL_W(regs.d[reg], dm));
        };
    case 3:
        return [reg, dm = dm ? dm : 8]() {
            WRITE_D16(reg, DO_ROL_W(regs.d[reg], dm));
        };
    case 4:
        return [reg, dm]() {
            WRITE_D16(reg, DO_ASL_W(regs.d[reg], regs.d[dm] & 63));
        };
    case 5:
        return [reg, dm]() {
            WRITE_D16(reg, DO_LSL_W(regs.d[reg], regs.d[dm] & 63));
        };
    case 6:
        return [reg, dm]() {
            WRITE_D16(reg, DO_ROXL_W(regs.d[reg], regs.d[dm] & 63));
        };
    case 7:
        return [reg, dm]() {
            WRITE_D16(reg, DO_ROL_W(regs.d[reg], regs.d[dm] & 63));
        };
    }
    ILLEGAL_INST();
}
std::function<void()> compile_shl_l(uint16_t op, int type, int reg, int dm) {
     switch(type) {
    case 0:
        return [reg, dm = dm ? dm : 8]() {
            regs.d[reg] = DO_ASL_L(regs.d[reg], dm);
        };
    case 1:
        return [reg, dm = dm ? dm : 8]() {
            regs.d[reg] = DO_LSL_L(regs.d[reg], dm);
        };
    case 2:
        return [reg, dm = dm ? dm : 8]() {
            regs.d[reg] = DO_ROXL_L(regs.d[reg], dm);
        };
    case 3:
        return [reg, dm = dm ? dm : 8]() {
            regs.d[reg] = DO_ROL_L(regs.d[reg], dm);
        };
    case 4:
        return [reg, dm]() {
            regs.d[reg] = DO_ASL_L(regs.d[reg], regs.d[dm] & 63);
        };
    case 5:
        return [reg, dm]() {
            regs.d[reg] = DO_LSL_L(regs.d[reg], regs.d[dm] & 63);
        };
    case 6:
        return [reg, dm]() {
            regs.d[reg] = DO_ROXL_L(regs.d[reg], regs.d[dm] & 63);
        };
    case 7:
        return [reg, dm]() {
            regs.d[reg] = DO_ROL_L(regs.d[reg], regs.d[dm] & 63);
        };
    }
    ILLEGAL_INST();
}

