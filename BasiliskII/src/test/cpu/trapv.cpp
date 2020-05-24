#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(trapv, fixture);
BOOST_AUTO_TEST_CASE(yes) {
	cpu.V = true;
	test_cpu( { 0047166 });
	test_in_exception(7);
}
BOOST_AUTO_TEST_CASE(no) {
	cpu.V = false;
	test_cpu( { 0047166 } );
	test_in_exception(0);
}
BOOST_AUTO_TEST_SUITE_END();
