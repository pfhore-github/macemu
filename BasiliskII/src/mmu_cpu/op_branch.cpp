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
void reset_all();
void op_link_l(int reg) {
    int32_t disp = FETCH32();
    PUSH32(regs.a[reg]);
    regs.a[reg] = regs.a[7];
    regs.a[7] += disp;
}

void op_bkpt() { ILLEGAL_INST(); }
void op_trap(int v) { TRAP(v); }

void op_link_w(int reg) {
    int16_t disp = FETCH();
    PUSH32(regs.a[reg]);
    regs.a[reg] = regs.a[7];
    regs.a[7] += disp;
}

void op_unlk(int reg) {
    regs.a[7] = regs.a[reg];
    regs.a[reg] = POP32();
}

void op_reset() {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    // TODO
    reset_all();
}

void op_stop() {
    uint16_t next = FETCH();
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    SET_SR(next);
    regs.sleep = std::make_unique<std::promise<void>>();
    std::future<void> f = regs.sleep->get_future();
    f.wait();
}

void op_rte() {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
BEGIN:
    uint16_t sr = POP16();
    uint32_t pc = POP32();
    uint16_t sx = POP16();
    switch(sx >> 12) {
    case 0:
        break;
    case 1:
        SET_SR(sr);
        goto BEGIN;
    case 2:
        POP32();
        break;
    case 3:
        POP32();
        break;
    case 7: {
        uint32_t ea = POP32();
        uint16_t ssw = POP16();
        regs.a[7] += 2 * 23;
        if(ssw & 1 << 13) {
            // TRACE
            SET_SR(sr);
            TRACE();
        }
        if(ssw & 1 << 12) {
            // MOVEM continution
            regs.i_ea = ea;
        }
        break;
    }
    default:
        FORMAT_ERROR();
        return;
    }
    SET_SR(sr);
    JUMP(pc);
    return;
}

void op_rtd() {
    int16_t disp = FETCH();
    uint32_t pc = POP32();
    regs.a[7] += disp;
    JUMP(pc);
}

void op_rts() {
    uint32_t pc = POP32();
    JUMP(pc);
}

void op_trapv() {
    if(regs.v) {
        TRPPcc();
    }
}

void op_rtr() {
    SET_CCR(POP16());
    uint32_t pc = POP32();
    JUMP(pc);
}

OP(jsr) {
    uint32_t dst = EA_Addr(type, reg, 0, false);
    PUSH32(regs.pc);
    JUMP(dst);
}

OP(jmp) {
    uint32_t dst = EA_Addr(type, reg, 0, false);
    JUMP(dst);
}

bool cc_test(int cc) {
    switch(cc) {
    case 0:
        return true; // T
    case 1:
        return false; // F
    case 2:
        return !regs.c & !regs.z; // HI
    case 3:
        return regs.c | regs.z; // LS
    case 4:
        return !regs.c; // CC
    case 5:
        return regs.c; // CS
    case 6:
        return !regs.z; // NE
    case 7:
        return regs.z; // EQ
    case 8:
        return !regs.v; // VC
    case 9:
        return regs.v; // VS
    case 10:
        return !regs.n; //  PL
    case 11:
        return regs.n; // MI
    case 12:
        return regs.n == regs.v; // GE
    case 13:
        return regs.n != regs.v; // LT
    case 14:
        return (regs.n == regs.v) & !regs.z; // GT
    case 15:
        return regs.z | (regs.n != regs.v); // LE
    }
    return false;
}
OP(scc) {
    int cond = xop >> 8 & 15;
    bool ret = cc_test(cond);
    if(type == 1) {
        // DBcc
        uint32_t pc = regs.pc;
        int16_t disp = FETCH();
        if(!ret) {
            int16_t dx = regs.d[reg] - 1;
            WRITE_D16(reg, dx);
            if(dx != -1) {
                JUMP(pc + disp);
            }
        }
    } else if(type == 7) {
        // TRAPcc
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
            ILLEGAL_INST();
        }
        if(ret) {
            TRPPcc();
        }
    } else {
        EA_WRITE8(type, reg, ret ? 0xff : 0);
    }
}

OP(bra) {
    uint32_t pc = regs.pc;
    int32_t disp = static_cast<int8_t>(xop & 0xff);
    if(disp == 0) {
        disp = static_cast<int16_t>(FETCH());
    } else if(disp == -1) {
        disp = FETCH32();
    }
    JUMP(pc + disp);
}

OP(bsr) {
    uint32_t pc = regs.pc;
    int32_t disp = static_cast<int8_t>(xop & 0xff);
    if(disp == 0) {
        disp = static_cast<int16_t>(FETCH());
    } else if(disp == -1) {
        disp = FETCH32();
    }
    PUSH32(pc);
    JUMP(pc + disp);
}

OP(bcc) {
    uint32_t pc = regs.pc;
    int cond = xop >> 8 & 15;
    int32_t disp = static_cast<int8_t>(xop & 0xff);
    if(disp == 0) {
        disp = static_cast<int16_t>(FETCH());
    } else if(disp == -1) {
        disp = FETCH32();
    }
    if(cc_test(cond)) {
        JUMP(pc + disp);
    }
}