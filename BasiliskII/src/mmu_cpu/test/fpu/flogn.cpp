#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FLOGN, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double in = get_rx(0.1, 100.0);
    fpu_test(0x14, in, 0.0, log(in));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    fpu_test(0x14, copysign(0.0, sg), 0.0, -INFINITY);
    BOOST_TEST(regs.fpu.FPSR.dz);
}

BOOST_AUTO_TEST_CASE(pinf) { fpu_test<double>(0x14, INFINITY, 0.0, INFINITY); }

BOOST_AUTO_TEST_CASE(minf) {
    fpu_test<double>(0x14, -INFINITY, 0.0, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(nan_) { fpu_test<double>(0x14, NAN, 0.0, NAN); }

BOOST_AUTO_TEST_CASE(operr) {
    double in = -get_rx(0.1, 100.0);
    fpu_test(0x14, in, 0.0, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}

BOOST_AUTO_TEST_SUITE_END()
