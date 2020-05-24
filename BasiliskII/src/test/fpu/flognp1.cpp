#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>


BOOST_FIXTURE_TEST_SUITE(flognp1, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 2.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x6 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == log1p(2.2) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[2] = -2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x6 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(dz) {
	FPU_R.FP[2] = -1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x6 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_DZ );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_DZ );
}

BOOST_AUTO_TEST_SUITE_END();
