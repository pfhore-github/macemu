#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(LSR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x80;
    write16(0, 0163012);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x80;
    write16(0, 0160012);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 2;
    regs.d[2] = 0x80;
    write16(0, 0163052);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x20);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 0x80;
    regs.c = regs.x = true;
    write16(0, 0163052);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x80);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    write16(0, 0162051);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 1;
    write16(0, 0161011);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161011);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x8000;
    write16(0, 0163112);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x1000);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x8000;
    write16(0, 0160112);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x0080);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 2;
    regs.d[2] = 0x8000;
    write16(0, 0163152);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x2000);
}


BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 0x8001;
    regs.c = regs.x = true;
    write16(0, 0163151);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x8001);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    write16(0, 0162151);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 1;
    write16(0, 0161111);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161111);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x8000);
    raw_write16(0, 0161322);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = 0x4000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x80000000;
    write16(0, 0163212);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x10000000);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x80000000;
    write16(0, 0160212);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x00800000);
}
BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 2;
    regs.d[2] = 0x80000000;
    write16(0, 0163252);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x20000000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 0x80000001;
    regs.c = regs.x = true;
    write16(0, 0160252);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x80000001);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    write16(0, 0162251);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 1;
    write16(0, 0161211);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161211);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()