#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FNEG, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x1A, 1.1, -1.1);
}

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x1A, copysign(1.0, sg), copysign(1.0, -sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x1A, copysign(0.0, sg), copysign(0.0,-sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x1A, copysign(INFINITY, sg), copysign(INFINITY,-sg));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSNEG, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x5A, -1.1f, 1.1f);
}

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x5A, copysignf(1.0, sg), copysignf(1.0, -sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x5A, copysignf(0.0f, sg), copysignf(0.0, -sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x5A, copysignf(INFINITY, sg), copysignf(INFINITY, -sg));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDNEG, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x5E, -1.1f, 1.1f);
}

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x5E, copysign(1.0, sg), copysign(1.0, -sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x5E, copysign(0.0, sg), copysign(0.0, -sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x5E, copysign(INFINITY, sg), copysign(INFINITY, -sg));
}

BOOST_AUTO_TEST_SUITE_END()