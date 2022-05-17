#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FATANH, InitFix)
BOOST_AUTO_TEST_CASE(operand, *boost::unit_test::tolerance(1e-12)) {
    double in = get_rx(-0.5, 0.5);
    fpu_test(0x0D, in, 0.0, atanh(in));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double in = copysign(0.0, sg);
    fpu_test(0x0D, in, 0.0, in);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    fpu_test<double>(0x0D, copysign(INFINITY, sg), 0.0, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(nan_) { fpu_test<double>(0x0D, NAN, 0.0, NAN); }

BOOST_DATA_TEST_CASE(dz, SIGN, sg) {
    fpu_test<double>(0x0D, copysign(1.0, sg), 0.0, copysign(INFINITY, sg));
    BOOST_TEST(regs.fpu.FPSR.dz);
}

BOOST_AUTO_TEST_CASE(operr) {
    fpu_test<double>(0x0D, 2.0, 0.0, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}

BOOST_AUTO_TEST_SUITE_END()
