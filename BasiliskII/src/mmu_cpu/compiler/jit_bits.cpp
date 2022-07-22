#include "compiler/jit.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
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
}

void jit_ror_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.ror(x86::ax, 1);
    a.setc(REG_BYTE(c));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
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
}

void jit_rol_ea(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(REG_BYTE(v), 0);
    jit_ea_read16(type, reg, true, a);
    a.rol(x86::ax, 1);
    a.setc(REG_BYTE(c));
    a.cmp(x86::ax, 0);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
}

