#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>


BOOST_FIXTURE_TEST_SUITE(frem_, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[3] = 3.0;
	FPU_R.FP[2] = 2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x25 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == -1.0, boost::test_tools::tolerance(1e-5) );
	BOOST_TEST( FPU_R.FPSR_quotient == 2 );
}

BOOST_AUTO_TEST_CASE(operr1) {
	FPU_R.FP[3] = 2.0;
	FPU_R.FP[2] = 0.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x25 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(operr2) {
	FPU_R.FP[3] = INFINITY;
	FPU_R.FP[2] = 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x25 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}


BOOST_AUTO_TEST_SUITE_END();
