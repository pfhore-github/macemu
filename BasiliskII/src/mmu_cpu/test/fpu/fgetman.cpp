#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FGETMAN, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    int _;
    double v1 = get_rx(1.0, 100.0);
    fpu_test(0x1F, v1, 0.0, frexp(v1, &_)*2);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x1F, v, 0.0, v);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x1F, copysign(INFINITY, sg), 0.0, NAN);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x1F, NAN, 0.0, NAN);
}



BOOST_AUTO_TEST_SUITE_END()
