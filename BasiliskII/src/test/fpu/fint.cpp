#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <fenv.h>
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(fint, fixture);
BOOST_DATA_TEST_CASE(to_nearest1, bdata::xrange(2), sign) {
	fesetround(FE_TONEAREST);
	FPU_R.FP[2] = sign ? -2.7 : 2.7;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == (sign ? -3.0 : 3.0) );
}

BOOST_DATA_TEST_CASE(to_nearest2, bdata::xrange(2), sign) {
	fesetround(FE_TONEAREST);
	FPU_R.FP[2] = sign ? -2.2 : 2.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == (sign ? -2.0 : 2.0) );
}

BOOST_DATA_TEST_CASE(upward, bdata::xrange(2), sign) {
	fesetround(FE_UPWARD);
	FPU_R.FP[2] = sign ? -2.2 : 2.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == (sign ? -2.0 : 3.0) );
}

BOOST_DATA_TEST_CASE(downward, bdata::xrange(2), sign) {
	fesetround(FE_DOWNWARD);
	FPU_R.FP[2] = sign ? -2.2 : 2.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == (sign ? -3.0 : 2.0) );
}

BOOST_DATA_TEST_CASE(toward_zero, bdata::xrange(2), sign) {
	fesetround(FE_TOWARDZERO);
	FPU_R.FP[2] = sign ? -2.2 : 2.2;
	test_cpu( { 0171000, (uint16_t)(2 << 10 | 3 << 7 | 0x1 ) });
	BOOST_TEST( (double)FPU_R.FP[3] == (sign ? -2.0 : 2.0) );
}

BOOST_AUTO_TEST_SUITE_END();
