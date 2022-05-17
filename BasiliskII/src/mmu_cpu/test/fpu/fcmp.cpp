#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FCMP, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double v1 = get_rx(-10.0, 10.0);
    double v2 = get_rx(-10.0, 10.0);
    fpu_test(0x38, v1, v2, v2);
    BOOST_TEST(regs.fpu.FPSR.z == (v1 == v2));
    BOOST_TEST(regs.fpu.FPSR.n == (v1 > v2));
}
// 0 - X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    double v1 = get_rx(-10.0, 10.0);
    fpu_test(0x38, v1, z, z);
    BOOST_TEST(regs.fpu.FPSR.z == (v1 == 0.0));
    BOOST_TEST(regs.fpu.FPSR.n == (v1 > 0.0));
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double z = copysign(0.0, sg1);
    double v1 = copysign(INFINITY, sg2);
    fpu_test(0x38, v1, z, z);
    BOOST_TEST(regs.fpu.FPSR.i);
    BOOST_TEST(regs.fpu.FPSR.n == (sg2 > 0));
}

BOOST_AUTO_TEST_CASE(zero) {
    fpu_test(0x38, 0.0, 0.0, 0.0);
    BOOST_TEST(regs.fpu.FPSR.z);
}

BOOST_AUTO_TEST_SUITE_END()

// ∞ - X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    double v1 = get_rx(-10.0, 10.0);
    fpu_test(0x38, v1, inf, inf);
    BOOST_TEST(regs.fpu.FPSR.i);
    BOOST_TEST(regs.fpu.FPSR.n == signbit(sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double inf = copysign(INFINITY, sg1);
    double z = copysign(0.0, sg2);
    fpu_test(0x38, z, inf, inf);
    BOOST_TEST(regs.fpu.FPSR.i);
    BOOST_TEST(regs.fpu.FPSR.n == signbit(sg1));
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double inf1 = copysign(INFINITY, sg1);
    double inf2 = copysign(INFINITY, sg2);
    fpu_test(0x38, inf2, inf1, inf1);
    if(sg1 != sg2) {
        BOOST_TEST(regs.fpu.FPSR.i);
        BOOST_TEST(regs.fpu.FPSR.n == signbit(sg1));
    } else {
        BOOST_TEST(regs.fpu.FPSR.nan);
    }
}

BOOST_AUTO_TEST_CASE(nan1) {
    fpu_test<double>(0x38, NAN, 1.0, 1.0);
    BOOST_TEST(regs.fpu.FPSR.nan);
}

BOOST_AUTO_TEST_CASE(nan2) {
    fpu_test<double>(0x38, 1.0, NAN, NAN);
    BOOST_TEST(regs.fpu.FPSR.nan);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
