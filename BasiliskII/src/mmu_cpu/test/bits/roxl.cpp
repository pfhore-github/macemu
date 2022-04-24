#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>



BOOST_FIXTURE_TEST_SUITE(ROXL, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0xc3;
    asm_m68k("roxlb #3, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1f );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x40;
    asm_m68k("roxlb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x20 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0xc3;
    regs.d[2] = 3;
    asm_m68k("roxlb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}
BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 32;
    regs.d[2] = 0;
    asm_m68k("roxlb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 32 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0xc3;
    regs.d[2] = 67;
    asm_m68k("roxlb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0xc003;
    asm_m68k("roxlw #3, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x001F );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x10;
    asm_m68k("roxlw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0xc003;
    regs.d[2] = 3;
    asm_m68k("roxlw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    asm_m68k("roxlw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0xc003;
    regs.d[2] = 67;
    asm_m68k("roxlw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    asm_m68k("roxlw (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.x = true;
    regs.d[1] = 0xc0000003;
    asm_m68k("roxll #3, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1f );
}
BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 256;
    asm_m68k("roxll #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000 );
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0xc0000003;
    regs.d[2] = 3;
    asm_m68k("roxll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b );
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    asm_m68k("roxll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000000 );
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    asm_m68k("roxll %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
