#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(FDBcc, InitFix)
BOOST_AUTO_TEST_CASE(t) {
    asm_m68k("fdbt %D1, DONE\nnop\nDONE:\nnop");
    m68k_do_execute();
    BOOST_TEST(regs.pc == 6);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.d[1] = 2;
    asm_m68k("fdbf %D1, DONE\nnop\nDONE:\nnop");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 1);
    BOOST_TEST(regs.pc == 8);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.d[1] = 0;
    asm_m68k("fdbf %D1, DONE\nnop\nDONE:\nnop");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff);
    BOOST_TEST(regs.pc == 6);
}
BOOST_AUTO_TEST_SUITE_END()