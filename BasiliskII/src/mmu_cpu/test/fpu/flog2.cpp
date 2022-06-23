#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FLOG2, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x16, 1.0, 0.0);
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test2(0x16, 2.0, 1.0);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x16, copysign(0.0, sg), -INFINITY);
    BOOST_TEST(fpu.FPSR.dz);
}

BOOST_AUTO_TEST_CASE(pinf) { fpu_test<double>(0x16, INFINITY, INFINITY); }

BOOST_AUTO_TEST_CASE(minf) {
    fpu_test<double>(0x16, -INFINITY, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(operr) {
    fpu_test(0x16, -1.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}


BOOST_AUTO_TEST_SUITE_END()
