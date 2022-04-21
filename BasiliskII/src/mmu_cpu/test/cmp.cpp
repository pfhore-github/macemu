#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(CMP, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 24;
    regs.d[2] = 25;
    asm_m68k("cmpb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.d[1] = 24;
    regs.a[1] = 0x10;
    raw_write8(0x10, 25);
    asm_m68k("cmpb (%A1), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 999;
    regs.d[2] = 1000;
    asm_m68k("cmpw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.d[1] = 999;
    regs.a[1] = 0x10;
    asm_m68k("cmpw (%A1), %D1");
    raw_write16(0x10, 1000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[2] = 100000001;
    regs.d[1] = 100000000;
    asm_m68k("cmpl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.a[1] = 0x10;
    regs.d[1] = 99999999;
    asm_m68k("cmpl (%A1), %D1");
    raw_write32(0x10, 100000000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(CMPA, InitFix)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(reg) {
    regs.a[1] = 998;
    regs.a[2] = 1000;
    asm_m68k("cmpaw %A2, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.a[2] = 998;
    regs.a[1] = 0x10;
    asm_m68k("cmpaw (%A1), %A2");
    raw_write16(0x10, 1000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(reg) {
    regs.a[1] = 0x10000;
    regs.a[2] = 0x20000;
    asm_m68k("cmpal %A2, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.a[2] = 0x10000;
    regs.a[1] = 0x10;
    asm_m68k("cmpal (%A1), %A2");
    raw_write32(0x10, 0x20000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
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
