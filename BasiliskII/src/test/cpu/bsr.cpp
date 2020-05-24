#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(bsr, fixture);
BOOST_AUTO_TEST_CASE( d8_for )  {
	cpu.R[8+7] = 0x2000;
	test_cpu( { (uint16_t)(0x6100 | 20) } );
	BOOST_TEST( cpu.PC == 22 );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 2 );
}

BOOST_AUTO_TEST_CASE( d8_back ) {
	cpu.R[8+7] = 0x2000;
	test_cpu( { (uint16_t)(0x6100 | (uint8_t)(-20) ) } );
	BOOST_TEST( cpu.PC == -18 );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 2 );
}

BOOST_AUTO_TEST_CASE( d16_for )  {
	cpu.R[8+7] = 0x2000;
	test_cpu( { 0x6100,  2000 } );
	BOOST_TEST( cpu.PC == 2002 );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 4 );
}

BOOST_AUTO_TEST_CASE( d16_back ) {
	cpu.R[8+7] = 0x2000;
	test_cpu( { 0x6100, (uint16_t)-2000 } );
	BOOST_TEST( cpu.PC == -1998 );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 4 );
}

BOOST_AUTO_TEST_CASE( d32 )  {
	cpu.R[8+7] = 0x2000;
	test_cpu( { 0x61ff,  0x5, 0 } );
	BOOST_TEST( cpu.PC == (0x50000 + 2) );
	BOOST_TEST( cpu.R[8+7] == 0x1ffc );
	BOOST_TEST( test_read32(0x1ffc) == 6 );
}


BOOST_AUTO_TEST_SUITE_END();
