#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;



BOOST_FIXTURE_TEST_SUITE(movep, fixture);
BOOST_AUTO_TEST_CASE(word_reg_to_mem) {
	cpu.R[4] = 0x1234;
	cpu.R[8+3] = 0x1000;
	test_cpu( { 0004613, 0x20 } );
	BOOST_TEST( test_read8(0x1020) == 0x12 );
	BOOST_TEST( test_read8(0x1022) == 0x34 );
}

BOOST_AUTO_TEST_CASE(long_reg_to_mem) {
	cpu.R[4] = 0x12345678;
	cpu.R[8+3] = 0x1000;
	test_cpu( { 0004713, 0x20 } );
	BOOST_TEST( test_read8(0x1020) == 0x12 );
	BOOST_TEST( test_read8(0x1022) == 0x34 );
	BOOST_TEST( test_read8(0x1024) == 0x56 );
	BOOST_TEST( test_read8(0x1026) == 0x78 );
}

BOOST_AUTO_TEST_CASE(word_mem_to_reg) {
	test_write8(0x1020, 0x12);
	test_write8(0x1022, 0x34);
	cpu.R[8+3] = 0x1000;
	test_cpu( { 0004413, 0x20 } );
	BOOST_TEST( cpu.R[4] == 0x1234 );
}

BOOST_AUTO_TEST_CASE(long_mem_to_reg) {
	test_write8(0x1020, 0x12);
	test_write8(0x1022, 0x34);
	test_write8(0x1024, 0x56);
	test_write8(0x1026, 0x78);
	cpu.R[8+3] = 0x1000;
	test_cpu( { 0004513, 0x20 } );
	BOOST_TEST( cpu.R[4] == 0x12345678 );
}

BOOST_AUTO_TEST_SUITE_END();
