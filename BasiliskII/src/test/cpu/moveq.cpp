#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(moveq, fixture);
BOOST_AUTO_TEST_CASE(exec) {
	cpu.V = cpu.C = true;
	test_cpu( { 0074000 | 0x22 } );
	BOOST_TEST( cpu.R[4] == 0x22 );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n, bdata::xrange(2), n) {
	test_cpu( { (uint16_t)(0074000 | (n << 7)) } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z, bdata::xrange(2), z) {
	test_cpu( { (uint16_t)(0074000 | (!z)) } );
	BOOST_TEST( cpu.Z == z );
}
BOOST_AUTO_TEST_SUITE_END();
