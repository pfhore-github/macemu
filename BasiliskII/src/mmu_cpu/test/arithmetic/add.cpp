#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>


BOOST_FIXTURE_TEST_SUITE(ADD, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.d[1] = 33;
    regs.d[2] = 25;
    asm_m68k("addb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 58);
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 33;
    raw_write8(0x10, 25);
    regs.a[2] = 0x10;
    asm_m68k("addb (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 58);
}

BOOST_AUTO_TEST_CASE(ImmToReg) {
    regs.d[1] = 33;
    asm_m68k("addb #25, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 58);
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.a[1] = 0x10;
    regs.d[2] = 25;
    asm_m68k("addb %D2, (%A1)");
    raw_write8(0x10, 66);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 91);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.d[1] = 3456;
    regs.d[2] = 1000;
    asm_m68k("addw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 4456);
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 3456;
    raw_write16(0x10, 1000);
    regs.a[2] = 0x10;
    asm_m68k("addw (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 4456);
}

BOOST_AUTO_TEST_CASE(ImmToReg) {
    regs.d[1] = 3456;
    asm_m68k("addw #1000, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 4456);
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.d[2] = 1000;
    regs.a[1] = 0x10;
    asm_m68k("addw %D2, (%A1)");
    raw_write16(0x10, 3456);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 4456);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.d[1] = 123456789;
    regs.d[2] = 100000000;
    asm_m68k("addl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 223456789);
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 123456789;
    regs.a[2] = 0x10;
    raw_write32(0x10, 100000000);
    asm_m68k("addl (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 223456789);
}

BOOST_AUTO_TEST_CASE(ImmToReg) {
    regs.d[1] = 123456789;
    asm_m68k("addl #100000000, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 223456789);
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.a[1] = 0x10;
    regs.d[2] = 100000000;
    asm_m68k("addl %D2, (%A1)");
    raw_write32(0x10, 123456789);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 223456789);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

