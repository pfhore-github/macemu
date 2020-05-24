#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>
BOOST_FIXTURE_TEST_SUITE(facos, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 0.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1C ) });
	BOOST_TEST( (double)FPU_R.FP[3] == acos(0.2), boost::test_tools::tolerance(1e-5)  );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[2] = 1.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1C ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_SUITE_END();
