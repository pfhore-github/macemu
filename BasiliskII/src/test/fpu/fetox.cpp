#define BOOST_TEST_DYN_LINK

#include "test_common.hpp"
#include <math.h>



BOOST_FIXTURE_TEST_SUITE(fetox, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 1.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x10 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == exp(1.0) );
}

BOOST_AUTO_TEST_CASE(ovfl) {
	FPU_R.FP[2] = 1e100;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x10 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_OVFL );
}
BOOST_AUTO_TEST_SUITE_END();
