#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(FABS, InitFix)

BOOST_DATA_TEST_CASE(operand, SIGN, sg) {
    double v = get_rx(0.1, 10.0);
    fpu_test(0x18, copysign(v, sg), 0.0, v);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x18, copysign(0.0, sg), 0.0, 0.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x18, copysign(INFINITY, sg), 0.0, INFINITY);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x18, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSABS, InitFix)

BOOST_DATA_TEST_CASE(operand, SIGN, sg) {
    float v = get_rx(0.1, 10.0);
    fpu_test(0x58, copysignf(v, sg), 0.0f, v);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x58, copysignf(0.0f, sg), 0.0f, 0.0f);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x58, copysignf(INFINITY, sg), 0.0f, INFINITY);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<float>(0x58, NAN, 0.0f, NAN);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDABS, InitFix)

BOOST_DATA_TEST_CASE(operand, SIGN, sg) {
    double v = get_rx(0.1, 10.0);
    fpu_test(0x5C, copysign(v, sg), 0.0, v);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x5C, copysign(0.0, sg), 0.0, 0.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x5C, copysign(INFINITY, sg), 0.0, INFINITY);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x5C, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_SUITE_END()
