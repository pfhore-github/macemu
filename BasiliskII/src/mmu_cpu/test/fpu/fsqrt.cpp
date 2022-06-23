#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSQRT, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x4, 4.0, 2.0);
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x4, 9.0, 3.0);
}

BOOST_AUTO_TEST_CASE(zero) { fpu_test(0x4, 0.0, 0.0); }

BOOST_AUTO_TEST_CASE(inf) { fpu_test<double>(0x4, INFINITY, INFINITY); }


BOOST_AUTO_TEST_CASE(operr) {
    fpu_test(0x4, -1.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(FSSQRT, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x41, 4.0f, 2.0f);
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test(0x41, 9.0f, 3.0f);
}

BOOST_AUTO_TEST_CASE(zero) { fpu_test(0x41, 0.0f, 0.0f); }

BOOST_AUTO_TEST_CASE(inf) { fpu_test<float>(0x41, INFINITY, INFINITY); }

BOOST_AUTO_TEST_CASE(operr) {
    fpu_test(0x41, -1.0f, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDSQRT, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    fpu_test2(0x45, 9.0, 3.0);
}

BOOST_AUTO_TEST_CASE(normal) {
    fpu_test2(0x45, 4.0, 2.0);
}

BOOST_AUTO_TEST_CASE(zero) { fpu_test(0x45, 0.0, 0.0); }

BOOST_AUTO_TEST_CASE(inf) { fpu_test<double>(0x45, INFINITY, INFINITY); }

BOOST_AUTO_TEST_CASE(operr) {
    fpu_test(0x45, -1.0, NAN);
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
