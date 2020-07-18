#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "SDL.h"
#include <stdio.h>
uint8_t *RAMBaseHost;
uint8_t *ROMBaseHost;
uint32_t ROMSize = 1024*1024; // 1MB
uint32_t RAMSize;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "machine.hpp"
#include "asc.hpp"
#include <unistd.h>
#include <stdlib.h>
FPU* fpu;
MMU* mmu;
bool rom;
bool dump_cpu;
using boost::unit_test::framework::master_test_suite;
uint8_t* rom2;
#define ROM_PATH "/home/michiaki/Document/macemu/macemu/BasiliskII/src/quadra630.rom"
#define ROM_PATH2 "/home/michiaki/Document/macemu/macemu/BasiliskII/src/quadra650.rom"
// initialization function:
bool init_unit_test()
{
	RAMSize = 1024*1024*4; // 4MB
	RAMBaseHost = new uint8_t[RAMSize];
	SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO);
	fpu = new M68040_FPU();
	mmu = new MC68040_MMU();
	cpu.init();
	int romp = open(ROM_PATH, O_RDONLY);
	if( romp == -1 )
		return false;
	int romp2 = open(ROM_PATH2, O_RDONLY);
	if( romp2 == -1 )
		return false;
	ROMBaseHost = (uint8_t*)mmap(nullptr, ROMSize, PROT_READ, MAP_SHARED, romp, 0);
	rom2 = (uint8_t*)mmap(nullptr, ROMSize, PROT_READ, MAP_SHARED, romp2, 0);
//	close(romp);
//	close(romp2);
	for(int i = 1; i < master_test_suite().argc; ++ i ) {
		const char* arg = master_test_suite().argv[i];
		if( strcmp(arg, "--dump") == 0 ) {
			dump_cpu = true;
			rom = true;
		} else if( strcmp(arg, "--rom") == 0 ) {
			rom = true;
		} else if( strcmp(arg, "--native") == 0 ) {
			rom = false;
		}
	}
	return true;
}

// entry point:
int main(int argc, char* argv[])
{
	atexit(	SDL_Quit );
	int r = boost::unit_test::unit_test_main( &init_unit_test, argc, argv );
	return r;
}
