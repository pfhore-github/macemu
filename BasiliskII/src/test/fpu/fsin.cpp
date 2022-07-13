#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSIN, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x0E, 1.0, sin(1.0));
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x0E, 0.1, sin(0.1));
}
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double d = copysign(0.0, sg);
    fpu_test<double>(0x0E, d, d);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x0E, copysign(INFINITY, sg), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_SUITE_END()