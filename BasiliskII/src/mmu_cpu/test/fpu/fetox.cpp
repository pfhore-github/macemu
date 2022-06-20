#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FETOX, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double in = get_rx(-1.0, 1.0);
    fpu_test(0x10, in, 0.0, exp(in));
}
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x10, copysign(0.0, sg), 0.0, 1.0);
}

BOOST_AUTO_TEST_CASE(pinf) { fpu_test<double>(0x10, INFINITY, 0.0, INFINITY); }

BOOST_AUTO_TEST_CASE(minf) { fpu_test<double>(0x10, -INFINITY, 0.0, 0.0); }

BOOST_AUTO_TEST_CASE(nan_) { fpu_test<double>(0x10, NAN, 0.0, NAN); }

BOOST_AUTO_TEST_CASE(ovfl) {
    xval v{false, 1, 0x3fff};
    fpu_test(0x10, v, v, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
