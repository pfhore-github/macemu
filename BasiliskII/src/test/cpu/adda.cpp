#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(adda_word, fixture);
BOOST_DATA_TEST_CASE( result,
					  bdata::xrange(2),
					  sign )  {
	cpu.R[8+0] = 0x200000;
	cpu.R[0] = (sign ? -1 : 1 ) * 0x5000;
	test_cpu( { 0150300 } );
	BOOST_TEST( cpu.R[8+0] == ( sign ? 0x200000 -0x5000 : 0x200000 + 0x5000 ) );
}

BOOST_DATA_TEST_CASE(from_ea, ea_all, ea)
{
	cpu.R[8+1] = 0x300000;
	test_ea_w(cpu, { 0151300 }, 0x2000, ea, 0x2000 );
	BOOST_TEST( cpu.R[8+1] == 0x302000 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(adda_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[8+0] = 0x20000000;
	cpu.R[0] = 0x5000;
	test_cpu( { 0150700 } );
	BOOST_TEST( cpu.R[8+0] == 0x20005000 );
}

BOOST_DATA_TEST_CASE(from_ea, ea_all, ea)
{
	cpu.R[8+1] = 0x3000000;
	test_ea_l(cpu, { 0151700 }, 0x2000000, ea, 0x2000000 );
	BOOST_TEST( cpu.R[8+1] == 0x5000000 );
}

BOOST_AUTO_TEST_SUITE_END();
