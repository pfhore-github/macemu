#include <stdint.h>
#include "sysdeps.h"
#include "main.h"
#include "registers.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "prototype.hpp"
#include "inst_cmn.hpp"
void emul_reset(uint32_t* d, uint32_t* a);
namespace ROM {
// $8c
void boot() {
	// T=0, S=true, M=false, INTMASK=7, X=N=Z=V=C=false
	set_sr( 0x2700 );
	load_sp();
	init_mmu();					// $04052

	emul_reset(cpu.R, cpu.R+8);	

	init_hw();					// $02E00

	init_powerbook();			// $8AC00
	
	return run_rom(0x008E0);
}
}
namespace ROMWrapper {
void run_0008C() {
	ROM::boot();
}
}
