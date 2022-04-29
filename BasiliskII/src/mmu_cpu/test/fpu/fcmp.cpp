#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_ieee.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FCMP, InitFix)
BOOST_AUTO_TEST_SUITE(zero_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    regs.fp[3] = copysign(0.0, sg);
    regs.fp[2] = 1.0;
    asm_m68k("fcmp.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.n);
}

BOOST_DATA_TEST_CASE(infinity, SIGN, sg) {
    regs.fp[3] = copysign(0.0, sg);
    regs.fp[2] = INFINITY;
    asm_m68k("fcmp.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.i);
    BOOST_TEST(regs.FPSR.n);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[3] = copysign(0.0, sg);
    regs.fp[2] = 0.0;
    asm_m68k("fcmp.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(infinity_with)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    regs.fp[2] = 1.0;
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fcmp.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.i);
    BOOST_TEST(regs.FPSR.n == signbit(sg));
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = 0.0;
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fcmp.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.i);
    BOOST_TEST(regs.FPSR.n == signbit(sg));
}

BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    regs.fp[2] = copysign(INFINITY, -sg);
    regs.fp[3] = copysign(INFINITY, sg);
    asm_m68k("fcmp.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.i);
    BOOST_TEST(regs.FPSR.n == signbit(sg));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(nan1) {
    regs.fp[2] = 1.0;
    regs.fp[3] = NAN;
    asm_m68k("fcmp.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.nan);
}

BOOST_AUTO_TEST_CASE(nan2) {
    regs.fp[3] = 1.0;
    regs.fp[2] = NAN;
    asm_m68k("fcmp.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.nan);
}

BOOST_AUTO_TEST_CASE(value) {
    regs.fp[3] = 1.0;
    regs.fp[2] = 2.0;
    asm_m68k("fcmp.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1.0);
}
BOOST_AUTO_TEST_SUITE_END()
