#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(ROL, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x39;
    asm_m68k("rolb #4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93);
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x40;
    asm_m68k("rolb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x40);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x39;
    regs.d[2] = 4;
    asm_m68k("rolb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93);
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("rolb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x39;
    regs.d[2] = 68;
    asm_m68k("rolb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x1234;
    asm_m68k("rolw #4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2341);
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x10;
    asm_m68k("rolw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1000);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x1234;
    regs.d[2] = 4;
    asm_m68k("rolw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2341);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    asm_m68k("rolw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x1234;
    regs.d[2] = 68;
    asm_m68k("rolw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2341);
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    asm_m68k("rolw (%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0x12345678;
    asm_m68k("roll #4, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x23456781);
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 256;
    asm_m68k("roll #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x12345678;
    regs.d[2] = 4;
    asm_m68k("roll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x23456781);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    asm_m68k("roll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("roll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
