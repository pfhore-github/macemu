#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSQRT, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double v = get_rx(0.1, 10.0);
    fpu_test(0x4, v, 0.0, sqrt(v));
}

BOOST_AUTO_TEST_CASE(zero) { fpu_test(0x4, 0.0, 0.0, 0.0); }

BOOST_AUTO_TEST_CASE(inf) { fpu_test<double>(0x4, INFINITY, 0.0, INFINITY); }

BOOST_AUTO_TEST_CASE(nan_) { fpu_test<double>(0x4, NAN, 0.0, NAN); }

BOOST_AUTO_TEST_CASE(operr) {
    double v = get_rx(0.1, 10.0);
    fpu_test(0x4, -v, 0.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(FSSQRT, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    float v = get_rx(0.1, 10.0);
    fpu_test(0x41, v, 0.0f, sqrtf(v));
}

BOOST_AUTO_TEST_CASE(zero) { fpu_test(0x41, 0.0f, 0.0f, 0.0f); }

BOOST_AUTO_TEST_CASE(inf) { fpu_test<float>(0x41, INFINITY, 0.0f, INFINITY); }

BOOST_AUTO_TEST_CASE(nan_) { fpu_test<float>(0x41, NAN, 0.0f, NAN); }

BOOST_AUTO_TEST_CASE(operr) {
    float v = get_rx(0.1, 10.0);
    fpu_test(0x41, -v, 0.0f, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDSQRT, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    double v = get_rx(0.1, 10.0);
    fpu_test(0x45, v, 0.0, sqrt(v));
}

BOOST_AUTO_TEST_CASE(zero) { fpu_test(0x45, 0.0, 0.0, 0.0); }

BOOST_AUTO_TEST_CASE(inf) { fpu_test<double>(0x45, INFINITY, 0.0, INFINITY); }

BOOST_AUTO_TEST_CASE(nan_) { fpu_test<double>(0x45, NAN, 0.0, NAN); }

BOOST_AUTO_TEST_CASE(operr) {
    double v = get_rx(0.1, 10.0);
    fpu_test(0x45, -v, 0.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
