#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
static mpfr_rnd_t modes[] = {MPFR_RNDN, MPFR_RNDZ, MPFR_RNDU, MPFR_RNDD};
BOOST_FIXTURE_TEST_SUITE(FSUB, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x28, 0.5, 2.0, 1.5);
}

// 0 - X
BOOST_AUTO_TEST_SUITE(zero_with)

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x28, 1.0, copysign(0.0, sg), -1.0);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg2);
    fpu_test(0x28, d, copysign(0.0, sg1), -d);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN *modes, sg1, sg2, mode) {
    mpfr_set_default_rounding_mode(mode);
    double expected = 0.0;
    if((sg1 == 1 && sg2 == -1) || (sg1 == sg2 && mode == MPFR_RNDD)) {
        expected = -0.0;
    }
    fpu_test(0x28, copysign(0.0, sg1), copysign(0.0, sg2), expected);
}

BOOST_AUTO_TEST_SUITE_END()
// ∞ - X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double e = copysign(INFINITY, sg);
    fpu_test(0x28, 1.0, e, e);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double e = copysign(INFINITY, sg2);
    fpu_test(0x28, copysign(0.0, sg1), e, e);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg1);
    fpu_test(0x28, copysign(INFINITY, sg2), d, (sg1 != sg2) ? d : NAN);
    if(sg1 == sg2) {
        BOOST_TEST(fpu.FPSR.operr);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    fpu_test(0x28, copysign(0.0, sg), 1.0, 1.0);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x28, inf, 1.0, -inf);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{true, ~0LLU, 0x3fff};
    xval ym{false, 1, 0x3fff};
    fpu_test<xval, double>(0x28, xm, ym, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSSUB, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x68, 1.0f, 3.0f, 2.0f);
}

// 0 - X
BOOST_AUTO_TEST_SUITE(zero_with)

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x68, 1.0f, copysignf(0.0, sg), -1.0f);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    float d = copysignf(INFINITY, sg2);
    fpu_test(0x68, d, copysignf(0.0, sg1), -d);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN *modes, sg1, sg2, mode) {
    mpfr_set_default_rounding_mode(mode);
    float expected = 0.0f;
    if((sg1 == 1 && sg2 == -1) || (sg1 == sg2 && mode == MPFR_RNDD)) {
        expected = -0.0f;
    }
    fpu_test(0x28, copysign(0.0, sg1), copysign(0.0, sg2), expected);
}

BOOST_AUTO_TEST_SUITE_END()
// ∞ - X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float e = copysignf(INFINITY, sg);
    fpu_test(0x68, 1.0f, e, e);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float e = copysignf(INFINITY, sg2);
    fpu_test(0x68, copysignf(0.0, sg1), e, e);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    float d = copysignf(INFINITY, sg1);
    fpu_test(0x68, copysignf(INFINITY, sg2), d, (sg1 != sg2) ? d : NAN);
    if(sg1 == sg2) {
        BOOST_TEST(fpu.FPSR.operr);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    fpu_test(0x68, copysignf(0.0f, sg), 1.0f, 1.0f);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    fpu_test<float>(0x68, inf, 1.0f, -inf);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, ~0LLU, 127};
    xval ym{true, 1, 127};
    fpu_test<xval, float>(0x68, xm, ym, -INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(FDSUB, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x6c, 2.0, 5.0, 3.0);
}

// 0 - X
BOOST_AUTO_TEST_SUITE(zero_with)

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x6c, 1.0, copysign(0.0, sg), -1.0);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg2);
    fpu_test(0x6c, d, copysign(0.0, sg1), -d);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN *modes, sg1, sg2, mode) {
    mpfr_set_default_rounding_mode(mode);
    double expected = 0.0;
    if((sg1 == 1 && sg2 == -1) || (sg1 == sg2 && mode == MPFR_RNDD)) {
        expected = -0.0;
    }
    fpu_test(0x6c, copysign(0.0, sg1), copysign(0.0, sg2), expected);
}

BOOST_AUTO_TEST_SUITE_END()
// ∞ - X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double e = copysign(INFINITY, sg);
    fpu_test(0x6c, 1.0, e, e);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double e = copysign(INFINITY, sg2);
    fpu_test(0x6c, copysign(0.0, sg1), e, e);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg1);
    fpu_test(0x6c, copysign(INFINITY, sg2), d, (sg1 != sg2) ? d : NAN);
    if(sg1 == sg2) {
        BOOST_TEST(fpu.FPSR.operr);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    fpu_test(0x6c, copysign(0.0, sg), 1.0, 1.0);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x6c, inf, 1.0, -inf);
}


BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, 0x3fff};
    xval ym{true, 1, 0x3fff};
    fpu_test<xval, double>(0x6c, xm, ym, -INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}
BOOST_AUTO_TEST_SUITE_END()