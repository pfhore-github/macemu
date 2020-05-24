#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(bkpt, fixture);
BOOST_AUTO_TEST_CASE(exec) {
	test_cpu( { 0044110 } );
	test_in_exception(4);
}
BOOST_AUTO_TEST_SUITE_END();
