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
BOOST_FIXTURE_TEST_SUITE(callm, fixture);
BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 src) {
	BOOST_CHECK_THROW(
		test_cpu( { (uint16_t)( 0003320 | src )} ),
		IllegalInstruction );
}
BOOST_DATA_TEST_CASE(d16,
					 bdata::xrange(8),
					 src) {
	BOOST_CHECK_THROW(
		test_cpu( { (uint16_t)( 0003350 | src ), 0x100} ),
		IllegalInstruction );
}
BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 src) {
	xcpu.D[0] = 0x10;
	BOOST_CHECK_THROW(
		test_cpu( { (uint16_t)( 0003360 | src ), 0x0830} ),
		IllegalInstruction );
}

BOOST_AUTO_TEST_CASE(xxxw) {
	BOOST_CHECK_THROW(
		test_cpu( { 0003370 , 0x1000} ),
		IllegalInstruction );
}

BOOST_AUTO_TEST_CASE(xxxl) {
	BOOST_CHECK_THROW(
		test_cpu( { 0003371 , 0, 0x1000} ),
		IllegalInstruction );
}

BOOST_AUTO_TEST_CASE(d16pc) {
	BOOST_CHECK_THROW(
		test_cpu( { 0003372, 0x1000} ),
		IllegalInstruction );
}
BOOST_DATA_TEST_CASE(d8pcxn,
					 bdata::xrange(8),
					 src) {
	xcpu.D[0] = 0x10;
	BOOST_CHECK_THROW(
		test_cpu( { 0003373, 0x0830} ),
		IllegalInstruction );
}


BOOST_AUTO_TEST_SUITE_END();
