#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FMUL, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double v1 = get_rx(-10.0, 10.0);
    if(v1 == 0.0) {
        v1 = 1.0;
    }
    double v2 = get_rx(-10.0, 10.0);
    if(v2 == 0.0) {
        v2 = 1.0;
    }
    fpu_test(0x23, v1, v2, v2 * v1);
}
// 0 * X

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x23, c, z, z);
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
    double d = get_rx(0.1, 10.0);
    fpu_test(0x23, d, inf, inf);
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
    double c = get_rx(0.1, 10.0);
    fpu_test(0x23, z, c, z);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x23, inf, c, inf);
}

BOOST_AUTO_TEST_CASE(nan1) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x23, NAN, c, NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x23, c, NAN, NAN);
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
    float v1 = get_rx(-10.0, 10.0);
    if(v1 == 0.0) {
        v1 = 1.0;
    }
    float v2 = get_rx(-10.0, 10.0);
    if(v2 == 0.0) {
        v2 = 1.0;
    }
    fpu_test(0x63, v1, v2, v2 * v1);
}
// 0 * X

BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x63, c, z, z);
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
    float d = get_rx(0.1, 10.0);
    fpu_test(0x63, d, inf, inf);
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
    float c = get_rx(0.1, 10.0);
    fpu_test(0x63, z, c, z);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x63, inf, c, inf);
}

BOOST_AUTO_TEST_CASE(nan1) {
    fpu_test<float>(0x63, NAN, get_rx(0.1, 10.0), NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    fpu_test<float>(0x63, get_rx(0.1, 10.0), NAN, NAN);
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
    double v1 = get_rx(-10.0, 10.0);
    if(v1 == 0.0) {
        v1 = 1.0;
    }
    double v2 = get_rx(-10.0, 10.0);
    if(v2 == 0.0) {
        v2 = 1.0;
    }
    fpu_test(0x23, v1, v2, v2 * v1);
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
    double d = get_rx(0.1, 10.0);
    fpu_test(0x67, d, inf, inf);
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
    double c = get_rx(0.1, 10.0);
    fpu_test(0x67, z, c, z);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x67, inf, c, inf);
}

BOOST_AUTO_TEST_CASE(nan1) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x67, NAN, c, NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x67, c, NAN, NAN);
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