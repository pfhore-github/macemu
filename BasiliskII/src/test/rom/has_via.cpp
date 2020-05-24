#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "via.hpp"
#include "asc.hpp"
namespace bdata = boost::unit_test::data;
using namespace ROM;
void prepare(MB_TYPE m) {
	fixture f(m);
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	DR(7) = 1 << ( INIT_FLAG_T::FAULT_SKIP + 16 );
}
BOOST_AUTO_TEST_CASE( success_nooffset )  {
	prepare(MB_TYPE::GLU);
	AR(2) = 0x50F01C00;
	TEST_ROM( 046AA );
	BOOST_CHECK( cpu.Z );
}
BOOST_AUTO_TEST_CASE( fail_nooffset )  {
	prepare(MB_TYPE::GLU);
	AR(2) = 0x50009c00;
	TEST_ROM( 046AA );
	BOOST_CHECK( ! cpu.Z );
}


BOOST_AUTO_TEST_CASE( success_offset )  {
	prepare(MB_TYPE::GLU);
	AR(2) = 0x50F01c00;
	DR(2) = 0x20000;
	TEST_ROM( 046AC );
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail_offset )  {
	prepare(MB_TYPE::V8);
	AR(2) = 0x50F00000;
	DR(2) = 0x20000;
	TEST_ROM( 046AC );
	BOOST_CHECK( ! cpu.Z );
}

