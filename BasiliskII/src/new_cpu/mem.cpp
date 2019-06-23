#include "registers.hpp"
#include "mmu.hpp"
#include "exceptions.hpp"
extern uint8_t *RAMBaseHost;
extern uint8_t *ROMBaseHost;
void* NonMMU::to_real(uint32_t va, int sz, bool rw, int mode) {
	if( sz != 1 && va & 1 ) {
		throw AddressError(va-1);
	}
	(void)sz;
	(void)rw;
	(int)mode;
	return &RAMBaseHost[va];		
}
