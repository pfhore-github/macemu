
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(SUBI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 66;
    asm_m68k("subib #25, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 41);
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    asm_m68k("subib #25, (%A1)");
    raw_write8(0x10, 66);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 41);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 3456;
    asm_m68k("subiw #1000, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 2456);
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    asm_m68k("subiw #1000, (%A1)");
    raw_write16(0x10, 3456);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 2456);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 123456789;
    asm_m68k("subil #100000000, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 23456789);
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    asm_m68k("subil #100000000, (%A1)");
    raw_write32(0x10, 123456789);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 23456789);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()