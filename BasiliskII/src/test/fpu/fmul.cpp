#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>

BOOST_FIXTURE_TEST_SUITE(fmul_,fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[3] = 3.0;
	FPU_R.FP[2] = 2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x23 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 6.0, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[3] = INFINITY;
	FPU_R.FP[2] = 0.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x23 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(ovfl) {
	FPU_R.FP[3] = LDBL_MAX;
	FPU_R.FP[2] = LDBL_MAX;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x23 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_OVFL );
}

BOOST_AUTO_TEST_CASE(unfl) {
	FPU_R.FP[3] = LDBL_MIN;
	FPU_R.FP[2] = LDBL_MIN;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x23 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_UNFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_UNFL );
}


BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(fsglmul,fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[3] = 3.0f;
	FPU_R.FP[2] = 2.0f;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x27 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 6.0f, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[3] = INFINITY;
	FPU_R.FP[2] = 0.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x27 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(ovfl) {
	FPU_R.FP[3] = FLT_MAX;
	FPU_R.FP[2] = FLT_MAX;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x27 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_OVFL );
}

BOOST_AUTO_TEST_CASE(unfl) {
	FPU_R.FP[3] = FLT_MIN;
	FPU_R.FP[2] = FLT_MIN;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x27 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_UNFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_UNFL );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fsmul,fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[3] = 3.0f;
	FPU_R.FP[2] = 2.0f;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x63 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 6.0f, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[3] = INFINITY;
	FPU_R.FP[2] = 0.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x63 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(ovfl) {
	FPU_R.FP[3] = FLT_MAX;
	FPU_R.FP[2] = FLT_MAX;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x63 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_OVFL );
}

BOOST_AUTO_TEST_CASE(unfl) {
	FPU_R.FP[3] = FLT_MIN;
	FPU_R.FP[2] = FLT_MIN;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x63 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_UNFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_UNFL );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fdmul,fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[3] = 3.0;
	FPU_R.FP[2] = 2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x67 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 6.0, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[3] = INFINITY;
	FPU_R.FP[2] = 0.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x67 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(ovfl) {
	FPU_R.FP[3] = DBL_MAX;
	FPU_R.FP[2] = DBL_MAX;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x67 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_OVFL );
}

BOOST_AUTO_TEST_CASE(unfl) {
	FPU_R.FP[3] = DBL_MIN;
	FPU_R.FP[2] = DBL_MIN;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x67 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_UNFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_UNFL );
}


BOOST_AUTO_TEST_SUITE_END();
