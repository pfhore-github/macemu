#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FTENTOX, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x12, 1.0, 10.0);
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test2(0x12, -1.0, 0.1);
}
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x12, copysign(0.0, sg), 1.0);
}

BOOST_AUTO_TEST_CASE(pinf) { fpu_test<double>(0x12, INFINITY, INFINITY); }

BOOST_AUTO_TEST_CASE(minf) { fpu_test<double>(0x12, -INFINITY, 0.0); }

BOOST_AUTO_TEST_CASE(ovfl) {
    xval v{false, 1, 0x3fff};
    fpu_test(0x12, v, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}
BOOST_AUTO_TEST_SUITE_END()
