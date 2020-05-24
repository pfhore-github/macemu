#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(exg, fixture);
BOOST_AUTO_TEST_CASE( dx_dy)  {
	cpu.R[2] = 0x12345678;
	cpu.R[4] = 0xabcdef01;
	test_cpu( { 0142504 } );
	BOOST_TEST( cpu.R[2] == 0xabcdef01 );
	BOOST_TEST( cpu.R[4] == 0x12345678 );
}

BOOST_AUTO_TEST_CASE( ax_ay)  {
	cpu.R[8+2] = 0x12345678;
	cpu.R[8+4] = 0xabcdef01;
	test_cpu( { 0142514 } );
	BOOST_TEST( cpu.R[8+2] == 0xabcdef01 );
	BOOST_TEST( cpu.R[8+4] == 0x12345678 );
}

BOOST_AUTO_TEST_CASE( dx_ay)  {
	cpu.R[2] = 0x12345678;
	cpu.R[8+4] = 0xabcdef01;
	test_cpu( { 0142614 } );
	BOOST_TEST( cpu.R[2] == 0xabcdef01 );
	BOOST_TEST( cpu.R[8+4] == 0x12345678 );
}

BOOST_AUTO_TEST_SUITE_END();
