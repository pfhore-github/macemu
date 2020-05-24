#include "data.hpp"
#include "prototype.hpp"
namespace ROM {
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
void run_04052() {
	ROM::init_mmu();
	cpu.PC = AR(6);
}
}
