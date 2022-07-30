#include "compiler/jit.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "newcpu.h"

#include <asmjit/x86.h>
using namespace asmjit;
uint8_t do_nbcd(uint8_t v1);
uint8_t do_abcd(uint8_t a, uint8_t b);
uint8_t do_sbcd(uint8_t a, uint8_t b);
void jit_link(int reg, int32_t disp, x86::Assembler &a);
void jit_nbcd(uint16_t op, int type, int reg, x86::Assembler &a) {
    if(type == 1) {
        jit_link(reg, FETCH32(), a);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.mov(ARG1, x86::rax);
        a.call(do_nbcd);
        jit_ea_update8(type, reg, a);
    }
}
void do_sbcd_d(int reg, int dm, x86::Assembler &a) {
    a.movzx(eARG1, JIT_REG_D_B(dm));
    a.movzx(eARG2, JIT_REG_D_B(reg));
    a.call(do_sbcd);
    a.mov(JIT_REG_D_B(dm), x86::al);
}
void do_sbcd_m(int reg, int dm, x86::Assembler &a) {
    a.dec(JIT_REG_A_L(reg));
    a.mov(eARG1, JIT_REG_A_L(reg));
    a.call(jit_read8);
    a.mov(x86::r8b, x86::al);
    a.dec(JIT_REG_A_L(dm));
    a.mov(x86::ebx, JIT_REG_A_L(dm));
    a.mov(eARG1, x86::ebx);
    a.call(jit_read8);
    a.movzx(eARG1, x86::al);
    a.movzx(eARG2, x86::r8b);
    a.call(do_sbcd);
    a.mov(eARG1, x86::ebx);
    a.movzx(eARG2, x86::al);
    a.call(jit_write8);
}
uint8_t do_pack(uint16_t v);
void do_pack_d(int reg, int dm, uint16_t adjust, x86::Assembler &a) {
    a.mov(x86::ax, JIT_REG_D_W(reg));
    a.add(x86::ax, adjust);
    a.movzx(eARG1, x86::ax);
    a.call(do_pack);
    a.movzx(x86::ax, x86::al);
    a.mov(JIT_REG_D_W(dm), x86::ax);
}

void do_pack_m(int reg, int dm, uint16_t adjust, x86::Assembler &a) {
    a.mov(x86::rbx, JIT_REG_A_L(reg));
    a.lea(ARG1, x86::ptr(x86::rbx, -1));
    a.call(jit_read8);
    a.mov(x86::r10b, x86::al);
    a.lea(x86::rbx, x86::ptr(x86::rbx, -2));
    a.mov(eARG1, x86::ebx);
    a.call(jit_read8);
    a.mov(x86::r11b, x86::al);
    a.mov(JIT_REG_A_L(reg), x86::ebx);
    a.mov(x86::dl, x86::r11b);
    a.mov(x86::dh, x86::dl);
    a.mov(x86::dl, x86::r10b);
    a.add(x86::dx, adjust);
    a.movzx(eARG1, x86::dx);
    a.call(do_pack);
    a.mov(x86::edi, JIT_REG_A_L(dm));
    a.dec(x86::edi);
    a.mov(JIT_REG_A_L(dm), x86::edi);
    a.mov(eARG1, x86::edi);
    a.movzx(eARG2, x86::al);
    a.call(jit_write8);
}

uint8_t do_unpk(uint8_t v);
void do_unpk_d(int reg, int dm, uint16_t adjust, x86::Assembler &a) {
    a.mov(x86::al, JIT_REG_D_B(reg));
    a.movzx(eARG1, x86::al);
    a.call(do_unpk);
    a.add(x86::ax, adjust);
    a.mov(JIT_REG_D_W(dm), x86::ax);
}

void do_unpk_m(int reg, int dm, uint16_t adjust, x86::Assembler &a) {
    a.dec(JIT_REG_A_L(reg));
    a.mov(x86::esi, JIT_REG_A_L(reg));
    a.mov(eARG1, x86::esi);
    a.call(jit_read8);
    a.movzx(eARG1, x86::al);
    a.call(do_unpk);
    a.add(x86::ax, adjust);
    a.sub(JIT_REG_A_L(dm), 2);
    a.mov(eARG1, JIT_REG_A_L(dm));
    a.movzx(eARG2, x86::ax);
    a.call(jit_write16);
}

void do_abcd_d(int reg, int dm, x86::Assembler &a) {
    a.movzx(eARG1, JIT_REG_D_B(dm));
    a.movzx(eARG2, JIT_REG_D_B(reg));
    a.call(do_abcd);
    a.mov(JIT_REG_D_B(dm), x86::al);
}
void do_abcd_m(int reg, int dm, x86::Assembler &a) {
    a.dec(JIT_REG_A_L(reg));
    a.mov(eARG1, JIT_REG_A_L(reg));
    a.call(jit_read8);
    a.mov(x86::r8b, x86::al);
    a.dec(JIT_REG_A_L(dm));
    a.mov(x86::ebx, JIT_REG_A_L(dm));
    a.mov(eARG1, x86::ebx);
    a.call(jit_read8);
    a.movzx(eARG1, x86::al);
    a.movzx(eARG2, x86::r8b);
    a.call(do_abcd);
    a.mov(eARG1, x86::ebx);
    a.movzx(eARG2, x86::al);
    a.call(jit_write8);
}