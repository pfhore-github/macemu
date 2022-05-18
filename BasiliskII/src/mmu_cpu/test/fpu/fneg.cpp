#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FNEG, InitFix)
BOOST_DATA_TEST_CASE(operand, SIGN, sg) {
    double v = get_rx(0.1, 10.0);
    fpu_test(0x1A, copysign(v, sg), 0.0, copysign(v,-sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x1A, copysign(0.0, sg), 0.0, copysign(0.0,-sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x1A, copysign(INFINITY, sg), 0.0, copysign(INFINITY,-sg));
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x1A, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSNEG, InitFix)

BOOST_DATA_TEST_CASE(operand, SIGN, sg) {
    float v = get_rx(0.1, 10.0);
    fpu_test(0x5A, copysignf(v, sg), 0.0f, copysignf(v, -sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x5A, copysignf(0.0f, sg), 0.0f, copysignf(0.0, -sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x5A, copysignf(INFINITY, sg), 0.0f, copysignf(INFINITY, -sg));
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<float>(0x5A, NAN, 0.0f, NAN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDNEG, InitFix)

BOOST_DATA_TEST_CASE(operand, SIGN, sg) {
    double v = get_rx(0.1, 10.0);
    fpu_test(0x5E, copysign(v, sg), 0.0, copysign(v, -sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x5E, copysign(0.0, sg), 0.0, copysign(0.0, -sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x5E, copysign(INFINITY, sg), 0.0, copysign(INFINITY, -sg));
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x5E, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_SUITE_END()