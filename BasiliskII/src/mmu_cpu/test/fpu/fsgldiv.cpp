#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSGLDIV, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    float v1 = get_rx(-10.0, 10.0);
    if(v1 == 0.0) {
        v1 = 1.0;
    }
    float v2 = get_rx(-10.0, 10.0);
    if(v2 == 0.0) {
        v2 = 1.0;
    }
    fpu_test(0x24, v1, v2, v2 / v1);
}
// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x24, c, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    float z = copysignf(0.0f, sg1);
    float inf = copysignf(INFINITY, sg2);
    fpu_test(0x24, inf, z, copysignf(z, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float z1 = copysign(0.0f, sg1);
    float z2 = copysign(0.0f, sg2);
    fpu_test(0x24, z2, z1, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    float d = get_rx(0.1, 10.0);
    fpu_test(0x24, d, inf, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float inf = copysignf(INFINITY, sg1);
    float z = copysignf(0.0f, sg2);
    fpu_test(0x24, z, inf, copysignf(inf, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    float inf1 = copysignf(INFINITY, sg1);
    float inf2 = copysignf(INFINITY, sg2);
    fpu_test(0x24, inf1, inf2, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x24, z, c, copysignf(INFINITY, sg));
    BOOST_TEST(regs.fpu.FPSR.dz);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float z = copysignf(INFINITY, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x24, z, c, copysignf(0.0, sg));
}

BOOST_AUTO_TEST_CASE(nan1) {
    float c = get_rx(0.1, 10.0);
    fpu_test<float>(0x24, NAN, c, NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    float c = get_rx(0.1, 10.0);
    fpu_test<float>(0x24, c, NAN, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{1, -0x7e};
    xval ym{1, 0x7e};
    fpu_test(0x24, xm, ym, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{1, -0x7e};
    xval ym{1, 0x7e};
    fpu_test(0x24, ym, xm, 0.0);
    BOOST_TEST(regs.fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()
