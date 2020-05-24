#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(link_, fixture);
BOOST_AUTO_TEST_CASE(word)
{
	cpu.R[8+2] = 0x3000;
	cpu.R[8+7] = 0x2004;
	test_cpu( { 0047122, 0x30 } );
	BOOST_TEST( test_read32( 0x2000) ==  0x3000 );
	BOOST_TEST( cpu.R[8+2] == 0x2000 );
	BOOST_TEST( cpu.R[8+7] == 0x2030 );
}

BOOST_AUTO_TEST_CASE(long_)
{
	cpu.R[8+2] = 0x3000;
	cpu.R[8+7] = 0x2004;
	test_cpu( { 0044012, 0, 0x30 } );
	BOOST_TEST( test_read32( 0x2000) ==  0x3000 );
	BOOST_TEST( cpu.R[8+2] == 0x2000 );
	BOOST_TEST( cpu.R[8+7] == 0x2030 );
}

BOOST_AUTO_TEST_SUITE_END();
