#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(trapcc, fixture);
BOOST_AUTO_TEST_CASE( no_word )  {
	test_cpu( { 0x51FC } );
	test_in_exception(0);
	BOOST_TEST( cpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE( word1 )  {
	test_cpu( {  0x51FA, 1 } );
	test_in_exception(0);
	BOOST_TEST( cpu.PC == 4 );
}

BOOST_AUTO_TEST_CASE( word2 )  {
	test_cpu( {  0x51FB, 1, 1 } );
	test_in_exception(0);
	BOOST_TEST( cpu.PC == 6 );
}



BOOST_DATA_TEST_CASE(t, t_data, cond) {
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	test_cpu( { (uint16_t)(0x50FC | (cond.cc << 8) ) });
	test_in_exception(7);
}

BOOST_DATA_TEST_CASE(f, f_data, cond) {
	cpu.C = cond.C;
	cpu.Z = cond.Z;
	cpu.V = cond.V;
	cpu.N = cond.N;
	test_cpu( { (uint16_t)(0x50FC | (cond.cc << 8) ) });
	test_in_exception(0);
}



BOOST_AUTO_TEST_SUITE_END();
