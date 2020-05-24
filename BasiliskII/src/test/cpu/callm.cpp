#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(callm, fixture);
BOOST_DATA_TEST_CASE(ea_test, ea_control, ea) {
	test_ea_b(cpu, { 0003300 }, 0, ea, 0 );
	test_in_exception(4);
}

BOOST_AUTO_TEST_SUITE_END();
