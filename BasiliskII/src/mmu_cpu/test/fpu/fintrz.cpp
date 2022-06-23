#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FINTRZ, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    fpu_test2(0x03, 1.2, 1.0);
}

static double test_values[] = { 3.0, 2.8, 2.5, 2.3, -3.0, -2.8, -2.5, -2.3};
static double to_zero[] = { 3.0, 2.0, 2.0, 2.0, -3.0, -2.0, -2.0, -2.0};
BOOST_DATA_TEST_CASE(values, test_values ^ to_zero, value, expected) {
    mpfr_set_default_rounding_mode(MPFR_RNDZ);
    fpu_test(0x03, value, expected);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x03, v, v);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    double v = copysign(INFINITY, sg);
    fpu_test(0x03, v, v);
}

BOOST_AUTO_TEST_SUITE_END()
