#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FCOS, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double in = get_rx(-1.0, 1.0);
    fpu_test(0x1D, in, 0.0, cos(in));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test<double>(0x1D, copysign(0.0, sg), 0.0, 1.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x1D, copysign(INFINITY, sg), 0.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x1D, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_SUITE_END()
