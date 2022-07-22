#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ASL, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.v = true;
    regs.d[2] = 0x1;
    raw_write16(0, 0163402);
    m68k_do_execute();
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.d[2] == 0x8);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x1;
    raw_write16(0, 0160402);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 4;
    regs.d[2] = 1;
    raw_write16(0, 0163442);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 1;
    regs.c = regs.x = true;
    raw_write16(0, 0163442);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 1);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 1;
    regs.d[2] = 66;
    raw_write16(0, 0162441);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 4);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 0xC0;
    raw_write16(0, 0161401);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[1] == 0x80);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40;
    raw_write16(0, 0161401);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x80);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x80;
    raw_write16(0, 0161401);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x60;
    raw_write16(0, 0162401);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.d[1] == 0x80);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x80;
    raw_write16(0, 0163502);
    m68k_do_execute();
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.d[2] == 0x400);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x1;
    raw_write16(0, 0160502);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 4;
    regs.d[2] = 0x80;
    raw_write16(0, 0163542);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x800);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 1;
    regs.c = regs.x = true;
    raw_write16(0, 0163542);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 1);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x80;
    regs.d[2] = 66;
    raw_write16(0, 0162541);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x200);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 0xC000;
    raw_write16(0, 0161501);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x4000;
    raw_write16(0, 0161501);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0161501);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x6000;
    raw_write16(0, 0162501);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0xffff);
    raw_write16(0, 0161722);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = 0xfffe);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x10000;
    raw_write16(0, 0163602);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x80000);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x100;
    raw_write16(0, 0160602);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x10000);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 4;
    regs.d[1] = 0x10000;
    raw_write16(0, 0163641);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x100000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[1] = 0x10000;
    regs.c = regs.x = true;
    raw_write16(0, 0163641);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    raw_write16(0, 0162641);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 0x80000000;
    raw_write16(0, 0161601);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40000000;
    raw_write16(0, 0161601);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161601);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x60000000;
    raw_write16(0, 0162601);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
