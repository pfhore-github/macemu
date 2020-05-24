#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "init.h"
#include "registers.hpp"
#include "exceptions.hpp"
#include <vector>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(nop, fixture);
BOOST_AUTO_TEST_CASE(pass) {
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0047161 } ) );
}

BOOST_AUTO_TEST_SUITE_END();
