#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(CMPI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 24;
    asm_m68k("cmpib #25, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.a[1] = 0x10;
    asm_m68k("cmpib #25, (%A1)");
    raw_write8(0x10, 24);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 1000;
    asm_m68k("cmpiw #1001, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.a[1] = 0x10;
    asm_m68k("cmpiw #1001, (%A1)");
    raw_write16(0x10, 1000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 100000000;
    asm_m68k("cmpil #100000001, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;

    asm_m68k("cmpil #100000001, (%A1)");
    raw_write32(0x10, 100000000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()