#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FCOS, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x1D, 1.0, cos(1.0));
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x1D, 0.1, cos(0.1));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test<double>(0x1D, copysign(0.0, sg), 1.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x1D, copysign(INFINITY, sg), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_SUITE_END()
