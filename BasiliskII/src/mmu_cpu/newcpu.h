// -*- c++ -*-
/*
 * UAE - The Un*x Amiga Emulator
 *
 * MC68000 emulation
 *
 * Copyright 1995 Bernd Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef NEWCPU_H
#define NEWCPU_H
#include <setjmp.h>
#include "spcflags.h"
#include "exception.h"
#include <memory>
#include <atomic>
struct fpu_reg {
};

struct mmu_t;
struct m68k_reg {
	union {
		struct {
			uint32_t d[8];
			uint32_t a[8];
		};
		uint32_t r[16];
	};
	uint32_t opc;
	uint32_t pc;
	bool c, v, z, n, x;
	uint8_t IM;
	bool M, S;
	uint8_t T;
	bool interrupt;
	bool exception;
	uint32_t isp;
	uint32_t usp;
	uint32_t msp;
	uint32_t vbr;
	uint8_t sfc, dfc;
	struct ttc_t {
		uint8_t address_base;
		uint8_t address_mask;
		bool E;
		uint8_t S;
		uint8_t U;
		uint8_t CM;
		bool W;
	};
	ttc_t ITT[2], DTT[2];
	struct cacr_t {
		bool DE, IE;
	} CACR;
	uint32_t i_ea;
	// 68040+native FPU
	fpu_reg fpu;
	// emulator flag
	std::atomic<uint32_t> spcflags;
};
// no multi cpu
extern m68k_reg regs;
void init_m68k();
void exit_m68k();
void m68k_reset();
void m68k_execute();
void m68k_compile_execute();
inline void JUMP(uint32_t pc) {
	regs.pc = pc;
}
inline uint8_t GET_CCR() {
	return regs.c | regs.v << 1 | regs.z << 2 | regs.n << 3 | regs.x << 4;
}

inline uint16_t GET_SR() {
	return regs.c | regs.v << 1 | regs.z << 2 | regs.n << 3 | regs.x << 4 |
		regs.IM << 8 | regs.M << 12 | regs.S << 13 | regs.T << 14;
}
inline void SET_CCR(uint8_t v) {
	regs.c = v & 1;
	regs.v = v >> 1 & 1;
	regs.z = v >> 2 & 1;
	regs.n = v >> 3 & 1;
	regs.x = v >> 4 & 1;
}
void SET_SR(uint16_t sr);

#endif /* NEWCPU_H */
