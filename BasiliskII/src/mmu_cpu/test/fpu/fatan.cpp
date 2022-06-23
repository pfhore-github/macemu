#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "fpu/fpu_mpfr.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FATAN, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x0A, 0.5, atan(0.5));
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x0A, 1.0, M_PI / 4);
}


BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x0A, v, v);
}
BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x0A, copysign(INFINITY, sg), copysign(M_PI_2, sg));
}

BOOST_AUTO_TEST_SUITE_END()
