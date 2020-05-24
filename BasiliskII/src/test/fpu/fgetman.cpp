#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>


BOOST_FIXTURE_TEST_SUITE(fgetman, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 3.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1F ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 1.5, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[2] = INFINITY;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1F ) });
	BOOST_TEST( isnan(FPU_R.FP[3]));
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(zero) {
	FPU_R.FP[2] = 0.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1F ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 0.0, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_SUITE_END();
