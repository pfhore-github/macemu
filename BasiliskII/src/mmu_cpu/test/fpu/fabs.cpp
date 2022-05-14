#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(FABS, InitFix)
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = copysign(0.0, sg);
    asm_m68k("fabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(!signbit(regs.fp[3]));
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    regs.fp[2] = copysign(INFINITY, sg);
    asm_m68k("fabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(!signbit(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_DATA_TEST_CASE(value, SIGN, sg) {
    regs.fp[2] = copysign(2.3, sg);
    asm_m68k("fabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.3);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FSABS, InitFix)
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = copysignf(0.0f, sg);
    asm_m68k("fsabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(!signbit(regs.fp[3]));
    BOOST_TEST(regs.fp[3] == 0.0f);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    regs.fp[2] = copysignf(INFINITY, sg);
    asm_m68k("fsabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(!signbit(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fsabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_DATA_TEST_CASE(value, SIGN, sg) {
    regs.fp[2] = copysignf(2.3f, sg);
    asm_m68k("fsabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.3f);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDABS, InitFix)
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = copysign(0.0, sg);
    asm_m68k("fdabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(!signbit(regs.fp[3]));
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    regs.fp[2] = copysign(INFINITY, sg);
    asm_m68k("fdabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(!signbit(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fdabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_DATA_TEST_CASE(value, SIGN, sg) {
    regs.fp[2] = copysign(2.3, sg);
    asm_m68k("fdabs.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 2.3);
}

BOOST_AUTO_TEST_SUITE_END()