#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FASIN, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x0C, 1.0, M_PI_2);
}
BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x0C, 0.5, asin(0.5));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x0C, v, v);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x0C, copysign(INFINITY, sg), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(operr) {
    fpu_test<double>(0x0C, 2.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
