#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(NEG, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 44;
    asm_m68k("negb %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 256 - 44);
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 44);
    asm_m68k("negb (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 256 - 44);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 440;
    asm_m68k("negw %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 65536 - 440);
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    raw_write16(0x10, 440);
    asm_m68k("negw (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 65536 - 440);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 440000;
    asm_m68k("negl %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -440000);
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    raw_write32(0x10, 44000);
    asm_m68k("negl (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == -44000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()