#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FMUL, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x23, 0.5, 2.0, 1.0);
}
// 0 * X

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x23, 1.0, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    fpu_test<double>(0x23, copysign(INFINITY, sg2), copysign(0.0, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x23, copysign(0.0, sg2), copysign(0.0, sg1),
             copysign(0.0, sg1 * sg2));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x23, 1.0, inf, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test<double>(0x23, copysign(0.0, sg2), copysign(INFINITY, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double inf1 = copysign(INFINITY, sg1);
    double inf2 = copysign(INFINITY, sg2);
    fpu_test(0x23, inf1, inf2, copysign(INFINITY, sg1*sg2));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x23, z, 1.0, z);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x23, inf, 1.0, inf);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, 0x3ffe};
    fpu_test(0x23, xm, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{false, 1, -0x3ffe};
    fpu_test(0x23, xm, xm, 0.0);
    BOOST_TEST(fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSMUL, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x63, 0.5f, 2.0f, 1.0f);
}
// 0 * X

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    fpu_test(0x63, 1.0f, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    fpu_test<float>(0x63, copysignf(INFINITY, sg2), copysignf(0.0, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x63, copysignf(0.0f, sg2), copysignf(0.0f, sg1),
             copysignf(0.0f, sg1 * sg2));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    fpu_test(0x63, 1.0f, inf, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test<float>(0x63, copysignf(0.0f, sg2), copysignf(INFINITY, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    float inf1 = copysignf(INFINITY, sg1);
    float inf2 = copysignf(INFINITY, sg2);
    fpu_test(0x63, inf1, inf2, copysignf(INFINITY, sg1*sg2));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    float z = copysignf(0.0, sg);
    fpu_test(0x63, z, 1.0f, z);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    fpu_test(0x63, inf, 1.0f, inf);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, 0x7e};
    fpu_test(0x63, xm, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{false, 1, -0x7e};
    fpu_test(0x63, xm, xm, 0.0);
    BOOST_TEST(fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDMUL, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x23, 2.0, 0.5, 1.0);
}
// 0 * X

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x67, c, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    fpu_test<double>(0x67, copysign(INFINITY, sg2), copysign(0.0, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x67, copysign(0.0, sg2), copysign(0.0, sg1),
             copysign(0.0, sg1 * sg2));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x67, 1.0, inf, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test<double>(0x67, copysign(0.0, sg2), copysign(INFINITY, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double inf1 = copysign(INFINITY, sg1);
    double inf2 = copysign(INFINITY, sg2);
    fpu_test(0x67, inf1, inf2, copysign(INFINITY, sg1*sg2));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x67, z, 1.0, z);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x67, inf, 1.0, inf);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, 0x3fe};
    fpu_test(0x67, xm, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{false, 1, -0x3fe};
    fpu_test(0x67, xm, xm, 0.0);
    BOOST_TEST(fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()