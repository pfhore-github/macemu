#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "machine.hpp"
#include "asc.hpp"
using namespace ROM;
#include <time.h>
namespace ROM {
void nanosleep_x(double ns) {
	uint64_t left = ns * SDL_GetPerformanceFrequency() / 1000000000;
	if( left > 10000000 ) {
		SDL_Delay(left/10000000*10);
		left %= 1000000;
	}
	uint64_t deadline = SDL_GetPerformanceCounter() + left;
	while( SDL_GetPerformanceCounter() <= deadline ) {
		__builtin_ia32_pause();
	}
}
}
const uint32_t addr[] = {
	0x40807158,
	0x40807178,
	0x40807198,
	0x408071ac,
	0x408071c4
};
BOOST_DATA_TEST_CASE( ring, addr, a ) {
	fixture f;
	AR(5) = 0x50f00000;
	AR(4) = a;	
	AR(3) = 0x50F14000;
	TEST_ROM( 0706E );
}
