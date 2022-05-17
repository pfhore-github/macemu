#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FDIV, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double v1 = get_rx(-10.0, 10.0);
    if(v1 == 0.0) {
        v1 = 1.0;
    }
    double v2 = get_rx(-10.0, 10.0);
    if(v2 == 0.0) {
        v2 = 1.0;
    }
    fpu_test(0x20, v1, v2, v2 / v1);
}
// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x20, c, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double z = copysign(0.0, sg1);
    double inf = copysign(INFINITY, sg2);
    fpu_test(0x20, inf, z, copysign(z, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double z1 = copysign(0.0, sg1);
    double z2 = copysign(0.0, sg2);
    fpu_test(0x20, z2, z1, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    double d = get_rx(0.1, 10.0);
    fpu_test(0x20, d, inf, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double inf = copysign(INFINITY, sg1);
    double z = copysign(0.0, sg2);
    fpu_test(0x20, z, inf, copysign(inf, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double inf1 = copysign(INFINITY, sg1);
    double inf2 = copysign(INFINITY, sg2);
    fpu_test(0x20, inf1, inf2, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x20, z, c, copysign(INFINITY, sg));
    BOOST_TEST(regs.fpu.FPSR.dz);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x20, inf, c, copysign(0.0, sg));
}

BOOST_AUTO_TEST_CASE(nan1) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x20, NAN, c, NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x20, c, NAN, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{1, -0x3ffe};
    xval ym{1, 0x3ffe};
    fpu_test(0x20, xm, ym, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{1, -0x3ffe};
    xval ym{1, 0x3ffe};
    fpu_test(0x20, ym, xm, 0.0);
    BOOST_TEST(regs.fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSDIV, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double v1 = get_rx(-10.0, 10.0);
    if(v1 == 0.0) {
        v1 = 1.0;
    }
    double v2 = get_rx(-10.0, 10.0);
    if(v2 == 0.0) {
        v2 = 1.0;
    }
    fpu_test(0x20, v1, v2, v2 / v1);
}
// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x60, c, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    float z = copysignf(0.0f, sg1);
    float inf = copysignf(INFINITY, sg2);
    fpu_test(0x60, inf, z, copysignf(z, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float z1 = copysign(0.0f, sg1);
    float z2 = copysign(0.0f, sg2);
    fpu_test(0x60, z2, z1, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    float inf = copysignf(INFINITY, sg);
    float d = get_rx(0.1, 10.0);
    fpu_test(0x60, d, inf, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    float inf = copysignf(INFINITY, sg1);
    float z = copysignf(0.0f, sg2);
    fpu_test(0x60, z, inf, copysignf(inf, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    float inf1 = copysignf(INFINITY, sg1);
    float inf2 = copysignf(INFINITY, sg2);
    fpu_test(0x60, inf1, inf2, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    float z = copysignf(0.0f, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x60, z, c, copysignf(INFINITY, sg));
    BOOST_TEST(regs.fpu.FPSR.dz);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    float z = copysignf(INFINITY, sg);
    float c = get_rx(0.1, 10.0);
    fpu_test(0x60, z, c, copysignf(0.0, sg));
}

BOOST_AUTO_TEST_CASE(nan1) {
    float c = get_rx(0.1, 10.0);
    fpu_test<float>(0x60, NAN, c, NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    float c = get_rx(0.1, 10.0);
    fpu_test<float>(0x60, c, NAN, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{1, -0x7e};
    xval ym{1, 0x7e};
    fpu_test(0x60, xm, ym, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{1, -0x7e};
    xval ym{1, 0x7e};
    fpu_test(0x60, ym, xm, 0.0);
    BOOST_TEST(regs.fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDDIV, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double v1 = get_rx(-10.0, 10.0);
    if(v1 == 0.0) {
        v1 = 1.0;
    }
    double v2 = get_rx(-10.0, 10.0);
    if(v2 == 0.0) {
        v2 = 1.0;
    }
    fpu_test(0x64, v1, v2, v2 / v1);
}
// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x64, c, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double z = copysign(0.0, sg1);
    double inf = copysign(INFINITY, sg2);
    fpu_test(0x64, inf, z, copysign(z, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double z1 = copysign(0.0, sg1);
    double z2 = copysign(0.0, sg2);
    fpu_test(0x64, z2, z1, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    double d = get_rx(0.1, 10.0);
    fpu_test(0x64, d, inf, inf);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    double inf = copysign(INFINITY, sg1);
    double z = copysign(0.0, sg2);
    fpu_test(0x64, z, inf, copysign(inf, sg1 * sg2));
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    double inf1 = copysign(INFINITY, sg1);
    double inf2 = copysign(INFINITY, sg2);
    fpu_test(0x64, inf1, inf2, NAN);
    BOOST_TEST(regs.fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    double z = copysign(0.0, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x64, z, c, copysign(INFINITY, sg));
    BOOST_TEST(regs.fpu.FPSR.dz);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double z = copysign(INFINITY, sg);
    double c = get_rx(0.1, 10.0);
    fpu_test(0x64, z, c, copysign(0.0, sg));
}

BOOST_AUTO_TEST_CASE(nan1) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x64, NAN, c, NAN);
}

BOOST_AUTO_TEST_CASE(nan2) {
    double c = get_rx(0.1, 10.0);
    fpu_test<double>(0x64, c, NAN, NAN);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    xval xm{1, -0x3fe};
    xval ym{1, 0x3fe};
    fpu_test(0x64, xm, ym, INFINITY);
    BOOST_TEST(regs.fpu.FPSR.ovfl);
}

BOOST_AUTO_TEST_CASE(unfl) {
    xval xm{1, -0x3fe};
    xval ym{1, 0x3fe};
    fpu_test(0x64, ym, xm, 0.0);
    BOOST_TEST(regs.fpu.FPSR.unfl);
}
BOOST_AUTO_TEST_SUITE_END()
