#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSINCOS, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    auto [r1, r2, r3] = rand_reg3();
    double in = get_rx(-1.0, 1.0);
    set_fpu_reg(r1, in);
    raw_write16(0, 0171000);
    raw_write16(2, r1 << 10 | r2 << 7 | 0x30 | r3);
    m68k_do_execute();
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[r2]), sin(in), 1e-10);
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[r3]), cos(in), 1e-10);
}
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    auto [r1, r2, r3] = rand_reg3();
    double x = copysign(0.0, sg);
    set_fpu_reg(r1, x);
    raw_write16(0, 0171000);
    raw_write16(2, r1 << 10 | r2 << 7 | 0x30 | r3);
    m68k_do_execute();
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[r2]), x, 1e-10);
    BOOST_CHECK_CLOSE(static_cast<double>(fpu.fp[r3]), 1.0, 1e-10);
}
BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    auto [r1, r2, r3] = rand_reg3();
    double x = copysign(INFINITY, sg);
    set_fpu_reg(r1, x);
    raw_write16(0, 0171000);
    raw_write16(2, r1 << 10 | r2 << 7 | 0x30 | r3);
    m68k_do_execute();
    BOOST_TEST(fpu.fp[r2].is_nan());
    BOOST_TEST(fpu.fp[r3].is_nan());
}

BOOST_AUTO_TEST_CASE(nan_) {
    auto [r1, r2, r3] = rand_reg3();
    set_fpu_reg(r1, NAN);
    raw_write16(0, 0171000);
    raw_write16(2, r1 << 10 | r2 << 7 | 0x30 | r3);
    m68k_do_execute();
    BOOST_TEST(fpu.fp[r2].is_nan());
    BOOST_TEST(fpu.fp[r3].is_nan());
}

BOOST_AUTO_TEST_SUITE_END()
