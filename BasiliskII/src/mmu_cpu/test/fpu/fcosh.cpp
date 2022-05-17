#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FCOSH, InitFix)
BOOST_AUTO_TEST_CASE(operand, *boost::unit_test::tolerance(1e-12)) {
    double in = get_rx(-1.0, 1.0);
    fpu_test(0x19, in, 0.0, cosh(in));
}
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x19, copysign(0.0, sg), 0.0, 1.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    double v = copysign(INFINITY, sg);
    fpu_test(0x19, v, 0.0, INFINITY);
}

BOOST_AUTO_TEST_CASE(nan_) {
    fpu_test<double>(0x19, NAN, 0.0, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xv{1, 200};
    fpu_test(0x19, xv, xv, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
