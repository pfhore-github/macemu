#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(ASR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 32;
    asm_m68k("asrb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = -128;
    asm_m68k("asrb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -1);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 32;
    regs.d[2] = 2;
    asm_m68k("asrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("asrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("asrb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 3200;
    asm_m68k("asrw #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 800);
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x1000;
    asm_m68k("asrw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 16);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = -1024;
    regs.d[2] = 2;
    asm_m68k("asrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -256);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = -32;
    regs.d[2] = 0;
    asm_m68k("asrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -32);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("asrw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, -16);
    asm_m68k("asrw (%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = static_cast<uint16_t>(-8));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x10000;
    asm_m68k("asrl #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4000);
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x10000;
    asm_m68k("asrl #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 256);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = -40000;
    regs.d[2] = 2;
    asm_m68k("asrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -10000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = -32000;
    regs.d[2] = 0;
    asm_m68k("asrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -32000);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("asrl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()