#include "compiler/jit.h"
#include "cpu_emulation.h"
#include "ea.h"
#include "emul_op.h"
#include "newcpu.h"

#include <asmjit/x86.h>
using namespace asmjit;
uint8_t do_nbcd(uint8_t v1);
void jit_link(int reg, int32_t disp, x86::Assembler &a);
void jit_nbcd(uint16_t op, int type, int reg, x86::Assembler &a) {
    if(type == 1) {
        jit_link(reg, FETCH32(), a);
    } else {
        jit_ea_read8(type, reg, true, a);
        a.mov(ARG1, x86::rax);
        a.call(do_nbcd);
        jit_ea_write8(type, reg, a);
    }
}