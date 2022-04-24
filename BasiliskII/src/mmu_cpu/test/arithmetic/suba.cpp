
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>


BOOST_FIXTURE_TEST_SUITE(SUBA, InitFix)

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(WithReg) {
    regs.a[1] = 3456;
    regs.d[2] = 1000;
    asm_m68k("subaw %D2, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 2456);
}

BOOST_AUTO_TEST_CASE(WithMem) {
    regs.a[1] = 3456;
    raw_write16(0x10, 1000);
    regs.a[2] = 0x10;
    asm_m68k("subaw (%A2), %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 2456);
}

BOOST_AUTO_TEST_CASE(WithImm) {
    regs.a[1] = 3456;
    asm_m68k("subaw #1000, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 2456);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(WithReg) {
    regs.a[1] = 123456789;
    regs.d[2] = 100000000;
    asm_m68k("subal %D2, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 23456789);
}

BOOST_AUTO_TEST_CASE(WithMem) {
    regs.a[1] = 123456789;
    regs.a[2] = 0x10;
    raw_write32(0x10, 100000000);
    asm_m68k("subal (%A2), %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 23456789);
}

BOOST_AUTO_TEST_CASE(WithImm) {
    regs.a[1] = 123456789;
    asm_m68k("subal #100000000, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 23456789);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
