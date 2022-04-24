#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(LSL, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 32;
    asm_m68k("lslb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0xff;
    asm_m68k("lslb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 32;
    regs.d[2] = 2;
    asm_m68k("lslb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("lslb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("lslb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 320;
    asm_m68k("lslw #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 1280);
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 16;
    asm_m68k("lslw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1000);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x2000;
    regs.d[2] = 2;
    asm_m68k("lslw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    asm_m68k("lslw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("lslw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    asm_m68k("lslw (%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x10000;
    asm_m68k("lsll #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x40000);
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 256;
    asm_m68k("lsll #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x20000000;
    regs.d[2] = 2;
    asm_m68k("lsll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    asm_m68k("lsll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("lsll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()