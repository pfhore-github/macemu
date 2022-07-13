#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSCALE, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x26, 2.0, 1.5, 6.0);
}

// 0 *2^ X
BOOST_AUTO_TEST_SUITE(zero_with)

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x26, 1.0, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x26, copysign(INFINITY, sg2), copysign(0.0, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
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
    fpu_test(0x26, 1.0 ,e,  e);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double e = copysign(INFINITY, sg);
    fpu_test(0x26, 0.0, e, e);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x26, copysign(INFINITY, sg1), copysign(INFINITY, sg2), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x26, z, 1.0, 1.0);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    fpu_test(0x26, copysign(INFINITY, sg), 1.0, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, 0x3fff};
    fpu_test<xval, double>(0x26, xm, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}
BOOST_AUTO_TEST_SUITE_END()
