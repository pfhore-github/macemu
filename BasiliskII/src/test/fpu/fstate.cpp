#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"


BOOST_FIXTURE_TEST_SUITE(fsave, fixture);
BOOST_AUTO_TEST_CASE(exec) {
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0171422 } );
	BOOST_TEST( test_read32(0x1000) == 0x00410000 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(frestore, fixture);
BOOST_AUTO_TEST_CASE(exe) {
	test_write32(0x1000, 0x00410000 );
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0171522 } );
}

BOOST_AUTO_TEST_SUITE_END();
