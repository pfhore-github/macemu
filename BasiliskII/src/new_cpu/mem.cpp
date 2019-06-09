#include "registers.hpp"
#include "memory.hpp"
extern uint8_t *RAMBaseHost;
extern uint8_t *ROMBaseHost;
void* to_ra(uint32_t va, int sz, bool rw) {
	(void)sz;
	(void)rw;
	return &RAMBaseHost[va];		
}
