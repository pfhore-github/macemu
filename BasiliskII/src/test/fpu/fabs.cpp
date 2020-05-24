#define BOOST_TEST_DYN_LINK

#include "test_common.hpp"
#include <math.h>

namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(fabs_, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2), sign) {
	FPU_R.FP[2] = sign ? -3.0 : 3.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x18 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 3.0, boost::test_tools::tolerance(1e-5) );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fsabs, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2), sign) {
	FPU_R.FP[2] = sign ? -3.0 : 3.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x58 ) });
	BOOST_TEST( (float)FPU_R.FP[3] == 3.0f, boost::test_tools::tolerance(1e-5) );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fdabs, fixture);
BOOST_DATA_TEST_CASE(result, bdata::xrange(2), sign) {
	FPU_R.FP[2] = sign ? -3.0 : 3.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x5C ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 3.0, boost::test_tools::tolerance(1e-5) );
}


BOOST_AUTO_TEST_SUITE_END();
