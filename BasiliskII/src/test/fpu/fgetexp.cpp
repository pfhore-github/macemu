#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FGETEXP, InitFix)
BOOST_AUTO_TEST_CASE(operand) { fpu_test2(0x1E, 1.0, 0.0); }

BOOST_AUTO_TEST_CASE(normal) { fpu_test(0x1E, 4.0, 2.0); }

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x1E, v, v);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x1E, copysign(INFINITY, sg), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_SUITE_END()
