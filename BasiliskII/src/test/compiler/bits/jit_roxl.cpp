#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(ROXL)
#if 0
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(Imm, BIT, x) {
    regs.x = x;
    regs.d[1] = 0x90;
    raw_write16(0, 0162421);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x41 | (x << 1)));
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.x = true;
    regs.d[1] = 0x3;
    raw_write16(0, 0160421);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x81);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.x = true;
    regs.d[1] = 0x10;
    regs.d[3] = 2;
    raw_write16(0, 0163461);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x42);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.x = true;
    regs.c = false;
    regs.d[1] = 0x10;
    regs.d[3] = 0;
    raw_write16(0, 0163461);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0xc3;
    regs.d[2] = 67;
    raw_write16(0, 0161461);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0x80;
    raw_write16(0, 0161421);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40;
    raw_write16(0, 0161421);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161421);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE(Word)
#if 0
BOOST_DATA_TEST_CASE(Imm, BIT, x) {
    regs.x = x;
    regs.d[1] = 0x8100;
    raw_write16(0, 0162521);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x401 | (x << 1)));
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.x = true;
    regs.d[1] = 0x8100;
    raw_write16(0, 0160521);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xC0);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.x = true;
    regs.d[1] = 0x8100;
    regs.d[3] = 2;
    raw_write16(0, 0163561);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x403);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.x = true;
    regs.c = false;
    regs.d[1] = 0x8100;
    regs.d[3] = 0;
    raw_write16(0, 0163561);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8100);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.x = true;
    regs.d[1] = 0x1234;
    regs.d[2] = 67;
    raw_write16(0, 0160560 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x91A4);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0161521);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x4000;
    raw_write16(0, 0161521);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0161521);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
#endif
BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    raw_write16(0, 0162722);
     jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()
#if 0
BOOST_AUTO_TEST_SUITE(Long)

BOOST_DATA_TEST_CASE(Imm, BIT, x) {
    regs.x = x;
    regs.d[1] = 0x80000001;
    raw_write16(0, 0162621);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x5 | (x << 1)));
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.x = true;
    regs.d[1] = 0x80000001;
    raw_write16(0, 0160621);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1C0);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.x = true;
    regs.d[1] = 0x80000001;
    regs.d[3] = 2;
    raw_write16(0, 0163661);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x7);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.x = true;
    regs.c = false;
    regs.d[1] = 0x80000001;
    regs.d[3] = 0;
    raw_write16(0, 0163661);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000001);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.x = true;
    regs.d[1] = 0x12345678;
    regs.d[2] = 67;
    raw_write16(0, 0160660 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x91A2B3C4);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0161621);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40000000;
    raw_write16(0, 0161621);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0161621);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
