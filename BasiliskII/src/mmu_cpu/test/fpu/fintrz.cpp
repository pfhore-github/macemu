#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(FINTRZ, InitFix)
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    set_fpu(2, copysign(0.0, sg));
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(mpfr_signbit(regs.fpu.fp[3]) == signbit(sg));
    BOOST_TEST(mpfr_zero_p(regs.fpu.fp[3]));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    set_fpu(2, copysign(INFINITY, sg));
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(mpfr_signbit(regs.fpu.fp[3]) == signbit(sg));
    BOOST_TEST(mpfr_inf_p(regs.fpu.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(positive_floor1) {
    regs.fp[2] = 2.3;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(positive_floor2) {
    regs.fp[2] = 2.5;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling1) {
    regs.fp[2] = 3.5;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(positive_ceiling2) {
    regs.fp[2] = 2.7;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.0);
}

BOOST_AUTO_TEST_CASE(negative_floor1) {
    regs.fp[2] = -2.3;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}

BOOST_AUTO_TEST_CASE(negative_floor2) {
    regs.fp[2] = -2.5;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling1) {
    regs.fp[2] = -3.5;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -3.0);
}

BOOST_AUTO_TEST_CASE(negative_ceiling2) {
    regs.fp[2] = -2.7;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == -2.0);
}
BOOST_AUTO_TEST_SUITE_END()
