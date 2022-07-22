#include "compiler/jit.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "newcpu.h"

#include <asmjit/x86.h>
using namespace asmjit;

// Bit field ops are too compicated to write open code.

void do_bftst(int type, int reg, uint16_t op2) ;
void do_bfchg(int type, int reg, uint16_t op2) ;
void do_bfset(int type, int reg, uint16_t op2) ;
void do_bfclr(int type, int reg, uint16_t op2) ;
void do_bfextu(int type, int reg, uint16_t op2);
void do_bfexts(int type, int reg, uint16_t op2);
void do_bfffo(int type, int reg, uint16_t op2);
void do_bfins(int type, int reg, uint16_t op2);

void jit_bftst(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t op2 = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, op2);
    a.call(do_bftst);
}

void jit_bfchg(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t op2 = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, op2);
    a.call(do_bfchg);
}

void jit_bfset(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t op2 = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, op2);
    a.call(do_bfset);
}

void jit_bfclr(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t op2 = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, op2);
    a.call(do_bfclr);
}

void jit_bfextu(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t op2 = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, op2);
    a.call(do_bfextu);
}

void jit_bfexts(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t op2 = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, op2);
    a.call(do_bfexts);
}

void jit_bfffo(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t op2 = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, op2);
    a.call(do_bfffo);
}

void jit_bfins(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    uint16_t op2 = FETCH();
    a.mov(ARG1, type);
    a.mov(ARG2, reg);
    a.mov(ARG3, op2);
    a.call(do_bfins);
}
