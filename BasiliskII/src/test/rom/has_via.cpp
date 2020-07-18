#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "via.hpp"
#include "asc.hpp"
#include "glu.hpp"
#include "v8.hpp"
namespace bdata = boost::unit_test::data;
using namespace ROM;
void prepare(bool is_glu) {
	if( is_glu ) {
		fixture f(std::make_unique<GLU>());
	} else {
		fixture f(std::make_unique<V8>());
	}
	set_sr( 0x2700 );
	cpu.VBR = 0x40803DD8;
	DR(7) = 1 << ( INIT_FLAG_T::FAULT_SKIP + 16 );
}
BOOST_AUTO_TEST_CASE( success_nooffset )  {
	prepare(true);
	AR(2) = 0x50F01C00;
	TEST_ROM( 046AA );
	BOOST_CHECK( cpu.Z );
}
BOOST_AUTO_TEST_CASE( fail_nooffset )  {
	prepare(true);
	AR(2) = 0x50009c00;
	TEST_ROM( 046AA );
	BOOST_CHECK( ! cpu.Z );
}


BOOST_AUTO_TEST_CASE( success_offset )  {
	prepare(true);
	AR(2) = 0x50F01c00;
	DR(2) = 0x20000;
	TEST_ROM( 046AC );
	BOOST_CHECK( cpu.Z );
}

BOOST_AUTO_TEST_CASE( fail_offset )  {
	prepare(false);
	AR(2) = 0x50F00000;
	DR(2) = 0x20000;
	TEST_ROM( 046AC );
	BOOST_CHECK( ! cpu.Z );
}

