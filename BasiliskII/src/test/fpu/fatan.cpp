#define BOOST_TEST_DYN_LINK

#include "test_common.hpp"
#include <math.h>

BOOST_FIXTURE_TEST_SUITE(fatan, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 0.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0xA ) });
	BOOST_TEST( (double)FPU_R.FP[3] == atan(0.2), boost::test_tools::tolerance(1e-5)  );
}

BOOST_AUTO_TEST_SUITE_END();
