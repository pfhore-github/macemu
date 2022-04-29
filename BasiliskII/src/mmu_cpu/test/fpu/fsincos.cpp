#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_ieee.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FSINCOS, InitFix)
BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    regs.fp[2] = copysign(0.0, sg);
    asm_m68k("fsincos.x %FP2, %FP4, %FP3");
    m68k_do_execute();
    BOOST_TEST(signbit(regs.fp[3]) == signbit(sg));
    BOOST_TEST(regs.fp[4] == 1.0);
    BOOST_TEST(regs.fp[3] == 0.0);
}
BOOST_DATA_TEST_CASE(inf, SIGN, sg) {
    regs.fp[2] = copysign(INFINITY, sg);
    asm_m68k("fsincos.x %FP2, %FP4, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[4]));
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(regs.FPSR.operr);
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fsincos.x %FP2, %FP4, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
    BOOST_TEST(isnan(regs.fp[4]));
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.000001)) {
    regs.fp[2] = 0.5;
    asm_m68k("fsincos.x %FP2, %FP4, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.479425538604203);
    BOOST_TEST(regs.fp[4] == 0.877582561890373);
}

BOOST_AUTO_TEST_SUITE_END()
