#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FADD, InitFix)
static mpfr_rnd_t modes[] = {MPFR_RNDN, MPFR_RNDZ, MPFR_RNDU, MPFR_RNDD};

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x22, 0.5, 1.5, 2.0);
}

// 0 + X
BOOST_AUTO_TEST_SUITE(zero_with)

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x22, copysign(0.0, sg), 2.0, 2.0);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg2);
    fpu_test(0x22, copysign(0.0, sg1), d, d);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN *modes, sg1, sg2, mode) {
    mpfr_set_default_rounding_mode(mode);
    double expected = 0.0;
    if((sg1 == -1 && sg2 == -1) || (sg1 != sg2 && mode == MPFR_RNDD)) {
        expected = -0.0;
    }
    fpu_test(0x22, copysign(0.0, sg1), copysign(0.0, sg2), expected);
}

BOOST_AUTO_TEST_SUITE_END()
// ∞ + X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double e = copysign(INFINITY, sg);
    fpu_test(0x22, e, 1.0, e);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double e = copysign(INFINITY, sg2);
    fpu_test(0x22, e, copysign(0.0, sg1), e);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg1);
    fpu_test(0x22, copysign(INFINITY, sg2), d, (sg1 == sg2) ? d : NAN);
    if(sg1 != sg2) {
        BOOST_TEST(fpu.FPSR.operr);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x22, z, 1.0, 1.0);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x22, inf, 1.0, inf);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, 0x3fff};
    fpu_test<xval, double>(0x22, xm, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_FIXTURE_TEST_SUITE(FSADD, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test<float>(0x62, 1.0f, 2.0f, 3.0f);
}

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test<float>(0x62, copysignf(0.0f, sg), 1.0f, 1.0f);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    float f = copysignf(INFINITY, sg2);
    fpu_test<float>(0x62, copysignf(0.0f, sg1), f, f);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN *modes, sg1, sg2, mode) {
    mpfr_set_default_rounding_mode(mode);
    float expected = 0.0f;
    if((sg1 == -1 && sg2 == -1) || (sg1 != sg2 && mode == MPFR_RNDD)) {
        expected = -0.0f;
    }
    fpu_test(0x62, copysignf(0.0f, sg1), copysignf(0.0f, sg2), expected);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float x = copysignf(INFINITY, sg);
    fpu_test<float>(0x62, x, 1.0f, x);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float x = copysignf(INFINITY, sg1);
    float y = copysignf(0.0f, sg2);
    fpu_test<float>(0x62, x, y, x);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    float x = copysignf(INFINITY, sg1);
    float y = copysignf(INFINITY, sg2);
    float ex = ((sg1==sg2) ? x : NAN);
    fpu_test<float>(0x62, x, y, ex);
    m68k_do_execute();
    if(sg1 != sg2) {
        BOOST_TEST(fpu.FPSR.operr);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    float z = copysignf(0.0, sg);
    fpu_test(0x62, z, 1.0f, 1.0f);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    fpu_test(0x62, inf, 1.0f, inf);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval ex{false, 1, 0x7f};
    fpu_test(0x62, ex, ex, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDADD, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x66, 1.0, 2.0, 3.0);
}

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x66, copysign(0.0, sg), 1.0, 1.0);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg2);
    fpu_test(0x66, copysign(0.0, sg1), d, d);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN *modes, sg1, sg2, mode) {
    mpfr_set_default_rounding_mode(mode);
    double expected = 0.0;
    if((sg1 == -1 && sg2 == -1) || (sg1 != sg2 && mode == MPFR_RNDD)) {
        expected = -0.0;
    }
    fpu_test(0x66, copysign(0.0, sg1), copysign(0.0, sg2), expected);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double e = copysign(INFINITY, sg);
    fpu_test(0x66, e, 1.0, e);
}


BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double d = copysign(0.0, sg1);
    double e = copysign(INFINITY, sg2);
    fpu_test(0x66, e, d, e);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg1);
    double e = copysign(INFINITY, sg2);
    fpu_test(0x66, e, d, (sg1 == sg2) ? d : NAN);
    if(sg1 != sg2) {
        BOOST_TEST(fpu.FPSR.operr);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x66, z, 1.0, 1.0);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x66, inf, 1.0, inf);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, 0x3ff};
    fpu_test<xval, double>(0x66, xm, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
