#define BOOST_TEST_DYN_LINK

#include "test_common.hpp"
#include <fenv.h>



BOOST_FIXTURE_TEST_SUITE(fbcc_w, fixture);
BOOST_AUTO_TEST_CASE(t) {
	test_cpu( { 0171200, 0x200 } );
	BOOST_TEST( cpu.PC = 0x204 );
}

BOOST_AUTO_TEST_CASE(f) {
	test_cpu( { 0171220, 0x200 } );
	BOOST_TEST( cpu.PC = 0x4 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fbcc_l, fixture);
BOOST_AUTO_TEST_CASE(t) {
	test_cpu( { 0171300, 0, 0x200 } );
	BOOST_TEST( cpu.PC = 0x206 );
}

BOOST_AUTO_TEST_CASE(f) {
	test_cpu( { 0171320, 0, 0x200 } );
	BOOST_TEST( cpu.PC = 0x6 );
}

BOOST_AUTO_TEST_SUITE_END();
