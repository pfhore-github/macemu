#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(chk_word, fixture);
BOOST_AUTO_TEST_CASE(pass) {
	cpu.R[0] = 10;
	cpu.R[1] = 3;
	test_cpu( { 0041600 } );
	test_in_exception( 0 );
}
BOOST_AUTO_TEST_CASE(negative) {
	cpu.R[0] = 10;
	cpu.R[1] = -2;
	test_cpu( { 0041600 } );
	BOOST_TEST( cpu.N == true );
	test_in_exception( 6 );
}

BOOST_AUTO_TEST_CASE(over) {
	cpu.R[0] = 10;
	cpu.R[1] = 12;
	test_cpu( { 0041600 } );
	BOOST_TEST( cpu.N == false );
	test_in_exception( 6 );
}

BOOST_DATA_TEST_CASE(ea_test, ea_data, ea) {
	cpu.R[0] = 400;
	test_ea_w(cpu, { 0040600 }, 1000, ea, 1000 );
	test_in_exception( 0 );
}


BOOST_AUTO_TEST_SUITE_END();


BOOST_FIXTURE_TEST_SUITE(chk_long, fixture);
BOOST_AUTO_TEST_CASE(pass) {
	cpu.R[0] = 100000;
	cpu.R[1] = 3000;
	test_cpu( { 0041400 } );
	test_in_exception( 0 );
}
BOOST_AUTO_TEST_CASE(negative) {
	cpu.R[0] = 100000;
	cpu.R[1] = -20000;
	test_cpu( { 0041400 } );
	BOOST_TEST( cpu.N == true );
	test_in_exception( 6 );
}

BOOST_AUTO_TEST_CASE(over) {
	cpu.R[0] = 100000;
	cpu.R[1] = 120000;
	test_cpu( { 0041400 } );
	BOOST_TEST( cpu.N == false );
	test_in_exception( 6 );
}

BOOST_DATA_TEST_CASE(ea_test, ea_data, ea) {
	cpu.R[0] = 40000;
	test_ea_l(cpu, { 0040400 }, 100000, ea, 100000 );
}


BOOST_AUTO_TEST_SUITE_END();
