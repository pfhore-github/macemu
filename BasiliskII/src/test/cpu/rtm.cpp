#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(rtm, fixture);
BOOST_AUTO_TEST_CASE(rtm_d) {
	test_cpu( { 0003302 } );
	test_in_exception(4);
}

BOOST_AUTO_TEST_CASE(rtm_r) {
	test_cpu( { 0003312 } );
	test_in_exception(4);
}

BOOST_AUTO_TEST_SUITE_END();
