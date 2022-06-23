#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FLOGNP1, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x06, 1.0, log(2.0));
}

BOOST_AUTO_TEST_CASE(value) {
    fpu_test(0x06, M_E - 1.0, 1.0);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x06, v, v);
}

BOOST_AUTO_TEST_CASE(pinf) { fpu_test<double>(0x06, INFINITY, INFINITY); }

BOOST_AUTO_TEST_CASE(minf) {
    fpu_test<double>(0x06, -INFINITY, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_SUITE_END()