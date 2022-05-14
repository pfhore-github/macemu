#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FCOSH, InitFix)
BOOST_DATA_TEST_CASE(zero, bdata::xrange(2), sg) {
    regs.fp[2] = sg ? -0.0 : 0.0;
    asm_m68k("fcosh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1.0);
}

BOOST_DATA_TEST_CASE(inf, bdata::xrange(2), sg) {
    regs.fp[2] = sg ? -INFINITY : INFINITY;
    asm_m68k("fcosh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(!signbit(regs.fp[3]));
    BOOST_TEST(isinf(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(nan_) {
    regs.fp[2] = NAN;
    asm_m68k("fcosh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(isnan(regs.fp[3]));
}

BOOST_AUTO_TEST_CASE(value, *boost::unit_test::tolerance(0.000001)) {
    regs.fp[2] = 2.3;
    asm_m68k("fcosh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 5.03722064926876100799);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    regs.fp[2] = 1e200;
    asm_m68k("fcosh.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == INFINITY);
    BOOST_TEST(regs.FPSR.ovfl);
}

BOOST_AUTO_TEST_SUITE_END()
