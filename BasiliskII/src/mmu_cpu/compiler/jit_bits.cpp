#include "compiler/jit.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "ex_stack.h"
#include "intop.h"
#include "newcpu.h"
#include <asmjit/x86.h>
using namespace asmjit;
void PRIV_ERROR();
void jit_ori_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint8_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        a.call(GET_CCR);
        a.or_(x86::al, v2);
        a.mov(ARG1, x86::rax);
        a.call(SET_CCR);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.or_(x86::al, v2);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update8(type, reg, a);
    }
}
void jit_ori_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        Label lb1 = a.newLabel(), lb2 = a.newLabel();
        a.mov(x86::cl, REG_BYTE(S));
        a.cmp(x86::cl, 1);
        a.je(lb1);
        a.call(PRIV_ERROR);
        a.jmp(lb2);
        a.bind(lb1);
        a.mov(REG_BYTE(traced), 1);
        a.call(GET_SR);
        a.or_(x86::ax, v2);
        a.mov(ARG1, x86::rax);
        a.call(SET_SR);
        a.bind(lb2);
    } else {
        jit_ea_read16(type, reg, true, a);
        a.or_(x86::ax, v2);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update16(type, reg, a);
    }
}

void jit_ori_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint32_t v2 = FETCH32();
    jit_ea_read32(type, reg, true, a);
    a.or_(x86::eax, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    jit_ea_update32(type, reg, a);
}

void jit_andi_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint8_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        a.call(GET_CCR);
        a.and_(x86::al, v2);
        a.mov(ARG1, x86::rax);
        a.call(SET_CCR);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.and_(x86::al, v2);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update8(type, reg, a);
    }
}
void jit_andi_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        Label lb1 = a.newLabel(), lb2 = a.newLabel();
        a.mov(x86::cl, REG_BYTE(S));
        a.cmp(x86::cl, 1);
        a.je(lb1);
        a.call(PRIV_ERROR);
        a.jmp(lb2);
        a.bind(lb1);
        a.mov(REG_BYTE(traced), 1);
        a.call(GET_SR);
        a.and_(x86::ax, v2);
        a.mov(ARG1, x86::rax);
        a.call(SET_SR);
        a.bind(lb2);
    } else {
        jit_ea_read16(type, reg, true, a);
        a.and_(x86::ax, v2);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update16(type, reg, a);
    }
}

void jit_andi_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint32_t v2 = FETCH32();
    jit_ea_read32(type, reg, true, a);
    a.and_(x86::eax, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    jit_ea_update32(type, reg, a);
}

void jit_eori_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint8_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        a.call(GET_CCR);
        a.xor_(x86::al, v2);
        a.mov(ARG1, x86::rax);
        a.call(SET_CCR);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.xor_(x86::al, v2);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update8(type, reg, a);
    }
}
void jit_eori_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t v2 = FETCH();
    if(type == 7 && reg == 4) {
        Label lb1 = a.newLabel(), lb2 = a.newLabel();
        a.mov(x86::cl, REG_BYTE(S));
        a.cmp(x86::cl, 1);
        a.je(lb1);
        a.call(PRIV_ERROR);
        a.jmp(lb2);
        a.bind(lb1);
        a.mov(REG_BYTE(traced), 1);
        a.call(GET_SR);
        a.xor_(x86::ax, v2);
        a.mov(ARG1, x86::rax);
        a.call(SET_SR);
        a.bind(lb2);
    } else {
        jit_ea_read16(type, reg, true, a);
        a.xor_(x86::ax, v2);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update16(type, reg, a);
    }
}

void jit_eori_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint32_t v2 = FETCH32();
    jit_ea_read32(type, reg, true, a);
    a.xor_(x86::eax, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    jit_ea_update32(type, reg, a);
}

void jit_btst_imm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    if(type == 0) {
        int bn = FETCH() & 31;
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.bt(x86::eax, bn);
        a.setnc(REG_BYTE(z));
    } else {
        int bn = FETCH() & 7;
        jit_ea_read8(type, reg, false, a);
        a.bt(x86::al, bn);
        a.setnc(REG_BYTE(z));
    }
}

void jit_bchg_imm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    if(type == 0) {
        int bn = FETCH() & 31;
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.btc(x86::eax, bn);
        a.setnc(REG_BYTE(z));
        a.mov(JIT_REG_D_L(reg), x86::eax);
    } else {
        int bn = FETCH() & 7;
        jit_ea_read8(type, reg, true, a);
        a.btc(x86::al, bn);
        a.setnc(REG_BYTE(z));
        jit_ea_update8(type, reg, a);
    }
}

void jit_bclr_imm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    if(type == 0) {
        int bn = FETCH() & 31;
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.btr(x86::eax, bn);
        a.setnc(REG_BYTE(z));
        a.mov(JIT_REG_D_L(reg), x86::eax);
    } else {
        int bn = FETCH() & 7;
        jit_ea_read8(type, reg, true, a);
        a.btr(x86::al, bn);
        a.setnc(REG_BYTE(z));
        jit_ea_update8(type, reg, a);
    }
}

void jit_bset_imm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    if(type == 0) {
        int bn = FETCH() & 31;
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.bts(x86::eax, bn);
        a.setnc(REG_BYTE(z));
        a.mov(JIT_REG_D_L(reg), x86::eax);
    } else {
        int bn = FETCH() & 7;
        jit_ea_read8(type, reg, true, a);
        a.bts(x86::al, bn);
        a.setnc(REG_BYTE(z));
        jit_ea_update8(type, reg, a);
    }
}

void jit_not_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read8(type, reg, true, a);
    a.not_(x86::al);
    a.test(x86::al, x86::al);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    jit_ea_update8(type, reg, a);
}

void jit_not_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read16(type, reg, true, a);
    a.not_(x86::ax);
    a.test(x86::ax, x86::ax);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    jit_ea_update16(type, reg, a);
}

void jit_not_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read32(type, reg, true, a);
    a.not_(x86::eax);
    a.test(x86::eax, x86::eax);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    jit_ea_update32(type, reg, a);
}

void jit_tst_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read8(type, reg, false, a);
    a.test(x86::al, x86::al);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
}

void jit_tst_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read16(type, reg, false, a);
    a.test(x86::ax, x86::ax);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
}

void jit_tst_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read32(type, reg, false, a);
    a.test(x86::eax, x86::eax);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
}
void jit_illegal(uint32_t pc);
void jit_tas(uint16_t op, int type, int reg, x86::Assembler &a) {
    if(type == 7 && reg == 4) {
        a.mov(ARG1, regs.opc);
        a.call(jit_illegal);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.test(x86::al, x86::al);
        a.sete(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        a.or_(x86::al, 0x80);
        jit_ea_update8(type, reg, a);
    }
}

void jit_asr_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.sar(x86::ax, 1);
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    jit_ea_write16(type, reg, a);
}

void jit_lsr_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.shr(x86::ax, 1);
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    jit_ea_write16(type, reg, a);
}

void jit_roxr_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.mov(x86::cl, REG_BYTE(x));
    a.shr(x86::cl, 1);
    a.rcr(x86::ax, 1);
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    jit_ea_write16(type, reg, a);
}

void jit_ror_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.ror(x86::ax, 1);
    a.setc(REG_BYTE(c));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    jit_ea_write16(type, reg, a);
}
void jit_asl_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.sal(x86::ax, 1);
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    jit_ea_write16(type, reg, a);
}

void jit_lsl_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.shl(x86::ax, 1);
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    jit_ea_write16(type, reg, a);
}

void jit_roxl_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.mov(x86::cl, REG_BYTE(x));
    a.shr(x86::cl, 1);
    a.rcl(x86::ax, 1);
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    jit_ea_write16(type, reg, a);
}

void jit_rol_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.rol(x86::ax, 1);
    a.setc(REG_BYTE(c));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    jit_ea_write16(type, reg, a);
}
void jit_movep_w_from(int dm, int reg, x86::Assembler &a);
void jit_movep_l_from(int dm, int reg, x86::Assembler &a);
void jit_movep_w_to(int dm, int reg, x86::Assembler &a);
void jit_movep_l_to(int dm, int reg, x86::Assembler &a);
void jit_btst_dm(uint16_t op, int type, int reg, x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.mov(x86::ecx, JIT_REG_D_L(dm));
        a.bt(x86::eax, x86::ecx);
        a.setnc(REG_BYTE(z));
    } else if(type == 1) {
        jit_movep_w_from(dm, reg, a);
    } else {
        jit_ea_read8(type, reg, false, a);
        a.mov(x86::ecx, JIT_REG_D_L(dm));
        a.and_(x86::ecx, 7);
        a.bt(x86::eax, x86::ecx);
        a.setnc(REG_BYTE(z));
    }
}

void jit_bchg_dm(uint16_t op, int type, int reg, x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.mov(x86::ecx, JIT_REG_D_L(dm));
        a.btc(x86::eax, x86::ecx);
        a.mov(JIT_REG_D_L(reg), x86::eax);
        a.setnc(REG_BYTE(z));
    } else if(type == 1) {
        jit_movep_l_from(dm, reg, a);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.mov(x86::ecx, JIT_REG_D_L(dm));
        a.and_(x86::ecx, 7);
        a.btc(x86::eax, x86::ecx);
        a.setnc(REG_BYTE(z));
        jit_ea_update8(type, reg, a);
    }
}

void jit_bclr_dm(uint16_t op, int type, int reg, x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.mov(x86::ecx, JIT_REG_D_L(dm));
        a.btr(x86::eax, x86::ecx);
        a.mov(JIT_REG_D_L(reg), x86::eax);
        a.setnc(REG_BYTE(z));
    } else if(type == 1) {
        jit_movep_w_to(dm, reg, a);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.mov(x86::ecx, JIT_REG_D_L(dm));
        a.and_(x86::ecx, 7);
        a.btr(x86::eax, x86::ecx);
        a.setnc(REG_BYTE(z));
        jit_ea_update8(type, reg, a);
    }
}

void jit_bset_dm(uint16_t op, int type, int reg, x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.mov(x86::ecx, JIT_REG_D_L(dm));
        a.bts(x86::eax, x86::ecx);
        a.mov(JIT_REG_D_L(reg), x86::eax);
        a.setnc(REG_BYTE(z));
    } else if(type == 1) {
        jit_movep_l_to(dm, reg, a);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.mov(x86::ecx, JIT_REG_D_L(dm));
        a.and_(x86::ecx, 7);
        a.bts(x86::eax, x86::ecx);
        a.setnc(REG_BYTE(z));
        jit_ea_update8(type, reg, a);
    }
}

void jit_or_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    jit_ea_read8(type, reg, false, a);
    a.mov(x86::dl, JIT_REG_D_B(dm));
    a.or_(x86::dl, x86::al);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    a.mov(JIT_REG_D_B(dm), x86::dl);
}
void jit_or_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    jit_ea_read16(type, reg, false, a);
    a.mov(x86::dx, JIT_REG_D_W(dm));
    a.or_(x86::dx, x86::ax);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    a.mov(JIT_REG_D_W(dm), x86::dx);
}

void jit_or_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    int dm = op >> 9 & 7;
    jit_ea_read32(type, reg, false, a);
    a.mov(x86::edx, JIT_REG_D_L(dm));
    a.or_(x86::edx, x86::eax);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    a.mov(JIT_REG_D_L(dm), x86::edx);
}
void do_sbcd_d(int reg, int dm, x86::Assembler &a);
void do_sbcd_m(int reg, int dm, x86::Assembler &a);
void jit_or_to_ea_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        do_sbcd_d(reg, dm, a);
    } else if(type == 1) {
        do_sbcd_m(reg, dm, a);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.mov(x86::dl, JIT_REG_D_B(dm));
        a.or_(x86::al, x86::dl);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update8(type, reg, a);
    }
}
void do_pack_d(int reg, int dm, uint16_t adjust, x86::Assembler &a);
void do_pack_m(int reg, int dm, uint16_t adjust, x86::Assembler &a);
void do_unpk_d(int reg, int dm, uint16_t adjust, x86::Assembler &a);
void do_unpk_m(int reg, int dm, uint16_t adjust, x86::Assembler &a);
void jit_or_to_ea_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        uint16_t adjust = FETCH();
        do_pack_d(reg, dm, adjust, a);
    } else if(type == 1) {
        uint16_t adjust = FETCH();
        do_pack_m(reg, dm, adjust, a);
    } else {
        jit_ea_read16(type, reg, true, a);
        a.mov(x86::dx, JIT_REG_D_W(dm));
        a.or_(x86::ax, x86::dx);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update16(type, reg, a);
    }
}

void jit_or_to_ea_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        uint16_t adjust = FETCH();
        do_unpk_d(reg, dm, adjust, a);
    } else if(type == 1) {
        uint16_t adjust = FETCH();
        do_unpk_m(reg, dm, adjust, a);
    } else {
        jit_ea_read32(type, reg, true, a);
        a.mov(x86::edx, JIT_REG_D_L(dm));
        a.or_(x86::eax, x86::edx);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update32(type, reg, a);
    }
}
void jit_eor_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 1) {
        a.mov(eARG1, JIT_REG_A_L(reg));
        a.call(jit_read8);
        a.push(x86::rax);
        a.mov(eARG1, JIT_REG_A_L(dm));
        a.call(jit_read8);
        a.pop(x86::rdx);
        a.cmp(x86::al, x86::dl);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.seto(REG_BYTE(v));
        a.setc(REG_BYTE(c));
        a.add(JIT_REG_A_L(reg), 1);
        a.add(JIT_REG_A_L(dm), 1);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.mov(x86::dl, JIT_REG_D_B(dm));
        a.xor_(x86::al, x86::dl);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update8(type, reg, a);
    }
}
void jit_eor_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 1) {
        a.mov(eARG1, JIT_REG_A_L(reg));
        a.call(jit_read16);
        a.push(x86::rax);
        a.mov(eARG1, JIT_REG_A_L(dm));
        a.call(jit_read16);
        a.pop(x86::rdx);
        a.cmp(x86::ax, x86::dx);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.seto(REG_BYTE(v));
        a.setc(REG_BYTE(c));
        a.add(JIT_REG_A_L(reg), 2);
        a.add(JIT_REG_A_L(dm), 2);
    } else {
        jit_ea_read16(type, reg, true, a);
        a.mov(x86::dx, JIT_REG_D_W(dm));
        a.xor_(x86::ax, x86::dx);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update16(type, reg, a);
    }
}

void jit_eor_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 1) {
        a.mov(eARG1, JIT_REG_A_L(reg));
        a.call(jit_read32);
        a.push(x86::rax);
        a.mov(eARG1, JIT_REG_A_L(dm));
        a.call(jit_read32);
        a.pop(x86::rdx);
        a.cmp(x86::eax, x86::edx);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.seto(REG_BYTE(v));
        a.setc(REG_BYTE(c));
        a.add(JIT_REG_A_L(reg), 4);
        a.add(JIT_REG_A_L(dm), 4);
    } else {
        jit_ea_read32(type, reg, true, a);
        a.mov(x86::edx, JIT_REG_D_L(dm));
        a.xor_(x86::eax, x86::edx);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update32(type, reg, a);
    }
}

void jit_and_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    jit_ea_read8(type, reg, false, a);
    a.mov(x86::dl, JIT_REG_D_B(dm));
    a.and_(x86::dl, x86::al);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    a.mov(JIT_REG_D_B(dm), x86::dl);
}
void jit_and_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    jit_ea_read16(type, reg, false, a);
    a.mov(x86::dx, JIT_REG_D_W(dm));
    a.and_(x86::dx, x86::ax);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    a.mov(JIT_REG_D_W(dm), x86::dx);
}

void jit_and_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    int dm = op >> 9 & 7;
    jit_ea_read32(type, reg, false, a);
    a.mov(x86::edx, JIT_REG_D_L(dm));
    a.and_(x86::edx, x86::eax);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(REG_BYTE(v), 0);
    a.mov(REG_BYTE(c), 0);
    a.mov(JIT_REG_D_L(dm), x86::edx);
}
void do_abcd_d(int reg, int dm, x86::Assembler &a);
void do_abcd_m(int reg, int dm, x86::Assembler &a);
void jit_and_to_ea_b(uint16_t op, int type, int reg,
                     asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        do_abcd_d(reg, dm, a);
    } else if(type == 1) {
        do_abcd_m(reg, dm, a);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.mov(x86::dl, JIT_REG_D_B(dm));
        a.and_(x86::al, x86::dl);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update8(type, reg, a);
    }
}
void jit_and_to_ea_w(uint16_t op, int type, int reg,
                     asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.xchg(JIT_REG_D_L(dm), x86::eax);
        a.xchg(JIT_REG_D_L(reg), x86::eax);
    } else if(type == 1) {
        a.mov(x86::eax, JIT_REG_A_L(reg));
        a.xchg(JIT_REG_A_L(dm), x86::eax);
        a.xchg(JIT_REG_A_L(reg), x86::eax);
    } else {
        jit_ea_read16(type, reg, true, a);
        a.mov(x86::dx, JIT_REG_D_W(dm));
        a.and_(x86::ax, x86::dx);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update16(type, reg, a);
    }
}

void jit_and_to_ea_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    int dm = op >> 9 & 7;
    if(type == 0) {
        throw ILLEGAL_INST_EX{};
    } else if(type == 1) {
        a.mov(x86::eax, JIT_REG_A_L(reg));
        a.xchg(JIT_REG_D_L(dm), x86::eax);
        a.xchg(JIT_REG_A_L(reg), x86::eax);
    } else {
        jit_ea_read32(type, reg, true, a);
        a.mov(x86::edx, JIT_REG_D_L(dm));
        a.and_(x86::eax, x86::edx);
        a.setz(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
        jit_ea_update32(type, reg, a);
    }
}

void jit_shr_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    Label lb_0 = a.newLabel(), lb_1 = a.newLabel();
    a.mov(REG_BYTE(v), 0);
    a.mov(x86::al, JIT_REG_D_B(reg));
    switch(type) {
    case 0:
        // ASR.B(Imm)
        a.sar(x86::al, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 1:
        // LSR.B(Imm)
        a.shr(x86::al, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 2:
        // ROXR.B(Imm)
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcr(x86::al, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 3:
        // ROR.B(Imm)
        a.ror(x86::al, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        break;
    case 4:
        // ASR.B(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.sar(x86::al, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 5:
        // LSR.B(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.shr(x86::al, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 6:
        // ROXR.B(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(x86::dl, REG_BYTE(x));
        a.mov(REG_BYTE(c), x86::dl);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcr(x86::al, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 7:
        // ROR.B(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.ror(x86::al, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    }
    a.test(x86::al, x86::al);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(JIT_REG_D_B(reg), x86::al);
}
void jit_shr_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    Label lb_0 = a.newLabel(), lb_1 = a.newLabel();
    a.mov(REG_BYTE(v), 0);
    a.mov(x86::ax, JIT_REG_D_W(reg));
    switch(type) {
    case 0:
        // ASR.W(Imm)
        a.sar(x86::ax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 1:
        // LSR.W(Imm)
        a.shr(x86::ax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 2:
        // ROXR.W(Imm)
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcr(x86::ax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 3:
        // ROR.W(Imm)
        a.ror(x86::ax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        break;
    case 4:
        // ASR.W(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.sar(x86::ax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 5:
        // LSR.W(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.shr(x86::ax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 6:
        // ROXR.W(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(x86::dl, REG_BYTE(x));
        a.mov(REG_BYTE(c), x86::dl);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcr(x86::ax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 7:
        // ROR.W(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.ror(x86::ax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    }
    a.test(x86::ax, x86::ax);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(JIT_REG_D_W(reg), x86::ax);
}
void jit_shr_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    Label lb_0 = a.newLabel(), lb_1 = a.newLabel();
    a.mov(REG_BYTE(v), 0);
    a.mov(x86::eax, JIT_REG_D_L(reg));
    switch(type) {
    case 0:
        // ASR.L(Imm)
        a.sar(x86::eax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 1:
        // LSR.L(Imm)
        a.shr(x86::eax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 2:
        // ROXR.L(Imm)
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcr(x86::eax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 3:
        // ROR.L(Imm)
        a.ror(x86::eax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        break;
    case 4:
        // ASR.L(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.sar(x86::eax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 5:
        // LSR.L(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.shr(x86::eax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 6:
        // ROXR.L(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(x86::dl, REG_BYTE(x));
        a.mov(REG_BYTE(c), x86::dl);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcr(x86::eax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 7:
        // ROR.L(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.ror(x86::eax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    }
    a.test(x86::eax, x86::eax);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(JIT_REG_D_L(reg), x86::eax);
}

void jit_shl_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    Label lb_0 = a.newLabel(), lb_1 = a.newLabel();
    a.mov(REG_BYTE(v), 0);
    a.mov(x86::al, JIT_REG_D_B(reg));
    switch(type) {
    case 0:
        // ASL.B(Imm)
        a.movzx(eARG1, x86::al);
        a.mov(eARG2, dm ? dm : 8);
        a.call(DO_ASL_B);
        a.mov(JIT_REG_D_B(reg), x86::al);
        return;
    case 1:
        // LSR.B(Imm)
        a.shl(x86::al, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 2:
        // ROXR.B(Imm)
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcl(x86::al, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 3:
        // ROR.B(Imm)
        a.rol(x86::al, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        break;
    case 4:
        // ASR.B(Reg)
        a.movzx(eARG1, x86::al);
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.and_(x86::cl, 63);
        a.movzx(eARG2, x86::cl);
        a.call(DO_ASL_B);
        a.mov(JIT_REG_D_B(reg), x86::al);
        return;
    case 5:
        // LSR.B(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.shl(x86::al, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 6:
        // ROXR.B(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(x86::dl, REG_BYTE(x));
        a.mov(REG_BYTE(c), x86::dl);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcl(x86::al, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 7:
        // ROR.B(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.rol(x86::al, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    }
    a.test(x86::al, x86::al);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(JIT_REG_D_B(reg), x86::al);
}
void jit_shl_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    Label lb_0 = a.newLabel(), lb_1 = a.newLabel();
    a.mov(REG_BYTE(v), 0);
    a.mov(x86::ax, JIT_REG_D_W(reg));
    switch(type) {
    case 0:
        // ASL.W(Imm)
        a.movzx(eARG1, x86::ax);
        a.mov(eARG2, dm ? dm : 8);
        a.call(DO_ASL_W);
        a.mov(JIT_REG_D_W(reg), x86::ax);
        return;
    case 1:
        // LSL.W(Imm)
        a.shl(x86::ax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 2:
        // ROXL.W(Imm)
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcl(x86::ax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 3:
        // ROL.W(Imm)
        a.rol(x86::ax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        break;
    case 4:
        // ASL.W(Reg)
        a.movzx(eARG1, x86::ax);
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.and_(x86::cl, 63);
        a.movzx(eARG2, x86::cl);
        a.call(DO_ASL_W);
        a.mov(JIT_REG_D_W(reg), x86::ax);
        return;
    case 5:
        // LSL.W(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.shl(x86::ax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 6:
        // ROXL.W(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(x86::dl, REG_BYTE(x));
        a.mov(REG_BYTE(c), x86::dl);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcl(x86::ax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 7:
        // ROL.W(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.rol(x86::ax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    }
    a.test(x86::ax, x86::ax);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(JIT_REG_D_W(reg), x86::ax);
}
void jit_shl_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    int dm = op >> 9 & 7;
    Label lb_0 = a.newLabel(), lb_1 = a.newLabel();
    a.mov(REG_BYTE(v), 0);
    a.mov(x86::eax, JIT_REG_D_L(reg));
    switch(type) {
    case 0:
        // ASL.L(Imm)
        a.mov(eARG1, x86::eax);
        a.mov(eARG2, dm ? dm : 8);
        a.call(DO_ASL_L);
        a.mov(JIT_REG_D_L(reg), x86::eax);
        return;
    case 1:
        // LSL.L(Imm)
        a.shl(x86::eax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 2:
        // ROXL.L(Imm)
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcl(x86::eax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        break;
    case 3:
        // ROL.L(Imm)
        a.rol(x86::eax, dm ? dm : 8);
        a.setc(REG_BYTE(c));
        break;
    case 4:
        // ASL.L(Reg)
        a.mov(eARG1, x86::eax);
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.and_(x86::cl, 63);
        a.movzx(eARG2, x86::cl);
        a.call(DO_ASL_L);
        a.mov(JIT_REG_D_L(reg), x86::eax);
        return;
    case 5:
        // LSL.L(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.shl(x86::eax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 6:
        // ROXL.L(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(x86::dl, REG_BYTE(x));
        a.mov(REG_BYTE(c), x86::dl);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.mov(x86::dl, REG_BYTE(x));
        a.shr(x86::dl, 1);
        a.rcl(x86::eax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    case 7:
        // ROL.L(Reg)
        a.mov(x86::cl, JIT_REG_D_B(dm));
        a.test(x86::cl, x86::cl);
        a.jne(lb_0);
        a.mov(REG_BYTE(c), 0);
        a.jmp(lb_1);
        a.bind(lb_0);
        a.and_(x86::cl, 63);
        a.rol(x86::eax, x86::cl);
        a.setc(REG_BYTE(c));
        a.setc(REG_BYTE(x));
        a.bind(lb_1);
        break;
    }
    a.test(x86::eax, x86::eax);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.mov(JIT_REG_D_L(reg), x86::eax);
}