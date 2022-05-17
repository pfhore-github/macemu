#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FGETEXP, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    int exp;
    double v1 = get_rx(1.0, 100.0);
    frexp(v1, &exp);
    fpu_test(0x1E, v1, 0.0, static_cast<double>(exp-1));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x1E, v, 0.0, v);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x1E, copysign(INFINITY, sg), 0.0, NAN);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x1E, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_SUITE_END()
