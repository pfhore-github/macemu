#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>


BOOST_FIXTURE_TEST_SUITE(fsqrt, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 3.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x4 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == sqrt(3.0) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[2] = -2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x4 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fssqrt, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 3.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x41 ) });
	BOOST_TEST( (float)FPU_R.FP[3] == sqrtf(3.0f) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[2] = -2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x41 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fdsqrt, fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[2] = 3.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x45 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == sqrt(3.0) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[2] = -2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x45 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_SUITE_END();
