#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FTWOTOX, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x11, 1.0, 2.0);
}

BOOST_AUTO_TEST_CASE(normaal) {
    fpu_test(0x11, -1.0, 0.5);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x11, copysign(0.0, sg), 1.0);
}

BOOST_AUTO_TEST_CASE(pinf) { fpu_test<double>(0x11, INFINITY, INFINITY); }

BOOST_AUTO_TEST_CASE(minf) { fpu_test<double>(0x11, -INFINITY, 0.0); }


BOOST_AUTO_TEST_CASE(ovfl) {
    xval v{false, 1, 0x3ffe};
    fpu_test(0x11, v, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
