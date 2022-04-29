#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(FTST, InitFix)
BOOST_DATA_TEST_CASE(n, bdata::xrange(2), value) {
    regs.fp[3] = value ? -2 : 2;
    asm_m68k("ftst.x %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.n == value);
}

BOOST_DATA_TEST_CASE(z, bdata::xrange(2), value) {
    regs.fp[3] = value ? 0 : 2;
    asm_m68k("ftst.x %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.z == value);
}

BOOST_DATA_TEST_CASE(inf, bdata::xrange(2), value) {
    regs.fp[3] = value ? INFINITY : 2;
    asm_m68k("ftst.x %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.i == value);
}

BOOST_DATA_TEST_CASE(nan_, bdata::xrange(2), value) {
    regs.fp[3] = value ? NAN : 2;
    asm_m68k("ftst.x %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.nan == value);
}

BOOST_AUTO_TEST_SUITE_END()
