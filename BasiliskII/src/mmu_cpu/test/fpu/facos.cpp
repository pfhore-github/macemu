#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FACOS, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x1C, 1.0, 0.0);
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x1C, 0.5, acos(0.5));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x1C, copysign(0.0, sg), M_PI_2);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x1C, copysign(INFINITY, sg), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(operr) {
    fpu_test(0x1C, 2.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
