#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSIN, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double in = get_rx(-1.0, 1.0);
    fpu_test(0x0E, in, 0.0, sin(in));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double d = copysign(0.0, sg);
    fpu_test<double>(0x0E, d, 0.0, d);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x0E, copysign(INFINITY, sg), 0.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x0E, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_SUITE_END()