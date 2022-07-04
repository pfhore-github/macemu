#ifndef EXCEPTION_H__
#define EXCEPTION_H__
#include "ex_stack.h"
#include "memory.h"
#include "newcpu.h"
#include <stdint.h>
#include <stdlib.h>
inline uint16_t LOW(uint32_t v) { return v & 0xffff; }
inline uint16_t HIGH(uint32_t v) { return v >> 16; }
void RAISE(int e, int f, const std::vector<uint16_t> &data, bool next,
           bool irq = false);
inline void RAISE0(int e, bool next_instruction, bool irq = false) {
    RAISE(e, 0, {}, next_instruction, irq);
}
inline void RAISE2(int e, uint32_t addr, bool next_instruction) {
    RAISE(e, 2, {LOW(addr), HIGH(addr)}, next_instruction);
}

inline void RAISE3(int e) {
    RAISE(e, 3, {LOW(regs.i_ea), HIGH(regs.i_ea)}, true);
}

inline __attribute__((noreturn)) void ILLEGAL_INST() { throw ILLEGAL_INST_EX{}; }
inline void DIV0_ERROR() { RAISE2(5, regs.opc, true); }
inline void CHK_ERROR() { RAISE2(6, regs.opc, true); }
inline void TRPPcc() { RAISE2(7, regs.opc, true); }
inline void PRIV_ERROR() { RAISE0(8, false); }
inline void TRACE() { RAISE2(9, regs.opc, true); }
inline void ALINE_EXCEPTION() { RAISE0(10, false); }
inline void FP_UNDEF() { RAISE0(11, false); }
inline void FORMAT_ERROR() { RAISE0(14, false); }
inline void TRAP(int v) { RAISE0(32 + v, true); }
inline void FP_UNCND() { RAISE0(48, false); }
inline void FP_INEX() { RAISE0(49, false); }
inline void FP_DIV0() { RAISE0(50, false); }
inline void FP_UNFL() { RAISE0(51, false); }
inline void FP_OPERR() { RAISE0(52, false); }
inline void FP_OVFR() { RAISE0(53, false); }
inline void FP_SNAN() { RAISE0(54, false); }
#endif