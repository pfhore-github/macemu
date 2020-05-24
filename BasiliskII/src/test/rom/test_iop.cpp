#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
using namespace ROM;
void prepare(MB_TYPE m) {
	fixture f(m);
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	DR(7) = 1 << ( INIT_FLAG_T::FAULT_SKIP + 16 );
}

BOOST_AUTO_TEST_CASE( success)  {
	prepare(MB_TYPE::MCU);
	AR(2) = 0x50F0C020;
	TEST_ROM( 0477C );
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail)  {
	prepare(MB_TYPE::GLU);
	AR(2) = 0x50F10013;
	TEST_ROM( 0477C );
	BOOST_CHECK( ! cpu.Z );
}


