#define BOOST_TEST_DYN_LINK

#include "test_common.hpp"
#include <math.h>

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(fadd_,fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[3] = 3.0;
	FPU_R.FP[2] = 2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x22 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 5.0, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[3] = INFINITY;
	FPU_R.FP[2] = -INFINITY;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x22 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(ovfl) {
	FPU_R.FP[3] = LDBL_MAX;
	FPU_R.FP[2] = LDBL_MAX;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x22 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_OVFL );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fsadd,fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[3] = 3.0f;
	FPU_R.FP[2] = 2.0f;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x62 ) });
	BOOST_TEST( (float)FPU_R.FP[3] == 5.0f, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[3] = INFINITY;
	FPU_R.FP[2] = -INFINITY;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x62 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(ovfl) {
	FPU_R.FP[3] = FLT_MAX;
	FPU_R.FP[2] = FLT_MAX;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x62 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_OVFL );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(fdadd,fixture);
BOOST_AUTO_TEST_CASE(result) {
	FPU_R.FP[3] = 3.0;
	FPU_R.FP[2] = 2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x66 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == 5.0, boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(operr) {
	FPU_R.FP[3] = INFINITY;
	FPU_R.FP[2] = -INFINITY;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x66 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OPERR );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_IOP );
}

BOOST_AUTO_TEST_CASE(ovfl) {
	FPU_R.FP[3] = DBL_MAX;
	FPU_R.FP[2] = DBL_MAX;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x66 ) });
	BOOST_TEST( FPU_R.FPSR_ex & EX_OVFL );
	BOOST_TEST( FPU_R.FPSR_aex & EX_ACC_OVFL );
}


BOOST_AUTO_TEST_SUITE_END();
