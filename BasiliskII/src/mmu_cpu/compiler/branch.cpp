#include "compiler/compiler.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "exception.h"
#include "intop.h"
#include "mbus.h"
#include "newcpu.h"
#include <unordered_map>
void reset_all();
uint32_t do_op_movec_from(int o);
void do_op_movec_to(int op, uint32_t v);
void op_rte();
std::function<void()> compile_0471(uint16_t op, int type, int reg, int dm) {
    switch(type) {
    case 0:
    case 1:
        // TRAP
        return [v = type << 3 | reg]() { TRAP(v); };
    case 2: { // LINK.W
        int16_t disp = FETCH();
        return [reg, disp]() {
            PUSH32(regs.a[reg]);
            regs.a[reg] = regs.a[7];
            regs.a[7] += disp;
        };
    }
    case 3:
        // UNLK
        return [reg]() {
            regs.a[7] = regs.a[reg];
            regs.a[reg] = POP32();
        };
    case 4:
        // MOVE to USP
        return [reg]() {
            if(!regs.S) {
                PRIV_ERROR();
            }
            regs.traced = true;
            regs.usp = regs.a[reg];
        };
    case 5:
        // MOVE from USP
        return [reg]() {
            if(!regs.S) {
                PRIV_ERROR();
            }
            regs.traced = true;
            regs.a[reg] = regs.usp;
        };
    case 6:
        switch(reg) {
        case 0:
            // RESET
            return []() {
                if(!regs.S) {
                    PRIV_ERROR();
                }
                // TODO
                reset_all();
            };
        case 1:
            // NOP
            return []() { regs.traced = true; };
        case 2:
            // STOP
            {
                uint16_t next = FETCH();
                return [next]() {
                    if(!regs.S) {
                        PRIV_ERROR();
                    }
                    regs.traced = true;
                    SET_SR(next);
                    cpu.sleep = std::make_unique<std::promise<void>>();
                    std::future<void> f = cpu.sleep->get_future();
                    f.wait();
                };
            }
        case 3:
            // RTE
            return op_rte;
        case 4:
            // RTD
            {
                int16_t disp = FETCH();
                return [disp]() {
                    regs.traced = true;
                    uint32_t pc = POP32();
                    regs.a[7] += disp;
                    JUMP(pc);
                };
            }
        case 5:
            // RTS
            return []() {
                regs.traced = true;
                uint32_t pc = POP32();
                JUMP(pc);
            };
        case 6:
            // TRAPV
            return []() {
                if(regs.v) {
                    TRPPcc();
                }
            };
        case 7:
            // RTR
            return []() {
                regs.traced = true;
                SET_CCR(POP16());
                uint32_t pc = POP32();
                JUMP(pc);
            };
        }
        break;
    case 7:
        if(reg == 2) {
            uint16_t next = FETCH();
            int dst = next >> 12 & 15;
            return [dst, p = next & 0xfff]() {
                if(!regs.S) {
                    PRIV_ERROR();
                }
                regs.traced = true;
                regs.r[dst] = do_op_movec_from(p);
            };
        } else if(reg == 3) {
            uint16_t next = FETCH();
            int src = next >> 12 & 15;
            return [src, p = next & 0xfff]() {
                if(!regs.S) {
                    PRIV_ERROR();
                    return;
                }
                regs.traced = true;
                do_op_movec_to(p, regs.r[src]);
            };
        }
    }
    ILLEGAL_INST();
}

std::function<void()> compile_jsr(uint16_t op, int type, int reg, int dm) {
    return [f = ea_addr(type, reg, 0, false)]() {
        regs.traced = true;
        uint32_t dst = f();
        PUSH32(regs.pc);
        JUMP(dst);
    };
}
std::function<void()> compile_jmp(uint16_t op, int type, int reg, int dm) {
    return [f = ea_addr(type, reg, 0, false)]() {
        regs.traced = true;
        uint32_t dst = f();
        JUMP(dst);
    };
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
std::function<void()> compile_scc(uint16_t op, int type, int reg, int dm) {
    int cond = op >> 8 & 15;
    if(type == 1) {
        // DBcc
        uint32_t pc = regs.pc;
        int16_t disp = FETCH();
        return [reg, cond, next = pc + disp]() {
            if(!cc_test(cond)) {
                int16_t dx = regs.d[reg] - 1;
                WRITE_D16(reg, dx);
                if(dx != -1) {
                    regs.traced = true;
                    JUMP(next);
                }
            }
        };
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
        return [cond]() {
            if(cc_test(cond)) {
                TRPPcc();
            }
        };
    } else {
        return EA_Write8(type, reg,
                         [cond]() { return cc_test(cond) ? 0xff : 0; });
    }
}

std::function<void()> compile_bra(uint16_t op, int type, int reg, int dm) {
    uint32_t pc = regs.pc;
    int32_t disp = DO_EXTB_L(op & 0xff);
    if(disp == 0) {
        disp = DO_EXT_L(FETCH());
    } else if(disp == -1) {
        disp = FETCH32();
    }
    return [target = pc + disp]() {
        regs.traced = true;
        JUMP(target);
    };
}

std::function<void()> compile_bsr(uint16_t op, int type, int reg, int dm) {
    uint32_t pc = regs.pc;
    int32_t disp = DO_EXTB_L(op & 0xff);
    if(disp == 0) {
        disp = DO_EXT_L(FETCH());
    } else if(disp == -1) {
        disp = FETCH32();
    }
    return [target = pc + disp]() {
        regs.traced = true;
        PUSH32(regs.pc);
        JUMP(target);
    };
}

std::function<void()> compile_bcc(uint16_t op, int type, int reg, int dm) {
    uint32_t pc = regs.pc;
    int32_t disp = DO_EXTB_L(op & 0xff);
    if(disp == 0) {
        disp = DO_EXT_L(FETCH());
    } else if(disp == -1) {
        disp = FETCH32();
    }
    int cond = op >> 8 & 15;

    return [cond, target = pc + disp]() {
        if(cc_test(cond)) {
            regs.traced = true;
            JUMP(target);
        }
    };
}