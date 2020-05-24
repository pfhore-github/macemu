#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <math.h>

namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(fcmp,fixture);
BOOST_DATA_TEST_CASE(n, bdata::xrange(2), tf) {
	FPU_R.FP[3] = 3.0;
	FPU_R.FP[2] = tf ? 4.0 : 2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x38 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_n) == tf );
}

BOOST_DATA_TEST_CASE(z, bdata::xrange(2), tf) {
	FPU_R.FP[3] = 3.0;
	FPU_R.FP[2] = tf ? 3.0 : 2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x38 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_z) == tf );
}

BOOST_DATA_TEST_CASE(infz, bdata::xrange(2), tf) {
	FPU_R.FP[3] = INFINITY;
	FPU_R.FP[2] = tf ? INFINITY : 2.0;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x38 ) });
	BOOST_TEST( !!(FPU_R.FPSR_cond & cc_z) == tf );
}


BOOST_AUTO_TEST_SUITE_END();
