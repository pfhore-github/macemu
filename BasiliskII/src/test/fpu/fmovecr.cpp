#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>

static void test_cpu_fmovecr(int dst, uint8_t op) {
	test_cpu( { 0171000, (uint16_t)(0x5C00 | (dst << 7) | op) } );
}

BOOST_FIXTURE_TEST_SUITE(fmovecr,fixture);
BOOST_AUTO_TEST_CASE(pi) {
	test_cpu_fmovecr(3, 0);
	BOOST_TEST( (double)FPU_R.FP[3] == atan(1.0)*4,
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(log10_2) {
	test_cpu_fmovecr(3, 0x0B);
	BOOST_TEST( (double)FPU_R.FP[3] == log10(2.0),
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(e) {
	test_cpu_fmovecr(3, 0x0C);
	BOOST_TEST( (double)FPU_R.FP[3] == exp(1.0),
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(log2_e) {
	test_cpu_fmovecr(3, 0x0D);
	BOOST_TEST( (double)FPU_R.FP[3] == log2(exp(1.0)),
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(log10_e) {
	test_cpu_fmovecr(3, 0x0E);
	BOOST_TEST( (double)FPU_R.FP[3] == log10(exp(1.0)),
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(zero) {
	test_cpu_fmovecr(3, 0x0F);
	BOOST_TEST( (double)FPU_R.FP[3] == 0.0,
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(log_2) {
	test_cpu_fmovecr(3, 0x30);
	BOOST_TEST( (double)FPU_R.FP[3] == log(2),
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(log_10) {
	test_cpu_fmovecr(3, 0x31);
	BOOST_TEST( (double)FPU_R.FP[3] == log(10),
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(v_1e0) {
	test_cpu_fmovecr(3, 0x32);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 0.0,
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(v_1e1) {
	test_cpu_fmovecr(3, 0x33);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 1.0,
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(v_1e2) {
	test_cpu_fmovecr(3, 0x34);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 2.0,
				boost::test_tools::tolerance(1e-5) );
}
BOOST_AUTO_TEST_CASE(v_1e4) {
	test_cpu_fmovecr(3, 0x35);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 4.0,
				boost::test_tools::tolerance(1e-5) );
}
BOOST_AUTO_TEST_CASE(v_1e8) {
	test_cpu_fmovecr(3, 0x36);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 8.0,
				boost::test_tools::tolerance(1e-5) );
}
BOOST_AUTO_TEST_CASE(v_1e16) {
	test_cpu_fmovecr(3, 0x37);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 16.0,
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(v_1e32) {
	test_cpu_fmovecr(3, 0x38);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 32.0,
				boost::test_tools::tolerance(1e-5) );
}
BOOST_AUTO_TEST_CASE(v_1e64) {
	test_cpu_fmovecr(3, 0x39);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 64.0,
				boost::test_tools::tolerance(1e-5) );
}
BOOST_AUTO_TEST_CASE(v_1e128) {
	test_cpu_fmovecr(3, 0x3A);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 128.0,
				boost::test_tools::tolerance(1e-5) );
}
BOOST_AUTO_TEST_CASE(v_1e256) {
	test_cpu_fmovecr(3, 0x3B);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 256.0,
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(v_1e512) {
	test_cpu_fmovecr(3, 0x3C);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 512.0,
				boost::test_tools::tolerance(1e-5) );
}
BOOST_AUTO_TEST_CASE(v_1e1024) {
	test_cpu_fmovecr(3, 0x3D);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 1024.0,
				boost::test_tools::tolerance(1e-5) );
}
BOOST_AUTO_TEST_CASE(v_1e2048) {
	test_cpu_fmovecr(3, 0x3E);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 2048.0,
				boost::test_tools::tolerance(1e-5) );
}

BOOST_AUTO_TEST_CASE(v_1e4096) {
	test_cpu_fmovecr(3, 0x3F);
	BOOST_TEST( (double)log10l(FPU_R.FP[3]) == 4096.0,
				boost::test_tools::tolerance(1e-5) );
}


BOOST_AUTO_TEST_SUITE_END();
