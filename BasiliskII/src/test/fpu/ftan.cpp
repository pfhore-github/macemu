#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>

BOOST_FIXTURE_TEST_SUITE(ftan, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 0.1;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0xF ) });
	BOOST_TEST( (double)FPU_R.FP[3] == tan(0.1) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[2] = INFINITY;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0xF ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}
#if 0
BOOST_AUTO_TEST_CASE(ovfl) {
	FPU_R.FP[2] = M_PI_2l;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0xF ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_OVFL );
}
#endif
BOOST_AUTO_TEST_SUITE_END();
