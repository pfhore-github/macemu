#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(trap, fixture);
BOOST_DATA_TEST_CASE(occur, bdata::xrange(16), n) {
	test_cpu( { (uint16_t)(0047100 | n) } );
	test_in_exception(32+n);
}


BOOST_AUTO_TEST_SUITE_END();
