#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "registers.hpp"
#include "test_common.hpp"
#include "data.hpp"
#include <fenv.h>
#include "main.h"
extern bool dump_cpu;
extern bool dump_;

void test_rom(uint32_t begin, uint32_t end) {
	dump_ = dump_cpu;
	set_sr( 0x2700 | ( get_sr() & 0xff ) );

	cpu.PC = begin;
	if( dump_cpu ) {
		printf("--------------\n");
	}
	while( cpu.PC != end ) {
		cpu.do_op();
		auto stub = ROM::rom_routines.find(cpu.PC & 0xfffff);
		if(stub != ROM::rom_routines.end() ) {
			(stub->second)( );
		}
	}
}
void clear_global() {
	using namespace ROM;
	model = nullptr;
	motherboard = nullptr;
	INIT_HW_FLG = 0;
	ROM_FLG = 0;
	HWCFG_FLAGS = 0;
	SET_D2( 0 );
	INIT_FLAGS = 0;
	EXCEPTION_STATUS = 0;
}
