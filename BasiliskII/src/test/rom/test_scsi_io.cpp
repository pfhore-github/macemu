#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "prototype.hpp"
#include "data.hpp"
#include "machine.hpp"
using namespace ROM;
void prepare() {
	fixture f;
	set_sr( 0x2700 );
	machine->rom_mirror = false;
	cpu.VBR = 0x40803DD8;
	DR(7) = 1 << ( INIT_FLAG_T::FAULT_SKIP + 16 );
}

BOOST_AUTO_TEST_CASE( success)  {
	prepare();
	AR(2) = 0x50F10000;
	TEST_ROM( 04700 );
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( failure)  {
	prepare();
	AR(2) = 0x5000A000;
	TEST_ROM( 04700 );
	BOOST_CHECK( ! cpu.Z );
}
