#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(rts, fixture);
BOOST_AUTO_TEST_CASE( exec)  {
	cpu.R[8+7] = 0x2000;
	test_write32(0x2000, 0x1000);
	test_cpu( { 0047165 } );
	BOOST_TEST( cpu.PC == 0x1000 );
}
BOOST_AUTO_TEST_SUITE_END();
