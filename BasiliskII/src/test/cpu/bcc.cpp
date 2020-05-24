#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(bcc, fixture);
BOOST_AUTO_TEST_CASE( d8_for )  {
	cpu.Z = true;
	test_cpu( { (uint16_t)(0x6700 | 20) } );
	BOOST_TEST( cpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE( d8_back ) {
	cpu.Z = true;
	test_cpu( { (uint16_t)(0x6700 | (uint8_t)(-20) ) } );
	BOOST_TEST( cpu.PC == -18 );
}

BOOST_AUTO_TEST_CASE( d16_for )  {
	cpu.Z = true;
	test_cpu( { 0x6700,  2000 } );
	BOOST_TEST( cpu.PC == 2002 );
}

BOOST_AUTO_TEST_CASE( d16_back ) {
	cpu.Z = true;
	test_cpu( { 0x6700, (uint16_t)-2000 } );
	BOOST_TEST( cpu.PC == -1998 );
}

BOOST_AUTO_TEST_CASE( d32 )  {
	cpu.Z = true;
	test_cpu( { 0x67ff,  0x5, 0 } );
	BOOST_TEST( cpu.PC == (0x50000 + 2) );
}

BOOST_DATA_TEST_CASE(jump, t_data, cond) {
	if( cond.cc < 2 )
		return;
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	test_cpu( { (uint16_t)(0x6000 | (cond.cc << 8) | 20) } );
	BOOST_TEST( cpu.PC == 22 );
}

BOOST_DATA_TEST_CASE(non_jump, f_data, cond) {
	if( cond.cc < 2 )
		return;
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	test_cpu( { (uint16_t)(0x6000 | (cond.cc << 8) | 20) } );
	BOOST_TEST( cpu.PC == 2 );
}



BOOST_AUTO_TEST_SUITE_END();
