/*
 *  basilisk_glue.cpp - Glue new CPU to Basilisk II CPU engine interface
 * *
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
#include "adb.h"
#include "registers.hpp"
#include "fpu/68881.hpp"
#include "../rom/data.hpp"
#include <SDL_thread.h>
#include <SDL_events.h>
// RAM and ROM pointers
uint32 RAMBaseMac = 0;		// RAM base (Mac address space) gb-- initializer is important
uint8 *RAMBaseHost;			// RAM base (host address space)
uint32 RAMSize;				// Size of RAM
uint32 ROMBaseMac;			// ROM base (Mac address space)
uint8 *ROMBaseHost;			// ROM base (host address space)
uint32 ROMSize;				// Size of ROM

// Mac frame buffer
uint8 *MacFrameBaseHost;	// Frame buffer base (host address space)
uint32 MacFrameSize;		// Size of frame buffer
int MacFrameLayout;			// Frame buffer layout
uintptr MEMBaseDiff;
CPU cpu;
static NoFPU no_fpu;
FPU* fpu = &no_fpu;
MMU* mmu;
#if USE_JIT
bool UseJIT = false;
#endif
// From newcpu.cpp
extern bool quit_program;
void init_rom_routines();
bool is_quadra = true;


/*
 *  Initialize 680x0 emulation, CheckROM() must have been called first
 */

bool Init680x0(void)
{
	// Mac address space = host address space minus constant offset (MEMBaseDiff)
	// NOTE: MEMBaseDiff is set up in main_unix.cpp/main()

	cpu.init();	
	mmu =new MC68040_MMU();
	fpu = new M68040_FPU();
	cpu.reset();
	// ROM to RAM
	memcpy(RAMBaseHost, ROMBaseHost, ROMSize);
	init_rom_routines();

#if USE_JIT
//	UseJIT = compiler_use_jit();
//	if (UseJIT)
//	    compiler_init();
#endif
	return true;
}

static std::atomic<bool> emul_done = false;
/*
 *  Deinitialize 680x0 emulation
 */

void Exit680x0(void)
{
#if USE_JIT
//    if (UseJIT)
//		compiler_exit();
#endif
	emul_done = true;
}


/*
 *  Initialize memory mapping of frame buffer (called upon video mode change)
 */

void InitFrameBufferMapping(void)
{
}
void m68k_execute();
// CPU thread
static int run_cpu(void*) {
	m68k_execute();
	return 0;
}
void handle_adb(SDL_Event* e);
/*
 *  Reset and start 680x0 emulation (doesn't return)
 */
void Start680x0(void)
{
	
	cpu.init();
	cpu.reset();
#if USE_JIT
//    if (UseJIT)
//	m68k_compile_execute();
//    else
#endif
	SDL_Thread* th = SDL_CreateThread(run_cpu, "CPU", nullptr);
	SDL_DetachThread(th);
	// input handling
	for(;;) {
		if( emul_done )
			return;
		// input handling
		SDL_Event e;
		while( SDL_PollEvent(&e) ) {
			handle_adb(&e);
		}
		SDL_Delay(0);
	}
}
void m68k_execute() {
	cpu.run = true;
	while(! quit_program) {
		if( cpu.run )
			cpu.do_op();
		else
			pthread_yield();
		
	}
}

/*
 *  Trigger interrupt
 */
void TriggerInterrupt(void)
{
	idle_resume();
	cpu.irq(1);
}

void TriggerNMI(void)
{
	cpu.irq(7);
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

	// Save old PC
	uint32_t oldpc = cpu.PC;

	// Set registers
	memcpy(&cpu.R[0], r, sizeof(uint32_t)*15);

	// Push trap and EXEC_RETURN on stack
	push16(M68K_EXEC_RETURN);
	push16(trap);

	// Execute trap
	cpu.PC = cpu.R[15];
	quit_program = false;
	m68k_execute();

	// Clean up stack
	cpu.R[15] += 4;

	// Restore old PC
	cpu.PC = oldpc;

	// Get registers
	memcpy(r, &cpu.R[0], sizeof(uint32_t)*15);
	quit_program = false;
}


/*
 *  Execute 68k subroutine
 *  The executed routine must reside in UAE memory!
 *  r->a[7] and r->sr are unused!
 */

void Execute68k(uint32 addr, struct M68kRegisters *r)
{

	// Save old PC
	uaecptr oldpc = cpu.PC;

	// Set registers
	memcpy(&cpu.R[0], r, sizeof(uint32_t)*15);

	// EXEC_RETURN to $0
	uint32_t oldlr = cpu.R[14];
	cpu.R[14] = 0x40000000;
	write_w(0x40000000, M68K_EXEC_RETURN);

	// Execute routine
	cpu.PC = addr;
	quit_program = false;
	m68k_execute();

	// Clean up stack
	cpu.R[14] = oldlr;

	// Restore old PC
	cpu.PC = oldpc;

	// Get registers
	memcpy(r, &cpu.R[0], sizeof(uint32_t)*15);
	quit_program = false;
}
void m68k_dumpstate() {
	cpu.dump();
}
