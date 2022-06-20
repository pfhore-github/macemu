#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSGLMUL, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    float v1 = get_rx(-10.0, 10.0);
    if(v1 == 0.0) {
        v1 = 1.0;
    }
    float v2 = get_rx(-10.0, 10.0);
    if(v2 == 0.0) {
        v2 = 1.0;
    }
    fpu_test(0x27, v1, v2, v2 * v1);
}
// 0 * X

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x27, c, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    fpu_test<float>(0x27, copysignf(INFINITY, sg2), copysignf(0.0, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x27, copysignf(0.0f, sg2), copysignf(0.0f, sg1),
             copysignf(0.0f, sg1 * sg2));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    float d = get_rx(0.1, 10.0);
    fpu_test(0x27, d, inf, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test<float>(0x27, copysignf(0.0f, sg2), copysignf(INFINITY, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    float inf1 = copysignf(INFINITY, sg1);
    float inf2 = copysignf(INFINITY, sg2);
    fpu_test(0x27, inf1, inf2, copysignf(INFINITY, sg1*sg2));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    float z = copysignf(0.0, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x27, z, c, z);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x27, inf, c, inf);
}

BOOST_AUTO_TEST_CASE(nan1) {
    fpu_test<float>(0x27, NAN, get_rx(0.1, 10.0), NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    fpu_test<float>(0x27, get_rx(0.1, 10.0), NAN, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{false, 1, 0x7e};
    fpu_test(0x27, xm, xm, INFINITY);
    BOOST_TEST(fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{false, 1, -0x7e};
    fpu_test(0x27, xm, xm, 0.0);
    BOOST_TEST(fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()
