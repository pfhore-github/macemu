#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FINTRZ, InitFix)
static double test_values[] = { 3.0, 2.8, 2.5, 2.3, -3.0, -2.8, -2.5, -2.3};
static double to_zero[] = { 3.0, 2.0, 2.0, 2.0, -3.0, -2.0, -2.0, -2.0};
BOOST_DATA_TEST_CASE(values, test_values ^ to_zero, value, expected) {
    mpfr_set_default_rounding_mode(MPFR_RNDZ);
    fpu_test(0x03, value, 0.0, expected);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x03, v, 0.0, v);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    double v = copysign(INFINITY, sg);
    fpu_test(0x03, v, 0.0, v);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x03, NAN, 0.0, NAN);
}
BOOST_AUTO_TEST_SUITE_END()
