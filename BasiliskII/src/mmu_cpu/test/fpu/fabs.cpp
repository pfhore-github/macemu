#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(FABS, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x18, -1.1, 1.1);
}

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x18, copysign(1.0, sg), 1.0);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x18, copysign(0.0, sg), 0.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x18, copysign(INFINITY, sg), 0.0, INFINITY);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSABS, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x58, -1.1f, 1.1f);
}

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x58, copysignf(1.1f, sg), 1.1f);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x58, copysignf(0.0f, sg), 0.0f);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x58, copysignf(INFINITY, sg), INFINITY);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDABS, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x5C, -1.1, 1.1);
}

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x5C, copysign(1.1, sg), 1.1);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x5C, copysign(0.0, sg), 0.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x5C, copysign(INFINITY, sg), INFINITY);
}

BOOST_AUTO_TEST_SUITE_END()
