#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "prototype.hpp"
#include <unordered_map>
#include "inst_cmn.hpp"
#include "data.hpp"
#include "machine.hpp"
using namespace ROM;
void prepare() {
	fixture f;
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	DR(0) = 1 << (INIT_FLAG_T::FAULT_SKIP + 16);
}

BOOST_AUTO_TEST_CASE( success)  {
	prepare();
	AR(2) = 0x50F10000;
	TEST_ROM( 04708 );
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( failure)  {
	prepare();
	AR(2) = 0x50F00000; 
	TEST_ROM( 04708 );
	BOOST_CHECK( ! cpu.Z );
}
