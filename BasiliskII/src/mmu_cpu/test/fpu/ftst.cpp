#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FTST, InitFix)
BOOST_DATA_TEST_CASE(n, SIGN, sg) {
    auto v1 = copysign(get_rx(1.0, 10.0), sg);
    fpu_test(0x3A, v1, 0.0, 0.0);
    BOOST_TEST(regs.fpu.FPSR.n == (sg < 0));
}

BOOST_DATA_TEST_CASE(z, BIT, value) {
    auto v1 = value ? 0.0 : get_rx(1.0, 10.0);
    fpu_test(0x3A, v1, 1.0, 1.0);
    BOOST_TEST(regs.fpu.FPSR.z == value);
}

BOOST_DATA_TEST_CASE(inf, BIT, value) {
    auto v1 = value ? INFINITY : get_rx(1.0, 10.0);
    fpu_test(0x3A, v1, 1.0, 1.0);
    BOOST_TEST(regs.fpu.FPSR.i == value);
}

BOOST_DATA_TEST_CASE(nan_, BIT, value) {
    auto v1 = value ? NAN : get_rx(1.0, 10.0);
    fpu_test(0x3A, v1, 1.0, 1.0);
    BOOST_TEST(regs.fpu.FPSR.nan == value);
}

BOOST_AUTO_TEST_SUITE_END()
