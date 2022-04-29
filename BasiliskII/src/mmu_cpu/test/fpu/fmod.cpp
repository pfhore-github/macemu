#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_ieee.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FMOD, InitFix)
BOOST_DATA_TEST_CASE(zero_w_v, bdata::xrange(2), sg) {
    regs.fp[3] = sg ? -0.0 : 0.0;
    regs.fp[2] = 1.0;
    asm_m68k("fmod.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == sg);
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_DATA_TEST_CASE(inf_w, bdata::xrange(2), sg) {
    regs.fp[2] = 1.0;
    regs.fp[3] = sg ? -INFINITY : INFINITY;
    asm_m68k("fmod.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(nan1) {
    regs.fp[2] = 1.0;
    regs.fp[3] = NAN;
    asm_m68k("fmod.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan2) {
    regs.fp[3] = 1.0;
    regs.fp[2] = NAN;
    asm_m68k("fmod.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(dv0) {
    regs.fp[2] = 0.0;
    regs.fp[3] = 1.0;
    asm_m68k("fmod.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.000001)) {
    regs.fp[3] = 9.0;
    regs.fp[2] = 2.0;
    asm_m68k("fmod.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1.0);
    BOOST_TEST(regs.FPSR.qutinent == 4.0);
}

BOOST_AUTO_TEST_SUITE_END()
