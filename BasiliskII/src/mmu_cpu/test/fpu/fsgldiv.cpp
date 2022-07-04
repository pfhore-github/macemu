#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSGLDIV, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x24, 6.0f, 2.0f, 3.0f);
}
// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    fpu_test(0x24, z, 1.0f, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    float z = copysignf(0.0f, sg1);
    float inf = copysignf(INFINITY, sg2);
    fpu_test(0x24, z, inf, copysignf(z, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float z1 = copysign(0.0f, sg1);
    float z2 = copysign(0.0f, sg2);
    fpu_test(0x24, z1, z2, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    fpu_test(0x24, inf, 1.0f, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float inf = copysignf(INFINITY, sg1);
    float z = copysignf(0.0f, sg2);
    fpu_test(0x24, inf, z, copysignf(inf, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    float inf1 = copysignf(INFINITY, sg1);
    float inf2 = copysignf(INFINITY, sg2);
    fpu_test(0x24, inf1, inf2, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    fpu_test(0x24, 1.0f, z, copysignf(INFINITY, sg));
    BOOST_TEST(fpu.FPSR.dz);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float i = copysignf(INFINITY, sg);
    fpu_test(0x24, 1.0f, i, copysignf(0.0, sg));
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, -0x7e};
    xval ym{false, 1, 0x7e};
    fpu_test(0x24, ym, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{false, 1, -0x7e};
    xval ym{false, 1, 0x7e};
    fpu_test(0x24, xm, ym, 0.0);
    BOOST_TEST(fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()
