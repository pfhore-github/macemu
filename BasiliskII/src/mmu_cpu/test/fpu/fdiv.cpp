#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FDIV, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x20, 6.0, 2.0, 3.0);
}

// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x20, z, 1.0, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double z = copysign(0.0, sg1);
    double inf = copysign(INFINITY, sg2);
    fpu_test(0x20, z, inf, copysign(z, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double z1 = copysign(0.0, sg1);
    double z2 = copysign(0.0, sg2);
    fpu_test(0x20, z1, z2, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x20, inf, 1.0, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double inf = copysign(INFINITY, sg1);
    double z = copysign(0.0, sg2);
    fpu_test(0x20, inf, z, copysign(inf, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double inf1 = copysign(INFINITY, sg1);
    double inf2 = copysign(INFINITY, sg2);
    fpu_test(0x20, inf1, inf2, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x20, 1.0, z, copysign(INFINITY, sg));
    BOOST_TEST(fpu.FPSR.dz);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x20, 1.0, inf, copysign(0.0, sg));
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, -0x3ffe};
    xval ym{false, 1, 0x3ffe};
    fpu_test(0x20, ym, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{false, 1, -0x3ffe};
    xval ym{false, 1, 0x3ffe};
    fpu_test(0x20, xm, ym, 0.0);
    BOOST_TEST(fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSDIV, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test<float>(0x60, 6.0f, 2.0f, 3.0f);
}

// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    fpu_test(0x60, z, 1.0f, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    float z = copysignf(0.0f, sg1);
    float inf = copysignf(INFINITY, sg2);
    fpu_test(0x60, z, inf, copysignf(z, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float z1 = copysign(0.0f, sg1);
    float z2 = copysign(0.0f, sg2);
    fpu_test(0x60, z1, z2, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    fpu_test(0x60, inf, 1.0f, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float inf = copysignf(INFINITY, sg1);
    float z = copysignf(0.0f, sg2);
    fpu_test(0x60, inf, z, copysignf(inf, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    float inf1 = copysignf(INFINITY, sg1);
    float inf2 = copysignf(INFINITY, sg2);
    fpu_test(0x60, inf1, inf2, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    fpu_test(0x60, 1.0f, z, copysignf(INFINITY, sg));
    BOOST_TEST(fpu.FPSR.dz);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float i = copysignf(INFINITY, sg);
    fpu_test(0x60, 1.0f, i, copysignf(0.0, sg));
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, -0x7e};
    xval ym{false, 1, 0x7e};
    fpu_test(0x60, ym, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{false, 1, -0x7e};
    xval ym{false, 1, 0x7e};
    fpu_test(0x60, xm, ym, 0.0);
    BOOST_TEST(fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDDIV, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test<float>(0x64, 6.0, 2.0, 3.0);
}
// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x64, z, 1.0, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double z = copysign(0.0, sg1);
    double inf = copysign(INFINITY, sg2);
    fpu_test(0x64, z, inf, copysign(z, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double z1 = copysign(0.0, sg1);
    double z2 = copysign(0.0, sg2);
    fpu_test(0x64, z2, z1, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x64, inf, 1.0, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double inf = copysign(INFINITY, sg1);
    double z = copysign(0.0, sg2);
    fpu_test(0x64, inf, z, copysign(inf, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double inf1 = copysign(INFINITY, sg1);
    double inf2 = copysign(INFINITY, sg2);
    fpu_test(0x64, inf1, inf2, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x64, 1.0, z, copysign(INFINITY, sg));
    BOOST_TEST(fpu.FPSR.dz);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double i = copysign(INFINITY, sg);
    fpu_test(0x64, 1.0, i, copysign(0.0, sg));
}


BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, -0x3fe};
    xval ym{false, 1, 0x3fe};
    fpu_test(0x64, ym, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{false, 1, -0x3fe};
    xval ym{false, 1, 0x3fe};
    fpu_test(0x64, xm, ym, 0.0);
    BOOST_TEST(fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()
