#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSINCOS, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    set_fpu_reg(1, 0.1);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x30 | 3);
    m68k_do_execute();
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[2]), sin(0.1), 1e-10);
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[3]), cos(0.1), 1e-10);
}

BOOST_AUTO_TEST_CASE(operand2) {
    set_fpu_reg(1, 0.1);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x30 | 2);
    m68k_do_execute();
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[2]), sin(0.1), 1e-10);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    double x = copysign(0.0, sg);
    set_fpu_reg(1, x);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x30 | 3);
    m68k_do_execute();
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[2]), x, 1e-10);
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[3]), 1.0, 1e-10);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    double x = copysign(INFINITY, sg);
    set_fpu_reg(1, x);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x30 | 3);
    m68k_do_execute();
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[3].is_nan());
}


BOOST_AUTO_TEST_SUITE_END()
