#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FADD, InitFix)
static mpfr_rnd_t modes[] = {MPFR_RNDN, MPFR_RNDZ, MPFR_RNDU, MPFR_RNDD};

BOOST_AUTO_TEST_CASE(operand) {
    double v1 = get_rx(-10.0, 10.0);
    double v2 = get_rx(-10.0, 10.0);
    fpu_test(0x22, v1, v2, v1 + v2);
}

// 0 + X
BOOST_AUTO_TEST_SUITE(zero_with)

BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double d = get_rx(-10.0, 10.0);
    fpu_test(0x22, copysign(0.0, sg), d, d);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg2);
    fpu_test(0x22, copysign(0.0, sg1), d, d);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN *modes, sg1, sg2, mode) {
    regs.fpu.rnd_mode = mode;
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
    fpu_test(0x22, e, get_rx(-10.0, 10.0), e);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double e = copysign(INFINITY, sg2);
    fpu_test(0x22, e, copysign(0.0, sg1), e);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg1);
    fpu_test(0x22, copysign(INFINITY, sg2), d, (sg1 == sg2) ? d : NAN);
    if(sg1 != sg2) {
        BOOST_TEST(regs.fpu.FPSR.operr);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x22, z, c, c);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x22, inf, c, inf);
}


BOOST_AUTO_TEST_CASE(nan1) {
    fpu_test<double>(0x22, NAN, get_rx(-10.0, 10.0), NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    fpu_test<double>(0x22, get_rx(-10.0, 10.0), NAN, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{1, 0x3ffe};
    fpu_test<xval, double>(0x22, xm, xm, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}

BOOST_FIXTURE_TEST_SUITE(FSADD, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    float v1 = get_rx(-1.0, 1.0);
    float v2 = get_rx(-1.0, 1.0);
    fpu_test<float>(0x62, v1, v2, v1 + v2);
}

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float f = get_rx(-10.0, 10.0);
    fpu_test<float>(0x62, copysignf(0.0f, sg), f, f);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    float f = copysignf(INFINITY, sg2);
    fpu_test<float>(0x62, copysignf(0.0f, sg1), f, f);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN *modes, sg1, sg2, mode) {
    regs.fpu.rnd_mode = mode;
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
    float y = get_rx(-10.0, 10.0);
    fpu_test<float>(0x62, x, y, x);
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
        BOOST_TEST(regs.fpu.FPSR.operr);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    float z = copysignf(0.0, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x62, z, c, c);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x62, inf, c, inf);
}


BOOST_AUTO_TEST_CASE(nan1) {
    fpu_test<float>(0x62, 1.0f, NAN, NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    fpu_test<float>(0x62, NAN, 1.0f, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval ex{1, 0x7e};
    fpu_test(0x62, ex, ex, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDADD, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    double v1 = get_rx(-10.0, 10.0);
    double v2 = get_rx(-10.0, 10.0);
    fpu_test(0x66, v1, v2, v1+v2);
}

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double d = get_rx(-10.0, 10.0);
    fpu_test(0x66, copysign(0.0, sg), d, d);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double d = copysign(INFINITY, sg2);
    fpu_test(0x66, copysign(0.0, sg1), d, d);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN *modes, sg1, sg2, mode) {
    regs.fpu.rnd_mode = mode;
    double expected = 0.0;
    if((sg1 == -1 && sg2 == -1) || (sg1 != sg2 && mode == MPFR_RNDD)) {
        expected = -0.0;
    }
    fpu_test(0x66, copysign(0.0, sg1), copysign(0.0, sg2), expected);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double d = get_rx(-10.0, 10.0);
    double e = copysign(INFINITY, sg);
    fpu_test(0x66, e, d, e);
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
        BOOST_TEST(regs.fpu.FPSR.operr);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x66, z, c, c);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x66, inf, c, inf);
}


BOOST_AUTO_TEST_CASE(nan1) {
    double d = get_rx(-10.0, 10.0);
    fpu_test<double>(0x66, NAN, d, NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    double d = get_rx(-10.0, 10.0);
    fpu_test<double>(0x66, d, NAN, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{1, 0x3fe};
    fpu_test<xval, double>(0x66, xm, xm, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
