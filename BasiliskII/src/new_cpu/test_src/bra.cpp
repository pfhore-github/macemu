#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "init.h"
#include "../registers.hpp"
#include "../exceptions.hpp"
#include <vector>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
extern CPU xcpu;
void test_cpu(const std::vector<uint16_t>& c);
BOOST_TEST_GLOBAL_FIXTURE(init);
BOOST_FIXTURE_TEST_SUITE(bra, fixture);
BOOST_AUTO_TEST_CASE( d8_for )  {
	test_cpu( { (uint16_t)(0x6000 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE( d8_back ) {
	test_cpu( { (uint16_t)(0x6000 | (uint8_t)(-20) ) } );
	BOOST_TEST( xcpu.PC == -18 );
}

BOOST_AUTO_TEST_CASE( d16_for )  {
	test_cpu( { 0x6000,  2000 } );
	BOOST_TEST( xcpu.PC == 2002 );
}

BOOST_AUTO_TEST_CASE( d16_back ) {
	test_cpu( { 0x6000, (uint16_t)-2000 } );
	BOOST_TEST( xcpu.PC == -1998 );
}

BOOST_AUTO_TEST_CASE( d32 )  {
	test_cpu( { 0x60ff,  0x5, 0 } );
	BOOST_TEST( xcpu.PC == (0x50000 + 2) );
}


BOOST_AUTO_TEST_SUITE_END();
