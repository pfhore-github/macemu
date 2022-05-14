
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(SUB, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.d[1] = 0x12345600 | 66;
    regs.d[2] = 25;
    asm_m68k("subb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345600 | (66 - 25)));
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 0x12345600 | 66;
    raw_write8(0x10, 25);
    regs.a[2] = 0x10;
    asm_m68k("subb (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345600 | (66 - 25)));
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.d[1] = 25;
    raw_write8(0x10, 66);
    regs.a[2] = 0x10;
    asm_m68k("subb %D1, (%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 66 - 25 );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(RegToReg) {

    regs.d[1] = 0x12340000 | 3456;
    regs.d[2] = 1000;
    asm_m68k("subw %D2, %D1");

    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12340000 | 2456));
}


BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 0x12340000 | 3456;
    raw_write16(0x10, 1000);
    regs.a[2] = 0x10;
    asm_m68k("subw (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x12340000 | 2456);
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.d[1] = 2500;
    raw_write16(0x10, 6600);
    regs.a[2] = 0x10;
    asm_m68k("subw %D1, (%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 6600 - 2500 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.d[1] = 123456789;
    regs.d[2] = 100000000;
    asm_m68k("subl %D2, %D1");

    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 23456789);
}



BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 123456789;
    regs.a[2] = 0x10;
    raw_write32(0x10, 100000000);

    asm_m68k("subl (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 23456789);
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.d[1] = 250000;
    raw_write32(0x10, 660000);
    regs.a[2] = 0x10;
    asm_m68k("subl %D1, (%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 660000 - 250000 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
