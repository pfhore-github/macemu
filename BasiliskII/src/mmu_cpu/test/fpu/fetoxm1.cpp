#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <vector>
BOOST_FIXTURE_TEST_SUITE(FETOXM1, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x08, 1.0, M_E - 1.0);
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x08, 0.5, exp(0.5) - 1.0);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double v = copysign(0.0, sg);
    fpu_test(0x08, v, v);
}

BOOST_AUTO_TEST_CASE(pinf) { fpu_test<double>(0x08, INFINITY, INFINITY); }

BOOST_AUTO_TEST_CASE(minf) { fpu_test<double>(0x08, -INFINITY, -1.0); }

BOOST_AUTO_TEST_CASE(ovfl) {
    xval v{false, 1, 0x3fff};
    fpu_test(0x08, v, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}


BOOST_AUTO_TEST_SUITE_END()