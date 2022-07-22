#include "compiler/jit.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "newcpu.h"

#include <asmjit/x86.h>
using namespace asmjit;
void CHK_ERROR();
void jit_subi_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint8_t v2 = FETCH();
    jit_ea_read8(type, reg, true, a);
    a.sub(x86::al, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update8(type, reg, a);
}

void jit_subi_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t v2 = FETCH();
    jit_ea_read16(type, reg, true, a);
    a.sub(x86::ax, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update16(type, reg, a);
}

void jit_subi_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint32_t v2 = FETCH32();
    jit_ea_read32(type, reg, true, a);
    a.sub(x86::eax, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update32(type, reg, a);
}

void jit_addi_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint8_t v2 = FETCH();
    jit_ea_read8(type, reg, true, a);
    a.add(x86::al, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update8(type, reg, a);
}

void jit_addi_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t v2 = FETCH();
    jit_ea_read16(type, reg, true, a);
    a.add(x86::ax, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update16(type, reg, a);
}

void jit_addi_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint32_t v2 = FETCH32();
    jit_ea_read32(type, reg, true, a);
    a.add(x86::eax, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update32(type, reg, a);
}
void jit_cmp2_chk2_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t op2 = FETCH();
    jit_ea_addr(type, reg, 1, false, a);
    a.push(x86::r10);
    a.push(x86::r11);
    a.mov(ARG1, x86::rbx);
    a.call(jit_read8);
    a.mov(x86::r10b, x86::al); // low
    a.lea(ARG1, x86::byte_ptr(x86::rbx, 1));
    a.call(jit_read8);
    a.mov(x86::r11b, x86::al); // high
    if(op2 & 0x8000) {
        // An vs Rn
        a.movsx(x86::r10d, x86::r10b);
        a.movsx(x86::r11d, x86::r11b);
        a.mov(x86::edx, JIT_REG_A_L(op2 >> 12 & 7));
        a.cmp(x86::r10d, x86::edx);
        a.sete(x86::cl);
        a.setg(x86::al);
        a.cmp(x86::edx, x86::r11d);
        a.sete(x86::ch);
        a.setg(x86::ah);
        a.or_(x86::cl, x86::ch);
        a.or_(x86::al, x86::ah);
        a.mov(REG_BYTE(z), x86::cl);
        a.mov(REG_BYTE(c), x86::al);
    } else {
        a.mov(x86::dl, JIT_REG_D_B(op2 >> 12 & 7));
        a.cmp(x86::r10b, x86::dl);
        a.sete(x86::cl);
        a.seta(x86::al);
        a.cmp(x86::dl, x86::r11b);
        a.sete(x86::ch);
        a.seta(x86::ah);
        a.or_(x86::cl, x86::ch);
        a.or_(x86::al, x86::ah);
        a.mov(REG_BYTE(z), x86::cl);
        a.mov(REG_BYTE(c), x86::al);
    }
    if(op2 & 0x800) {
        Label lb = a.newLabel();
        a.cmp(x86::al, 1);
        a.jne(lb);
        a.call(CHK_ERROR);
        a.bind(lb);
    }
    a.pop(x86::r11);
    a.pop(x86::r10);
}

void jit_cmp2_chk2_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t op2 = FETCH();
    jit_ea_addr(type, reg, 2, false, a);
    a.push(x86::r10);
    a.push(x86::r11);
    a.mov(ARG1, x86::rbx);
    a.call(jit_read16);
    a.mov(x86::r10w, x86::ax); // low
    a.lea(ARG1, x86::word_ptr(x86::rbx, 2));
    a.call(jit_read16);
    a.mov(x86::r11w, x86::ax); // high
    if(op2 & 0x8000) {
        // An vs Rn
        a.movsx(x86::r10d, x86::r10w);
        a.movsx(x86::r11d, x86::r11w);
        a.mov(x86::edx, JIT_REG_A_L(op2 >> 12 & 7));
        a.cmp(x86::r10d, x86::edx);
        a.sete(x86::cl);
        a.setg(x86::al);
        a.cmp(x86::edx, x86::r11d);
        a.sete(x86::ch);
        a.setg(x86::ah);
        a.or_(x86::cl, x86::ch);
        a.or_(x86::al, x86::ah);
        a.mov(REG_BYTE(z), x86::cl);
        a.mov(REG_BYTE(c), x86::al);
    } else {
        a.mov(x86::dx, JIT_REG_D_W(op2 >> 12 & 7));
        a.cmp(x86::r10w, x86::dx);
        a.sete(x86::cl);
        a.seta(x86::al);
        a.cmp(x86::dx, x86::r11w);
        a.sete(x86::ch);
        a.seta(x86::ah);
        a.or_(x86::cl, x86::ch);
        a.or_(x86::al, x86::ah);
        a.mov(REG_BYTE(z), x86::cl);
        a.mov(REG_BYTE(c), x86::al);
    }
    if(op2 & 0x800) {
        Label lb = a.newLabel();
        a.cmp(x86::al, 1);
        a.jne(lb);
        a.call(CHK_ERROR);
        a.bind(lb);
    }
    a.pop(x86::r11);
    a.pop(x86::r10);
}

void jit_cmp2_chk2_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t op2 = FETCH();
    jit_ea_addr(type, reg, 4, false, a);
    a.push(x86::r10);
    a.push(x86::r11);
    a.mov(ARG1, x86::rbx);
    a.call(jit_read32);
    a.mov(x86::r10d, x86::eax); // low
    a.lea(ARG1, x86::dword_ptr(x86::rbx, 4));
    a.call(jit_read32);
    a.mov(x86::r11d, x86::eax); // high
    if(op2 & 0x8000) {
        // An vs Rn
        a.mov(x86::edx, JIT_REG_A_L(op2 >> 12 & 7));
        a.cmp(x86::r10d, x86::edx);
        a.sete(x86::cl);
        a.setg(x86::al);
        a.cmp(x86::edx, x86::r11d);
        a.sete(x86::ch);
        a.setg(x86::ah);
        a.or_(x86::cl, x86::ch);
        a.or_(x86::al, x86::ah);
        a.mov(REG_BYTE(z), x86::cl);
        a.mov(REG_BYTE(c), x86::al);
    } else {
        a.mov(x86::edx, JIT_REG_D_L(op2 >> 12 & 7));
        a.cmp(x86::r10d, x86::edx);
        a.sete(x86::cl);
        a.seta(x86::al);
        a.cmp(x86::edx, x86::r11d);
        a.sete(x86::ch);
        a.seta(x86::ah);
        a.or_(x86::cl, x86::ch);
        a.or_(x86::al, x86::ah);
        a.mov(REG_BYTE(z), x86::cl);
        a.mov(REG_BYTE(c), x86::al);
    }
    if(op2 & 0x800) {
        Label lb = a.newLabel();
        a.cmp(x86::al, 1);
        a.jne(lb);
        a.call(CHK_ERROR);
        a.bind(lb);
    }
    a.pop(x86::r11);
    a.pop(x86::r10);
}

void jit_cmpi_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint8_t v2 = FETCH();
    jit_ea_read8(type, reg, false, a);
    a.cmp(x86::al, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
}

void jit_cmpi_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t v2 = FETCH();
    jit_ea_read16(type, reg, false, a);
    a.cmp(x86::ax, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
}

void jit_cmpi_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint32_t v2 = FETCH32();
    jit_ea_read32(type, reg, false, a);
    a.cmp(x86::eax, v2);
    a.setz(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
}
void jit_cas_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    Label lb1 = a.newLabel(), lb2 = a.newLabel();
    uint16_t op2 = FETCH();
    a.mov(REG_BYTE(traced), 1);
    jit_ea_addr(type, reg, 1, true, a);
    a.mov(ARG1, x86::rbx);
    a.call(jit_read8); // d
    a.mov(x86::dl, JIT_REG_D_B(op2 & 7));
    a.cmp(x86::al, x86::dl);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.jne(lb1);
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, JIT_REG_D_B(op2 >> 6 & 7));
    a.call(jit_write8);
    a.jmp(lb2);
    a.bind(lb1);
    a.mov(JIT_REG_D_B(op2 & 7), x86::al);
    a.bind(lb2);
}

void jit_cas_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    Label lb1 = a.newLabel(), lb2 = a.newLabel();
    uint16_t op2 = FETCH();
    a.mov(REG_BYTE(traced), 1);
    jit_ea_addr(type, reg, 2, true, a);
    a.mov(ARG1, x86::rbx);
    a.call(jit_read16); // d
    a.mov(x86::dx, JIT_REG_D_W(op2 & 7));
    a.cmp(x86::ax, x86::dx);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.jne(lb1);
    a.mov(ARG1, x86::rbx);
    a.mov(ARG2, JIT_REG_D_W(op2 >> 6 & 7));
    a.call(jit_write16);
    a.jmp(lb2);
    a.bind(lb1);
    a.mov(JIT_REG_D_W(op2 & 7), x86::ax);
    a.bind(lb2);
}

void jit_cas_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    Label lb1 = a.newLabel(), lb2 = a.newLabel();
    uint16_t op2 = FETCH();
    a.mov(REG_BYTE(traced), 1);
    jit_ea_addr(type, reg, 2, true, a);
    a.mov(ARG1, x86::rbx);
    a.call(jit_read32); // d
    a.mov(x86::edx, JIT_REG_D_L(op2 & 7));
    a.cmp(x86::eax, x86::edx);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.jne(lb1);
    a.mov(ARG1, x86::rbx);
    a.mov(eARG2, JIT_REG_D_L(op2 >> 6 & 7));
    a.call(jit_write32);
    a.jmp(lb2);
    a.bind(lb1);
    a.mov(JIT_REG_D_L(op2 & 7), x86::eax);
    a.bind(lb2);
}

void jit_negx_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read8(type, reg, true, a);
    a.add(x86::al, REG_BYTE(x));
    a.neg(x86::al);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update8(type, reg, a);
}

void jit_negx_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read16(type, reg, true, a);
    a.add(x86::ax, REG_BYTE(x));
    a.neg(x86::ax);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update16(type, reg, a);
}

void jit_negx_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read32(type, reg, true, a);
    a.add(x86::eax, REG_BYTE(x));
    a.neg(x86::eax);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update32(type, reg, a);
}

void jit_clr_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(x86::al, 0);
    a.mov(REG_BYTE(z), 1);
    a.mov(REG_BYTE(n), 0);
    a.mov(REG_BYTE(c), 0);
    a.mov(REG_BYTE(v), 0);
    jit_ea_write8(type, reg, a);
}

void jit_clr_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(x86::ax, 0);
    a.mov(REG_BYTE(z), 1);
    a.mov(REG_BYTE(n), 0);
    a.mov(REG_BYTE(c), 0);
    a.mov(REG_BYTE(v), 0);
    jit_ea_write16(type, reg, a);
}

void jit_clr_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    a.mov(x86::eax, 0);
    a.mov(REG_BYTE(z), 1);
    a.mov(REG_BYTE(n), 0);
    a.mov(REG_BYTE(c), 0);
    a.mov(REG_BYTE(v), 0);
    jit_ea_write32(type, reg, a);
}

void jit_neg_b(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read8(type, reg, true, a);
    a.neg(x86::al);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update8(type, reg, a);
}

void jit_neg_w(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read16(type, reg, true, a);
    a.neg(x86::ax);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update16(type, reg, a);
}

void jit_neg_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    jit_ea_read32(type, reg, true, a);
    a.neg(x86::eax);
    a.sete(REG_BYTE(z));
    a.sets(REG_BYTE(n));
    a.seto(REG_BYTE(v));
    a.setc(REG_BYTE(c));
    a.setc(REG_BYTE(x));
    jit_ea_update32(type, reg, a);
}

void jit_mul_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t op2 = FETCH();
    int dl = op2 >> 12 & 7;
    int dh = op2 & 7;
    a.mov(REG_BYTE(c), 0);
    jit_ea_read32(type, reg, false, a);
    a.mov(x86::edx, JIT_REG_D_L(dl));
    if(op2 & 1 << 10) {
        // 64bit multiply
        a.mov(REG_BYTE(v), 0);
        if(op2 & 1 << 11) {
            // muls
            a.imul(x86::edx);
        } else {
            // mulu
            a.mul(x86::edx);
        }
        a.mov(JIT_REG_D_L(dh), x86::edx);
        a.mov(JIT_REG_D_L(dl), x86::eax);
        a.cmp(x86::edx, 0);
        a.sets(REG_BYTE(n));
        a.test(x86::edx, x86::eax);
        a.sete(REG_BYTE(z));
    } else {
        // 32bit multiply
        if(op2 & 1 << 11) {
            // muls
            a.imul(x86::eax, x86::edx);
        } else {
            // mulu
            a.mul(x86::edx);
        }
        a.seto(REG_BYTE(v));
        a.test(x86::eax, x86::eax);
        a.sete(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(JIT_REG_D_L(dl), x86::eax);
    }
}
uint64_t do_divs(int32_t ah, uint32_t al, int32_t b);
uint64_t do_divu(uint32_t ah, uint32_t al, uint32_t b);
void jit_div_l(uint16_t op, int type, int reg, x86::Assembler &a) {
    uint16_t op2 = FETCH();
    int dq = op2 >> 12 & 7;
    int dr = op2 & 7;
    a.mov(REG_BYTE(c), 0);
    jit_ea_read32(type, reg, false, a);
    a.mov(ARG3, x86::rax);
    a.mov(eARG2, JIT_REG_D_L(dq));
    if(op2 & 1 << 11) {
        // divs
        if(op2 & 1 << 10) {
            a.mov(eARG1, JIT_REG_D_L(dr));
        } else {
            a.mov(eARG1, eARG2);
            a.sar(eARG1, 31);
        }
        a.call(do_divs);
    } else {
        // divu
        if(op2 & 1 << 10) {
            a.mov(eARG1, JIT_REG_D_L(dr));
        } else {
            a.mov(eARG1, 0);
        }
        a.call(do_divu);
    }
    a.mov(JIT_REG_D_L(dq), x86::eax);
    if(dq != dr) {
        a.shr(x86::rax, 32);
        a.mov(JIT_REG_D_L(dr), x86::eax);
    }
}

