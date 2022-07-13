#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FCOSH, InitFix)
BOOST_AUTO_TEST_CASE(operand, *boost::unit_test::tolerance(1e-12)) {
    fpu_test2(0x19, 0.2, cosh(0.2));
}

BOOST_AUTO_TEST_CASE(normal, *boost::unit_test::tolerance(1e-12)) {
    fpu_test(0x19, 0.1, cosh(0.1));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x19, copysign(0.0, sg), 1.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test(0x19, copysign(INFINITY, sg), INFINITY);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xv{false, 1, 200};
    fpu_test(0x19, xv, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
