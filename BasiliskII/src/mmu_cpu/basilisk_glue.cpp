/*
 *  basilisk_glue.cpp - Glue UAE CPU to Basilisk II CPU engine interface
 *
 *  Basilisk II (C) 1997-2008 Christian Bauer
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

#include "sysdeps.h"

#include "cpu_emulation.h"
#include "main.h"
#include "prefs.h"
#include "emul_op.h"
#include "rom_patches.h"
#include "timer.h"
#include "memory.h"
#include "readcpu.h"
#include "newcpu.h"
#include "compiler/compemu.h"
#include <unordered_map>

// RAM and ROM pointers
uint32 RAMBaseMac = 0;		// RAM base (Mac address space) gb-- initializer is important
uint8 *RAMBaseHost;			// RAM base (host address space)
uint32 RAMSize;				// Size of RAM
uint32 ROMBaseMac;			// ROM base (Mac address space)
uint8 *ROMBaseHost;			// ROM base (host address space)
uint32 ROMSize;				// Size of ROM


#if USE_JIT
bool UseJIT = false;
#endif

// From newcpu.cpp
extern bool quit_program;


/*
 *  Initialize 680x0 emulation, CheckROM() must have been called first
 */

bool Init680x0(void)
{
	init_m68k();
#if USE_JIT
	UseJIT = compiler_use_jit();
	if (UseJIT)
	    compiler_init();
#endif
	return true;
}


/*
 *  Deinitialize 680x0 emulation
 */

void Exit680x0(void)
{
#if USE_JIT
    if (UseJIT)
	compiler_exit();
#endif
	exit_m68k();
}


/*
 *  Initialize memory mapping of frame buffer (called upon video mode change)
 */

void InitFrameBufferMapping(void)
{
}

/*
 *  Reset and start 680x0 emulation (doesn't return)
 */

void Start680x0(void)
{
//	m68k_reset();
#if USE_JIT
    if (UseJIT)
	m68k_compile_execute();
    else
#endif
	m68k_execute();
}


/*
 *  Trigger interrupt
 */

void TriggerInterrupt(void)
{
	idle_resume();
	SPCFLAGS_SET( SPCFLAG_INT );
}

void TriggerNMI(void)
{
	//!! not implemented yet
}


/*
 *  Get 68k interrupt level
 */

int intlev(void)
{
	return InterruptFlags ? 1 : 0;
}


/*
 *  Execute MacOS 68k trap
 *  r->a[7] and r->sr are unused!
 */

void Execute68kTrap(uint16 trap, struct M68kRegisters *r)
{
	int i;

	// Save old PC
	uaecptr oldpc = regs.pc;

	// Set registers
	for (i=0; i<8; i++)
		regs.d[i] = r->d[i];
	for (i=0; i<7; i++)
		regs.a[i] = r->a[i];

	// Push trap and EXEC_RETURN on stack
	PUSH16( M68K_EXEC_RETURN );
	PUSH16( trap );

	// Execute trap
	JUMP( regs.a[7] );
	quit_program = false;
	m68k_execute();

	// Clean up stack
	regs.a[7] += 4;

	// Restore old PC
	JUMP( oldpc );

	// Get registers
	for (i=0; i<8; i++)
		r->d[i] = regs.d[i];
	for (i=0; i<7; i++)
		r->a[i] = regs.a[i];
	quit_program = false;
}


/*
 *  Execute 68k subroutine
 *  The executed routine must reside in UAE memory!
 *  r->a[7] and r->sr are unused!
 */

void Execute68k(uint32 addr, struct M68kRegisters *r)
{
	int i;

	// Save old PC
	uaecptr oldpc = regs.pc;

	// Set registers
	for (i=0; i<8; i++)
		regs.d[i] = r->d[i];
	for (i=0; i<7; i++)
		regs.a[i] = r->a[i];

	// Push EXEC_RETURN and faked return address (points to EXEC_RETURN) on stack
	PUSH16( M68K_EXEC_RETURN );
	PUSH32( regs.a[7] + 4 );

	// Execute routine
	JUMP( addr );
	quit_program = false;
	m68k_execute();

	// Clean up stack
	regs.a[7] += 2;

	// Restore old PC
	JUMP( oldpc );

	// Get registers
	for (i=0; i<8; i++)
		r->d[i] = regs.d[i];
	for (i=0; i<7; i++)
		r->a[i] = regs.a[i];
	quit_program = false;
}
