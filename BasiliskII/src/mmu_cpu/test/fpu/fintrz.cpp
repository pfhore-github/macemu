#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_ieee.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(FINTRZ, InitFix)
BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sg) {
    regs.fp[2] = sg ? -0.0 : 0.0;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == sg);
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_DATA_TEST_CASE(inf, bdata::xrange(2), sg) {
    regs.fp[2] = sg ? -INFINITY : INFINITY;
    asm_m68k("fintrz.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == sg);
    BOOST_TEST(isinf(regs.fp[3]));
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
