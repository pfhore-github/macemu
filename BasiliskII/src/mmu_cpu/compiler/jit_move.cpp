#include "compiler/jit.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "exception.h"
#include "mbus.h"
#include "newcpu.h"
#include <asmjit/x86.h>
#include <setjmp.h>
using namespace asmjit;
void PRIV_ERROR();
extern jmp_buf jit_ex;
void m68k_do_buserror(BUS_ERROR_EX &e);
void do_moves_b_w(uint32_t addr, int rg2) {
    try {
        b_write8(addr, regs.r[rg2]);
    } catch(BUS_ERROR_EX &e) {
        e.tt = TT::LFC;
        e.tm = TM(regs.dfc);
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}

void do_moves_b_r(uint32_t addr, int rg2) {
    try {
        WRITE_D8(rg2, b_read8(addr));
    } catch(BUS_ERROR_EX &e) {
        e.tt = TT::LFC;
        e.tm = TM(regs.sfc);
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}

void do_moves_w_w(uint32_t addr, int rg2) {
    try {
        b_write16(addr, regs.r[rg2]);
    } catch(BUS_ERROR_EX &e) {
        e.tt = TT::LFC;
        e.tm = TM(regs.dfc);
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}

void do_moves_w_r(uint32_t addr, int rg2) {
    try {
        WRITE_D16(rg2, b_read16(addr));
    } catch(BUS_ERROR_EX &e) {
        e.tt = TT::LFC;
        e.tm = TM(regs.sfc);
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}

void do_moves_l_w(uint32_t addr, int rg2) {
    try {
        b_write32(addr, regs.r[rg2]);
    } catch(BUS_ERROR_EX &e) {
        e.tt = TT::LFC;
        e.tm = TM(regs.dfc);
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}

void do_moves_l_r(uint32_t addr, int rg2) {
    try {
        regs.r[rg2] = b_read32(addr);
    } catch(BUS_ERROR_EX &e) {
        e.tt = TT::LFC;
        e.tm = TM(regs.sfc);
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}

void jit_moves_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t op2 = FETCH();
    int rg2 = op2 >> 12 & 7;
    bool w = op2 >> 11 & 1;
    Label lb1 = a.newLabel(), lb2 = a.newLabel();
    a.mov(x86::cl, REG_BYTE(S));
    a.cmp(x86::cl, 1);
    a.je(lb1);
    a.call(PRIV_ERROR);
    a.jmp(lb2);
    a.bind(lb1);
    a.mov(REG_BYTE(traced), 1);
    jit_ea_addr(type, reg, 1, w, a);
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, rg2);
    if(w) {
        a.call(do_moves_b_w);
    } else {
        a.call(do_moves_b_r);
    }
    a.bind(lb2);
}

void jit_moves_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t op2 = FETCH();
    int rg2 = op2 >> 12 & 7;
    bool w = op2 >> 11 & 1;
    Label lb1 = a.newLabel(), lb2 = a.newLabel();
    a.mov(x86::cl, REG_BYTE(S));
    a.cmp(x86::cl, 1);
    a.je(lb1);
    a.call(PRIV_ERROR);
    a.jmp(lb2);
    a.bind(lb1);
    a.mov(REG_BYTE(traced), 1);
    jit_ea_addr(type, reg, 2, w, a);
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, rg2);
    if(w) {
        a.call(do_moves_w_w);
    } else {
        a.call(do_moves_w_r);
    }
    a.bind(lb2);
}

void jit_moves_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t op2 = FETCH();
    int rg2 = op2 >> 12 & 7;
    bool w = op2 >> 11 & 1;
    Label lb1 = a.newLabel(), lb2 = a.newLabel();
    a.mov(x86::cl, REG_BYTE(S));
    a.cmp(x86::cl, 1);
    a.je(lb1);
    a.call(PRIV_ERROR);
    a.jmp(lb2);
    a.bind(lb1);
    a.mov(REG_BYTE(traced), 1);
    jit_ea_addr(type, reg, 4, w, a);
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, rg2);
    if(w) {
        a.call(do_moves_l_w);
    } else {
        a.call(do_moves_l_r);
    }
    a.bind(lb2);
}

void jit_move_from_sr(uint16_t op, int type, int reg, x86::Assembler &a) {
    Label lb1 = a.newLabel(), lb2 = a.newLabel();
    a.mov(x86::cl, REG_BYTE(S));
    a.cmp(x86::cl, 1);
    a.je(lb1);
    a.call(PRIV_ERROR);
    a.jmp(lb2);
    a.bind(lb1);
    a.mov(REG_BYTE(traced), 1);
    a.call(GET_SR);
    jit_ea_write16(type, reg, a);
    a.bind(lb2);
}

void jit_move_from_ccr(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.call(GET_CCR);
    a.mov(x86::ax, x86::al);
    jit_ea_write16(type, reg, a);
}

void jit_move_to_ccr(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read16(type, reg, false, a);
    a.movzx(ARG1, x86::al);
    a.call(SET_CCR);
}

void jit_move_to_sr(uint16_t op, int type, int reg, x86::Assembler &a) {
    Label lb1 = a.newLabel(), lb2 = a.newLabel();
    a.mov(x86::cl, REG_BYTE(S));
    a.cmp(x86::cl, 1);
    a.je(lb1);
    a.call(PRIV_ERROR);
    a.jmp(lb2);
    a.bind(lb1);
    a.mov(REG_BYTE(traced), 1);
    jit_ea_read16(type, reg, false, a);
    a.movzx(ARG1, x86::ax);
    a.call(SET_SR);
    a.bind(lb2);
}

void jit_lea(uint16_t op, int type, int reg,  x86::Assembler &a) {
    if(type == 0) {
        a.movsx(x86::eax, JIT_REG_D_B(reg));
        a.test(x86::eax, x86::eax);
        a.sete(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        a.mov(JIT_REG_D_L(reg), x86::eax);
    } else {
        jit_ea_addr(type, op & 7, 0, false, a);
        a.mov(JIT_REG_A_L(op >> 9 & 7), x86::ebx);
    }
}

void jit_pea(int type, int reg, x86::Assembler &a) {
    jit_ea_addr(type, reg, 0, false, a);
    a.mov(x86::edi, JIT_REG_A_L(7));
    a.sub(x86::edi, 4);
    a.mov(JIT_REG_A_L(7), x86::edi);
    a.mov(ARG1, x86::rdi);
    a.mov(ARG2, x86::rbx);
    a.call(jit_write32);
}

void do_movem_to_w(int type, int reg, uint16_t reg_list);
void jit_do_movem_to_w(int type, int reg, uint16_t reg_list) {
    try {
        do_movem_to_w(type, reg, reg_list);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}

void jit_movem_to_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    if(type == 0) {
        a.movsx(x86::ax, JIT_REG_D_B(reg));
        a.test(x86::ax, x86::ax);
        a.sete(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        a.mov(JIT_REG_D_W(reg), x86::ax);
    } else {
        uint16_t reg_list = FETCH();
        a.mov(ARG1, type);
        a.mov(ARG2, reg);
        a.mov(ARG3, reg_list);
        a.call(jit_do_movem_to_w);
    }
}

void do_movem_to_l(int type, int reg, uint16_t reg_list);
void jit_do_movem_to_l(int type, int reg, uint16_t reg_list) {
    try {
        do_movem_to_l(type, reg, reg_list);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}
void jit_movem_to_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    if(type == 0) {
        a.movsx(x86::eax, JIT_REG_D_W(reg));
        a.test(x86::eax, x86::eax);
        a.sete(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        a.mov(JIT_REG_D_L(reg), x86::eax);
    } else {
        uint16_t reg_list = FETCH();
        a.mov(ARG1, type);
        a.mov(ARG2, reg);
        a.mov(ARG3, reg_list);
        a.call(jit_do_movem_to_l);
    }
}

void do_movem_from_w(int type, int reg, uint32_t reg_list);
void jit_do_movem_from_w(int type, int reg, uint16_t reg_list) {
    try {
        do_movem_from_w(type, reg, reg_list);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}

void jit_movem_from_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t reg_list = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, reg_list);
    a.call(jit_do_movem_from_w);
}

void do_movem_from_l(int type, int reg, uint32_t reg_list) ;
void jit_do_movem_from_l(int type, int reg, uint16_t reg_list) {
    try {
        do_movem_from_l(type, reg, reg_list);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        longjmp(jit_ex, 1);
    }
}

void jit_movem_from_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t reg_list = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, reg_list);
    a.call(jit_do_movem_from_l);
}