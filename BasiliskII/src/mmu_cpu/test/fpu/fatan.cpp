#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "fpu/fpu_mpfr.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FATAN, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double in = get_rx(-1.0, 1.0);
    fpu_test(0x0A, in, 0.0, atan(in));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x0A, v, 0.0, v);
}
BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x0A, copysign(INFINITY, sg), 0.0, copysign(M_PI_2, sg));
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x0A, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_SUITE_END()
