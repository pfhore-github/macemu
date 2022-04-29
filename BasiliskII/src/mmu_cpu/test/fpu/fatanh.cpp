#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_ieee.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FATANH, InitFix)
BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sg) {
    regs.fp[2] = sg ? -0.0 : 0.0;
    asm_m68k("fatanh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == sg);
    BOOST_TEST(regs.fp[3] == 0.0);
}
BOOST_DATA_TEST_CASE(inf, bdata::xrange(2), sg) {
    regs.fp[2] = sg ? -INFINITY : INFINITY;
    asm_m68k("fatanh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fatanh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.000001)) {
    regs.fp[2] = 0.5;
    asm_m68k("fatanh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.549306144334055);
}

BOOST_AUTO_TEST_CASE(dz) {
    regs.fp[2] = 1.0;
    asm_m68k("fatanh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isinf(regs.fp[3]));
    BOOST_TEST(regs.FPSR.dz);
}
BOOST_AUTO_TEST_CASE(operr) {
    regs.fp[2] = 2.0;
    asm_m68k("fatanh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
