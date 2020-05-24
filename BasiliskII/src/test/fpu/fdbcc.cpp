#define BOOST_TEST_DYN_LINK

#include "test_common.hpp"

BOOST_FIXTURE_TEST_SUITE(fdbcc, fixture);
BOOST_AUTO_TEST_CASE(t) {
	cpu.R[3] = 0;
	test_cpu( { 0171113, 0, 0x100 } );
	BOOST_TEST( cpu.PC = 0x6 );
	BOOST_TEST( cpu.R[3] == 0 );
}

BOOST_AUTO_TEST_CASE(f1) {
	cpu.R[3] = 3;
	test_cpu( { 0171113, 15, 0x100 } );
	BOOST_TEST( cpu.PC = 0x106 );
	BOOST_TEST( cpu.R[3] == 2 );
}

BOOST_AUTO_TEST_CASE(f2) {
	cpu.R[3] = 0;
	test_cpu( { 0171113, 15, 0x100 } );
	BOOST_TEST( cpu.PC = 6 );
	BOOST_TEST( cpu.R[3] == -1 );
}



BOOST_AUTO_TEST_SUITE_END();
