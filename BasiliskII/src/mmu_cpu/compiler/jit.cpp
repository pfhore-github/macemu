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

#include <asmjit/x86.h>
#include <setjmp.h>
using namespace asmjit;
jmp_buf jit_ex;
typedef int (*Func)(void);
void jit_buserror() {
    uint32_t ea_v = regs.err_ssw.cm ? regs.i_ea : 0;
    std::vector<uint16_t> data(26u, 0);
    data[18] = LOW(regs.err_address);
    data[19] = HIGH(regs.err_address);
    data[23] = regs.err_ssw.to_value();
    data[24] = LOW(ea_v);
    data[25] = HIGH(ea_v);
    RAISE(2, 7, data, false);
    longjmp(jit_ex, 1);
}
uint32_t jit_read8(uint32_t addr) {
    try {
        return read8(addr);
    } catch(BUS_ERROR_EX &) {
        jit_buserror();
    }
    return 0;
}

uint32_t jit_read16(uint32_t addr) {
    try {
        return read16(addr);
    } catch(BUS_ERROR_EX &) {
        jit_buserror();
    }
    return 0;
}

uint32_t jit_read32(uint32_t addr) {
    try {
        return read32(addr);
    } catch(BUS_ERROR_EX &) {
        jit_buserror();
    }
    return 0;
}

void jit_write8(uint32_t addr, uint32_t v) {
    try {
        write8(addr, v);
    } catch(BUS_ERROR_EX &) {
        jit_buserror();
    }
}

void jit_write16(uint32_t addr, uint32_t v) {
    try {
        write16(addr, v);
    } catch(BUS_ERROR_EX &) {
        jit_buserror();
    }
}

void jit_write32(uint32_t addr, uint32_t v) {
    try {
        write32(addr, v);
    } catch(BUS_ERROR_EX &) {
        jit_buserror();
    }
}

void jit_compile(x86::Assembler& a, uint32_t from, uint32_t to) {
    
}
int jit() {
    JitRuntime rt;   // Runtime specialized for JIT code execution.
    CodeHolder code; // Holds code and relocation information.
    code.init(
        rt.environment());   // Initialize code to match the JIT environment.
    x86::Assembler a(&code); // Create and attach x86::Assembler to code.
    a.mov(x86::eax, 1);      // Move one to eax register.
    a.ret();                 // Return from function.
    // ===== x86::Assembler is no longer needed from here and can be
    // destroyed
    // =====
    Func fn;                        // Holds address to the generated function.
    Error err = rt.add(&fn, &code); // Add the generated code to the runtime.
    if(err)
        return 1; // Handle a possible error returned by AsmJit.
    // ===== CodeHolder is no longer needed from here and can be destroyed
    // =====
    int result = fn();      // Execute the generated code.
    printf("%d\n", result); // Print the resulting "1".
    // All classes use RAII, all resources will be released before `main()`
    // returns, the generated function can be, however, released explicitly
    // if you intend to reuse or keep the runtime alive, which you should in
    // a production-ready code.
    rt.release(fn);

    return 0;
}