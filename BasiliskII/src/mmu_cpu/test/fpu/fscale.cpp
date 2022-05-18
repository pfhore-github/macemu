#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSCALE, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double v1 = get_rx(-10.0, 10.0);
    int v2 = get_v8() % 30;
    fpu_test<double>(0x26, v2, v1, ldexp(v1, v2));
}

// 0 *2^ X
BOOST_AUTO_TEST_SUITE(zero_with)

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x26, get_rx(-10.0, 10.0), z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x26, copysign(INFINITY, sg2), copysign(0.0, sg1), NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x26, 0.0, z, z);
}

BOOST_AUTO_TEST_SUITE_END()
// ∞ *2^ X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double e = copysign(INFINITY, sg);
    fpu_test(0x26, get_rx(-10.0, 10.0),e,  e);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double e = copysign(INFINITY, sg);
    fpu_test(0x26, 0.0, e, e);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x26, copysign(INFINITY, sg1), copysign(INFINITY, sg2), NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x26, z, c, c);
    BOOST_TEST(regs.fpu.FPSR.dz);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    fpu_test(0x26, copysign(INFINITY, sg), get_rx(0.1, 10.0), NAN);
}

BOOST_AUTO_TEST_CASE(nan1) {
    fpu_test<double>(0x26, NAN, get_rx(-10.0, 10.0), NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    fpu_test<double>(0x26, get_rx(-10.0, 10.0), NAN, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{1, 0x3ffe};
    fpu_test<xval, double>(0x22, xm, xm, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}
BOOST_AUTO_TEST_SUITE_END()
