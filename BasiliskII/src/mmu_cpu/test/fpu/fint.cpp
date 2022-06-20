#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "fpu/fpu_mpfr.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FINT, InitFix)
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x01, v, 0.0, v);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    double v = copysign(INFINITY, sg);
    fpu_test(0x01, v, 0.0, v);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x01, NAN, 0.0, NAN);
}
static double test_values[] = { 3.0, 2.8, 2.5, 2.3, -3.0, -2.8, -2.5, -2.3};
static double to_nearest[] = { 3.0, 3.0, 2.0, 2.0, -3.0, -3.0, -2.0, -2.0};
static double to_zero[] = { 3.0, 2.0, 2.0, 2.0, -3.0, -2.0, -2.0, -2.0};
static double downward[] = { 3.0, 2.0, 2.0, 2.0, -3.0, -3.0, -3.0, -3.0};
static double upward[] = { 3.0, 3.0, 3.0, 3.0, -3.0, -2.0, -2.0, -2.0};
BOOST_DATA_TEST_CASE(TO_NEAREST, test_values ^ to_nearest, value, expected) {
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    fpu_test(0x01, value, 0.0, expected);
}

BOOST_DATA_TEST_CASE(TO_ZERO, test_values ^ to_zero, value, expected) {
    mpfr_set_default_rounding_mode(MPFR_RNDZ);
    fpu_test(0x01, value, 0.0, expected);
}

BOOST_DATA_TEST_CASE(DOWNWARD, test_values ^ downward, value, expected) {
    mpfr_set_default_rounding_mode(MPFR_RNDD);
    fpu_test(0x01, value, 0.0, expected);
}

BOOST_DATA_TEST_CASE(UPWARD, test_values ^ upward, value, expected) {
    mpfr_set_default_rounding_mode(MPFR_RNDU);
    fpu_test(0x01, value, 0.0, expected);
}
BOOST_AUTO_TEST_SUITE_END()