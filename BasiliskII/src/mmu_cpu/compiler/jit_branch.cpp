#include "compiler/jit.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "exception.h"
#include "mbus.h"
#include "newcpu.h"
#include "op.h"
#include <asmjit/x86.h>
#include <setjmp.h>
#include <unordered_map>
using namespace asmjit;
extern std::unordered_map<uint32_t, Label> jit_labels;

void jit_link(int reg, int32_t disp, x86::Assembler &a) {
    a.sub(JIT_REG_A_L(7), 4);
    a.mov(ARG1, JIT_REG_A_L(7));
    a.mov(ARG2, JIT_REG_A_L(reg));
    a.call(jit_write32);
    a.mov(x86::eax, JIT_REG_A_L(7));
    a.mov(JIT_REG_A_L(reg), x86::eax);
    a.add(JIT_REG_A_L(7), disp);
}
void jit_illegal(uint32_t pc);
void do_stop(uint16_t v);
void do_rtd(int16_t disp);
uint32_t do_op_movec_from(int o);
void do_op_movec_to(int op, uint32_t v);
void jit_0471(uint16_t op, int type, int reg, x86::Assembler &a) {
    Label lb1 = a.newLabel(), lb2 = a.newLabel();
    switch(type) {
    case 0:
    case 1:
        // TRAP
        a.mov(ARG1, type << 3 | reg);
        a.call(TRAP);
        break;
    case 2:
        jit_link(reg, FETCH(), a);
        break;
    case 3:
        // UNLK
        a.mov(x86::ebx, JIT_REG_A_L(reg));
        a.mov(eARG1, x86::ebx);
        a.call(jit_read32);
        a.mov(JIT_REG_A_L(reg), x86::eax);
        a.add(x86::ebx, 4);
        a.mov(JIT_REG_A_L(7), x86::ebx);
        break;
    case 4:
        // MOVE TO USP
        a.mov(x86::cl, REG_BYTE(S));
        a.cmp(x86::cl, 1);
        a.je(lb1);
        a.call(PRIV_ERROR);
        a.jmp(lb2);
        a.bind(lb1);
        a.mov(REG_BYTE(traced), 1);
        a.mov(x86::eax, JIT_REG_A_L(reg));
        a.mov(REG_LWORD(usp), x86::eax);
        a.bind(lb2);
        break;
    case 5:
        // MOVE FROM USP
        a.mov(x86::cl, REG_BYTE(S));
        a.cmp(x86::cl, 1);
        a.je(lb1);
        a.call(PRIV_ERROR);
        a.jmp(lb2);
        a.bind(lb1);
        a.mov(REG_BYTE(traced), 1);
        a.mov(x86::eax, REG_LWORD(usp));
        a.mov(JIT_REG_A_L(reg), x86::eax);
        a.bind(lb2);
        break;
    case 6:
        switch(reg) {
        case 0:
            a.call(op_reset);
            break;
        case 1:
            a.call(op_nop);
            break;
        case 2: {
            uint16_t next = FETCH();
            a.mov(eARG1, next);
            a.call(do_stop);
            break;
        }
        case 3:
            // RTE
            a.call(op_rte);
            a.call(exit_jit);
            break;
        case 4:
            // RTD
            {
                int16_t disp = FETCH();
                a.mov(ARG1, disp);
                a.call(do_rtd);
                a.call(exit_jit);
                break;
            }
        case 5:
            // RTS
            a.call(op_rts);
            a.call(exit_jit);
            break;
        case 6:
            // TRAPV
            a.call(op_trapv);
            break;
        case 7:
            // RTS
            a.call(op_rtr);
            a.call(exit_jit);
            break;
        }
        break;
    case 7:
        if(reg == 2) {
            uint16_t next = FETCH();
            int dst = next >> 12 & 15;
            Label lb1 = a.newLabel(), lb2 = a.newLabel();
            a.mov(x86::cl, REG_BYTE(S));
            a.cmp(x86::cl, 1);
            a.je(lb1);
            a.call(PRIV_ERROR);
            a.jmp(lb2);
            a.bind(lb1);
            a.mov(REG_BYTE(traced), 1);
            a.mov(ARG1, next & 0xfff);
            a.call(do_op_movec_from);
            a.mov(JIT_REG_D_L(dst), x86::eax);
            a.bind(lb2);
        } else if(reg == 3) {
            uint16_t next = FETCH();
            int src = next >> 12 & 15;
            Label lb1 = a.newLabel(), lb2 = a.newLabel();
            a.mov(x86::cl, REG_BYTE(S));
            a.cmp(x86::cl, 1);
            a.je(lb1);
            a.call(PRIV_ERROR);
            a.jmp(lb2);
            a.bind(lb1);
            a.mov(REG_BYTE(traced), 1);
            a.mov(ARG1, next & 0xfff);
            a.mov(ARG2, JIT_REG_D_L(src));
            a.call(do_op_movec_to);
            a.bind(lb2);
        } else {
            a.mov(ARG1, regs.opc);
            a.call(jit_illegal);
        }
    }
}

void jit_JUMP(x86::Assembler &a) {
    Label ok = a.newLabel(), done = a.newLabel();
    a.test(x86::eax, 1);
    a.je(ok);
    // ADDRESS ERROR
    a.mov(ARG1, 3);
    a.btr(x86::eax, 0);
    a.mov(eARG2, x86::eax);
    a.mov(ARG3, 0);
    a.call(RAISE2);
    a.jmp(done);
    a.bind(ok);
    a.mov(REG_LWORD(pc), x86::eax);
    a.bind(done);
}

void jit_jsr(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(traced), 1);
    jit_ea_addr(type, reg, 0, false, a);
    a.mov(eARG1, JIT_REG_A_L(7));
    a.sub(ARG1, 4);
    a.mov(JIT_REG_A_L(7), eARG1);
    a.mov(eARG2, REG_LWORD(pc));
    a.call(jit_write32);
    a.mov(x86::eax, x86::ebx);
    jit_JUMP(a);
    a.call(exit_jit);
}

void jit_jmp(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(traced), 1);
    jit_ea_addr(type, reg, 0, false, a);
    a.mov(x86::eax, x86::ebx);
    jit_JUMP(a);
    a.call(exit_jit);
}
