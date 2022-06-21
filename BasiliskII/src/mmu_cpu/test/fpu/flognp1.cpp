#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FLOGNP1, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double in = get_rx(0.1, 10.0);
    fpu_test(0x06, in, 0.0, log1p(in));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x06, v, 0.0, v);
}

BOOST_AUTO_TEST_CASE(pinf) { fpu_test<double>(0x06, INFINITY, 0.0, INFINITY); }

BOOST_AUTO_TEST_CASE(minf) {
    fpu_test<double>(0x06, -INFINITY, 0.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_CASE(nan_) { fpu_test<double>(0x06, NAN, 0.0, NAN); }

BOOST_AUTO_TEST_SUITE_END()