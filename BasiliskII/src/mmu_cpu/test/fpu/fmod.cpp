#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FMOD, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double v1 = get_rx(-10.0, 10.0);
    if(v1 == 0.0) {
        v1 = 1.0;
    }
    double v2 = get_rx(-10.0, 10.0);
    if(v2 == 0.0) {
        v2 = 1.0;
    }
    fpu_test(0x21, v1, v2, fmod(v2,v1));
    BOOST_TEST(regs.fpu.FPSR.quotient == int(v2/v1)%0x80);
}

// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x21, get_rx(0.1, 10.0), z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double z = copysign(0.0, sg1);
    fpu_test(0x21, copysign(INFINITY, sg2), z, z);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x21, copysign(0.0, sg2), copysign(0.0, sg1), NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x21, get_rx(0.1, 10.0), copysign(INFINITY, sg), NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x21, copysign(0.0, sg2), copysign(INFINITY, sg1), NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x21, copysign(INFINITY, sg2), copysign(INFINITY, sg1), NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    fpu_test(0x21, copysign(0.0, sg), get_rx(0.1, 10.0), NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x21, inf, c, c);
}

BOOST_AUTO_TEST_CASE(nan1) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x21, NAN, c, NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x21, c, NAN, NAN);
}
BOOST_AUTO_TEST_SUITE_END()
