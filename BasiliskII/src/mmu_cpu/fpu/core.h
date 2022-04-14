/*
 *  fpu/core.h - base fpu context definition
 *
 *  Basilisk II (C) 1997-2008 Christian Bauer
 *
 *  MC68881/68040 fpu emulation
 *  
 *  Original UAE FPU, copyright 1996 Herman ten Brugge
 *  Rewrite for x86, copyright 1999-2000 Lauri Pesonen
 *  New framework, copyright 2000 Gwenole Beauchesne
 *  Adapted for JIT compilation (c) Bernd Meyer, 2000
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef FPU_CORE_H
#define FPU_CORE_H

#include "sysdeps.h"
#include "fpu/types.h"

/* Always use x87 FPU stack on IA-32.  */
#if defined(X86_ASSEMBLY)
#define USE_X87_ASSEMBLY 1
#endif

/* Only use x87 FPU on x86-64 if long double precision is requested.  */
#if defined(X86_64_ASSEMBLY) && USE_LONG_DOUBLE
#define USE_X87_ASSEMBLY 1
#endif

/* ========================================================================== */
/* ========================= FPU CONTEXT DEFINITION ========================= */
/* ========================================================================== */

/*	We don't use all features of the C++ language so that we may still
 *	easily backport that code to C.
 */

struct fpu_t {

	double fp[8];
	struct FPCR_t {
		uint8_t rounding_mode;
		uint8_t rounding_prec;
		bool INEX1;
		bool INEX2;
		bool DZ;
		bool UNFL;
		bool OVFL;
		bool operr;
		bool snan;
		bool bsun;
	} fpcr;
	struct FPSR_t {
		bool nan;
		bool i;
		bool z;
		bool n;
		uint16_t QUITIENT;
		bool s;
	};
	uint32_t fpiar;	
};

/* We handle only one global fpu */
extern fpu_t fpu;

/* Return the address of a particular register */
inline double * const fpu_register_address(int i)
	{ return &fpu.fp[i]; }

/* Dump functions for m68k_dumpstate */
extern void fpu_dump_registers(void);
extern void fpu_dump_flags(void);

/* Accessors to FPU Control Register */
static inline uae_u32 get_fpcr(void);
static inline void set_fpcr(uae_u32 new_fpcr);

/* Accessors to FPU Status Register */
static inline uae_u32 get_fpsr(void);
static inline void set_fpsr(uae_u32 new_fpsr);
	
/* Accessors to FPU Instruction Address Register */
static inline uae_u32 get_fpiar();
static inline void set_fpiar(uae_u32 new_fpiar);

/* Initialization / Finalization */
extern void fpu_init(bool integral_68040);
extern void fpu_exit(void);
extern void fpu_reset(void);
	
/* Floating-point arithmetic instructions */
void fpuop_arithmetic(uae_u32 opcode, uae_u32 extra) REGPARAM;

/* Floating-point program control operations */
void fpuop_bcc(uae_u32 opcode, uaecptr pc, uae_u32 extra) REGPARAM;
void fpuop_dbcc(uae_u32 opcode, uae_u32 extra) REGPARAM;
void fpuop_scc(uae_u32 opcode, uae_u32 extra) REGPARAM;

/* Floating-point system control operations */
void fpuop_save(uae_u32 opcode) REGPARAM;
void fpuop_restore(uae_u32 opcode) REGPARAM;
void fpuop_trapcc(uae_u32 opcode, uaecptr oldpc) REGPARAM;

#endif /* FPU_CORE_H */
