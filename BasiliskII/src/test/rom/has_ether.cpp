#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
using namespace ROM;
void prepare(MB_TYPE m) {
	fixture f(m);
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	DR(7) = 1 << ( INIT_FLAG_T::FAULT_SKIP + 16 );
}
BOOST_AUTO_TEST_CASE( with)  {
	prepare(MB_TYPE::MCU);
	AR(1) = 0x50F0A000;
	TEST_ROM( 047B0 );
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( without )  {
	prepare(MB_TYPE::GLU);
	AR(1) = 0x50F18000;
	TEST_ROM( 047B0 );
	BOOST_CHECK( ! cpu.Z );
}
