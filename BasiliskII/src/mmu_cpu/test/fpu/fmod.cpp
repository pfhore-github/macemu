#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FMOD, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test(0x21, 2.0, 7.0, 1.0);
    BOOST_TEST( fpu.FPSR.quotient == 3);
}

// 0 / X
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    double z = copysign(0.0, sg);
    fpu_test(0x21, 1.0, z, z);
}

BOOST_DATA_TEST_CASE(infinity, SIGN *SIGN, sg1, sg2) {
    double z = copysign(0.0, sg1);
    fpu_test(0x21, copysign(INFINITY, sg2), z, z);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x21, copysign(0.0, sg2), copysign(0.0, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
// ∞ / X
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    fpu_test(0x21, 1.0, copysign(INFINITY, sg), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(zero, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x21, copysign(0.0, sg2), copysign(INFINITY, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(inf, SIGN *SIGN, sg1, sg2) {
    fpu_test(0x21, copysign(INFINITY, sg2), copysign(INFINITY, sg1), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(with_zero, SIGN, sg) {
    fpu_test(0x21, copysign(0.0, sg), get_rx(0.1, 10.0), NAN);
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_DATA_TEST_CASE(with_inf, SIGN, sg) {
    double inf = copysign(INFINITY, sg);
    fpu_test(0x21, inf, 1.0, 1.0);
}

BOOST_AUTO_TEST_SUITE_END()
