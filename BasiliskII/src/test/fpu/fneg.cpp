#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>

BOOST_FIXTURE_TEST_SUITE(fneg, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 2.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1A ) });
	BOOST_TEST( (double)FPU_R.FP[3] == -2.2, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fsneg, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 2.2f;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x5A ) });
	BOOST_TEST( (float)FPU_R.FP[3] == -2.2f, boost::test_tools::tolerance(1e-3)  );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fdneg, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 2.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x5E ) });
	BOOST_TEST( (double)FPU_R.FP[3] == -2.2, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_SUITE_END();
