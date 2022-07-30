#include "cpu_emulation.h"
#include "emul_op.h"
#include "intop.h"
#include "main.h"
#include "sysdeps.h"
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <unordered_map>
#include <vector>

#include "compiler/jit.h"
#include "ea.h"
#include "ex_stack.h"
#include "exception.h"
#include "fpu/fpu_mpfr.h"
#include "newcpu.h"
#include "op.h"

#include "ea.h"
#include <asmjit/x86.h>
#include <setjmp.h>
using namespace asmjit;
void get_eaext(x86::Assembler &a) {
    uint16_t nextw = FETCH();

    bool full = (nextw >> 8 & 1);
    int reg = nextw >> 12 & 15;
    bool wl = nextw >> 11 & 1;
    int scale = nextw >> 9 & 3;
    if(!wl) {
        a.movsx(x86::r9d, JIT_REG_D_W(reg));
    } else {
        a.mov(x86::r9d, JIT_REG_D_L(reg));
    }
    if(!full) {
        int8_t disp = nextw & 0xff;
        a.lea(x86::rbx, x86::dword_ptr(x86::rbx, x86::r9, scale, disp));
    } else {
        if(nextw & 1 << 3) {
            ILLEGAL_INST();
        }
        bool bs = nextw >> 7 & 1;
        bool is = nextw >> 6 & 1;
        int bd_c = nextw >> 4 & 3;
        bool post = nextw >> 2 & 1;
        int od_c = nextw & 3;
        int32_t bd = 0;
        int32_t od = 0;
        switch(bd_c) {
        case 0:
            ILLEGAL_INST();
        case 1:
            break;
        case 2:
            bd = (int16_t)FETCH();
            break;
        case 3:
            bd = FETCH32();
            break;
        }
        if(bs) {
            a.mov(x86::rbx, 0);
        }
        
        switch(od_c) {
        case 0:
            if(post) {
                ILLEGAL_INST();
            }
            if(!is) {
                a.lea(x86::rbx, x86::dword_ptr(x86::rbx, x86::r9, scale, bd));
            } else {
                a.lea(x86::rbx, x86::dword_ptr(x86::rbx, bd));
            }
            return;
        case 1:
            break;
        case 2:
            od = (int16_t)FETCH();
            break;
        case 3:
            od = FETCH32();
            break;
        }
        if(!post) {
            if(!is) {
                a.lea(x86::rbx, x86::dword_ptr(x86::rbx, x86::r9, scale, bd));
            } else {
                a.lea(x86::rbx, x86::dword_ptr(x86::rbx, bd));
            }
            a.mov(ARG1, x86::rbx);
            a.call(jit_read32);
            a.lea(x86::rbx, x86::dword_ptr(x86::rax, od));
        } else {
            a.push(x86::r9);
            a.lea(ARG1, x86::dword_ptr(x86::rbx, bd));
            a.call(jit_read32);
            a.pop(x86::r9);
            if(!is) {
                a.lea(x86::rbx, x86::dword_ptr(x86::rax, x86::r9, scale, od));
            } else {
                a.lea(x86::rbx, x86::dword_ptr(x86::rax, od));
            }
        }
    }
}

void jit_ea_addr(int type, int reg, int sz, bool w, x86::Assembler &a) {
    a.mov(x86::ebx, JIT_REG_A_L(reg));
    switch(EA_OP{type}) {
    case EA_OP::DR:
    case EA_OP::AR:
        ILLEGAL_INST();
        break;
    case EA_OP::MEM:
        break;
    case EA_OP::INCR:
        if(sz) {
            if(reg == 7 && sz == 1) {
                sz = 2;
            }
            a.add(JIT_REG_A_L(reg), sz);
        } else {
            ILLEGAL_INST();
        }
        break;
    case EA_OP::DECR:
        if(sz) {
            if(reg == 7 && sz == 1) {
                sz = 2;
            }
            a.lea(x86::ebx, x86::dword_ptr(x86::ebx, -sz));
            a.mov(JIT_REG_A_L(reg), x86::ebx);
        } else {
            ILLEGAL_INST();
        }
        break;
    case EA_OP::OFFSET:
        a.lea(x86::ebx, x86::dword_ptr(x86::ebx, (int16_t)FETCH()));
        break;
    case EA_OP::EXT:
        get_eaext(a);
        break;
    case EA_OP::EXT2:
        switch(reg) {
        case 0:
            a.mov(x86::ebx, DO_EXT_L(FETCH()));
            break;
        case 1:
            a.mov(x86::ebx, FETCH32());
            break;
        case 2:
            if(!w) {
                a.mov(x86::ebx, regs.pc + (int16_t)FETCH());
            } else {
                ILLEGAL_INST();
            }
            break;
        case 3:
            if(!w) {
                a.mov(x86::ebx, regs.pc);
                get_eaext(a);
                break;
            }else {
                ILLEGAL_INST();
            }
            break;
        }
        break;
    default:
        ILLEGAL_INST();
    }
    a.mov(x86::rdi, &regs.i_ea);
    a.mov(x86::dword_ptr(x86::rdi), x86::rbx);
}
void jit_ea_read8(int type, int reg, bool override, x86::Assembler &a) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        a.mov(x86::al, JIT_REG_D_B(reg));
        return;
    case EA_OP::AR:
        throw ILLEGAL_INST_EX{};
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        if(reg == 4 && !override) {
            a.mov(x86::al, FETCH());
            return;
        } else {
            throw ILLEGAL_INST_EX{};
        }
    default:
        break;
    }
    a.mov(REG_BYTE(ea_rw), override);
    jit_ea_addr(type, reg, 1, override, a);
    a.mov(ARG1, x86::rbx);
    a.call(jit_read8);
}

void jit_ea_read16(int type, int reg, bool override, x86::Assembler &a) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        a.mov(x86::ax, JIT_REG_D_W(reg));
        return;
    case EA_OP::AR:
        a.mov(x86::ax, JIT_REG_A_W(reg));
        return;
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        if(reg == 4 && !override) {
            a.mov(x86::ax, FETCH());
            return;
        } else {
            throw ILLEGAL_INST_EX{};
        }
    default:
        break;
    }
    a.mov(REG_BYTE(ea_rw), override);
    jit_ea_addr(type, reg, 2, override, a);
    a.mov(ARG1, x86::rbx);
    a.call(jit_read16);
}

void jit_ea_read32(int type, int reg, bool override, x86::Assembler &a) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        a.mov(x86::eax, JIT_REG_D_L(reg));
        return;
    case EA_OP::AR:
        a.mov(x86::eax, JIT_REG_A_L(reg));
        return;
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        if(reg == 4 && !override) {
            a.mov(x86::eax, FETCH32());
            return;
        } else {
            throw ILLEGAL_INST_EX{};
        }
    default:
        break;
    }
    a.mov(REG_BYTE(ea_rw), override);
    jit_ea_addr(type, reg, 4, override, a);
    a.mov(ARG1, x86::rbx);
    a.call(jit_read32);
}

void jit_ea_write8(int type, int reg, x86::Assembler &a) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        a.mov(JIT_REG_D_B(reg), x86::al);
        return;
    case EA_OP::AR:
        throw ILLEGAL_INST_EX{};
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        throw ILLEGAL_INST_EX{};
    default:
        break;
    }
    jit_ea_addr(type, reg, 1, true, a);
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, x86::rax);
    a.call(jit_write8);
}

void jit_ea_write16(int type, int reg, x86::Assembler &a) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        a.mov(JIT_REG_D_W(reg), x86::ax);
        return;
    case EA_OP::AR:
        a.cwde();
        a.mov(JIT_REG_A_L(reg), x86::eax);
        return;
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        throw ILLEGAL_INST_EX{};
    default:
        break;
    }
    jit_ea_addr(type, reg, 2, true, a);
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, x86::rax);
    a.call(jit_write16);
}

void jit_ea_write32(int type, int reg, x86::Assembler &a) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        a.mov(JIT_REG_D_L(reg), x86::eax);
        return;
    case EA_OP::AR:
        a.mov(JIT_REG_A_L(reg), x86::eax);
        return;
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        throw ILLEGAL_INST_EX{};
    default:
        break;
    }
    jit_ea_addr(type, reg, 4, true, a);
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, x86::rax);
    a.call(jit_write32);
}

void jit_ea_update8(int type, int reg, x86::Assembler &a) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        a.mov(JIT_REG_D_B(reg), x86::al);
        return;
    case EA_OP::AR:
        throw ILLEGAL_INST_EX{};
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        throw ILLEGAL_INST_EX{};
    default:
        break;
    }
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, x86::rax);
    a.call(jit_write8);
}

void jit_ea_update16(int type, int reg, x86::Assembler &a) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        a.mov(JIT_REG_D_W(reg), x86::ax);
        return;
    case EA_OP::AR:
        a.cwde();
        a.mov(JIT_REG_A_L(reg), x86::eax);
        return;
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        throw ILLEGAL_INST_EX{};
    default:
        break;
    }
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, x86::rax);
    a.call(jit_write16);
}

void jit_ea_update32(int type, int reg, x86::Assembler &a) {
    switch(EA_OP{type}) {
    case EA_OP::DR:
        a.mov(JIT_REG_D_L(reg), x86::eax);
        return;
    case EA_OP::AR:
        a.mov(JIT_REG_A_L(reg), x86::eax);
        return;
    case EA_OP::EXT2:
        if(reg < 4) {
            break;
        }
        throw ILLEGAL_INST_EX{};
    default:
        break;
    }
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, x86::rax);
    a.call(jit_write32);
}