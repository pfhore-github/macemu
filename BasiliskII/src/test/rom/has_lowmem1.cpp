#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
using namespace ROM;
void prepare() {
	fixture f;
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	DR(7) = 1 << ( INIT_FLAG_T::FAULT_SKIP + 16);
}

BOOST_AUTO_TEST_CASE( success)  {
	prepare();
	AR(1) = 0x10;
	write_l( 0x14, 0x726f6d70 );
	TEST_ROM( 047BA );
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail)  {
	prepare();
	AR(1) = 0x10;
	write_l( 0x14, 0x1f1f1f1f );
	TEST_ROM( 047BA );
	BOOST_CHECK( ! cpu.Z );
}


