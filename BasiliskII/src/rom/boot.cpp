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

	goto_boot2();				// $008E0
	return;
}
// $4052
void init_mmu() {
	if( dynamic_cast<MC68040_MMU*>(mmu) ) {
		// 68040
		mmu->exec(3, 3, 0); 	// cinva bc
		mmu->movec_to(2, 0);	// CACR
		mmu->movec_to(3, 0);	// TC
		mmu->movec_to(6, 0x807FC040); // DTT0; high address; no-cache
		mmu->movec_to(7, 0x500FC040); // DTT1; IO address;no-cache
	} else if( auto mmup = dynamic_cast<MC68030_MMU*>(mmu) ) {
		// 68030
		mmup->pmove(0x40A4, 0); // TC
	}
}

}
namespace ROMWrapper {
void run_0008C() {
	ROM::boot();
}

void run_04052() {
	ROM::init_mmu();
	cpu.PC = AR(6);
}

}
