#ifndef EXCEPTION_H__
#define EXCEPTION_H__
#include "newcpu.h"
#include <stdint.h>
#include <stdlib.h>
#include <setjmp.h>
extern jmp_buf ex_jmp;
inline uint16_t LOW(uint32_t v) { return v & 0xffff; }
inline uint16_t HIGH(uint32_t v) { return v >> 16; }
void RAISE(int e, int pri, int f, const std::vector<uint16_t> &data, bool next);
inline void RAISE0(int e, int pri, bool next_instruction) {
    RAISE(e, pri, 0, {}, next_instruction);
}
inline void RAISE2(int e, int pri, uint32_t addr, bool next_instruction) {
    RAISE(e, pri, 2, {LOW(addr), HIGH(addr)}, next_instruction);
}
void BUSERROR(uint32_t addr, bool code, uint16_t ssw, uint32_t ea)
    __attribute__((noreturn));

inline void ADDRESS_ERROR() { RAISE2(3, 5, regs.opc - 1, false); }
inline __attribute__((noreturn)) void ILLEGAL_INST() {
    RAISE0(4, 3, false);
    longjmp(ex_jmp, 1);
}
inline void DIV0_ERROR() { RAISE2(5, 3, regs.opc, true); }
inline void CHK_ERROR() { RAISE2(6, 3, regs.opc, true); }
inline void TRPPcc() { RAISE2(7, 3, regs.opc, true); }
inline void PRIV_ERROR() { RAISE0(8, 3, false); }
inline void TRACE() { RAISE2(9, 3, regs.opc, true); }
inline void ALINE_EXCEPTION() { RAISE0(10, 3, false); }
inline void FORMAT_ERROR() { RAISE0(14, 3, false); }
inline void TRAP(int v) { RAISE0(32 + v, 3, true); }
inline void IRQ(int v) { RAISE0(25 + v, 8, true); }

#endif