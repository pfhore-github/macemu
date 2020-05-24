#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(swap, fixture);
BOOST_AUTO_TEST_CASE(result) {
	cpu.R[2] = 0x12345678;
	cpu.V = cpu.C = true;
	test_cpu( { 0044102 } );
	BOOST_TEST( cpu.R[2] == 0x56781234 );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[2] = n ?0xffff : 0xffff0000;
	test_cpu( { 0044102 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[2] = z ? 0 : 1;
	test_cpu( { 0044102 } );
	BOOST_TEST( cpu.Z == z );
}


BOOST_AUTO_TEST_SUITE_END();
