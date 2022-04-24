
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(SUBQ, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(reg)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[1] = 33;
    asm_m68k("subqb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 31);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.d[1] = 33;
    asm_m68k("subqb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 25);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(mem)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 33);
    asm_m68k("subqb #2, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 31);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 33);
    asm_m68k("subqb #2, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 31);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(Dreg)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[1] = 3456;
    asm_m68k("subqw #1, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 3455);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.d[1] = 3456;
    asm_m68k("subqw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 3448);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Areg)
BOOST_AUTO_TEST_CASE(value) {
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x1234;

    asm_m68k("subqw #1, %A1");

    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1233);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.a[1] = 0x1234;
    asm_m68k("subqw #8, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == (0x1234 - 8));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(mem)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 0x10;
    raw_write16(0x10, 3300);
    asm_m68k("subqw #2, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 3298);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.a[1] = 0x10;
    raw_write16(0x10, 3300);
    asm_m68k("subqw #8, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 3292);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(Dreg)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[1] = 123456789;
    asm_m68k("subql #1, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 123456788);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.d[1] = 123456789;
    asm_m68k("subql #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 123456781);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Areg)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 0x20000002;

    asm_m68k("subql #1, %A1");

    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x20000001);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x20000008;

    asm_m68k("subql #8, %A1");

    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x20000000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(mem)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 0x10;
    raw_write32(0x10, 330000);
    asm_m68k("subql #2, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 329998);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.a[1] = 0x10;
    raw_write32(0x10, 330000);
    asm_m68k("subql #8, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 329992);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
