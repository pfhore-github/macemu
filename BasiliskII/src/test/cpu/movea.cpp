#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(movea_word, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[2] = (-1024 & 0xffff);
	test_cpu( { 0031102 } );
	BOOST_TEST( cpu.R[8+1] == -1024 );
}

BOOST_DATA_TEST_CASE( from_an, ea_all, ea ) {
	test_ea_w(cpu, { 0031100 }, 0x2000, ea, 0x2000 );
	BOOST_TEST( cpu.R[8+1] == 0x2000 );
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(movea_long, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[1] = 0x24242424;
	test_cpu( { 0022101 } );
	BOOST_TEST( cpu.R[8+2] == ( 0x24242424 ));
}

BOOST_DATA_TEST_CASE( from_an, ea_all, ea ) {
	test_ea_l(cpu, { 0021100 }, 0x2000, ea, 0x2000 );
	BOOST_TEST( cpu.R[8+1] == 0x2000 );
}


BOOST_AUTO_TEST_SUITE_END();
