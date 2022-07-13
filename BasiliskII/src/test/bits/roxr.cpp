#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ROXR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(Imm, BIT, x) {
    regs.x = x;
    regs.d[1] = 0x81;
    write16(0, 0162021);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0xA0 | (x << 6)));
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.x = true;
    regs.d[1] = 0x81;
    write16(0, 0160021);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x3);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.x = true;
    regs.d[1] = 0x81;
    regs.d[3] = 2;
    write16(0, 0163061);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xE0);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.x = true;
    regs.c = false;
    regs.d[1] = 0x81;
    regs.d[3] = 0;
    write16(0, 0163061);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x81);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0xc3;
    regs.d[2] = 67;
    write16(0, 0160060 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xD8);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 1;
    write16(0, 0161021);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.x = true;
    regs.d[1] = 1;
    write16(0, 0161021);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161021);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_DATA_TEST_CASE(Imm, BIT, x) {
    regs.x = x;
    regs.d[1] = 0x8001;
    write16(0, 0162121);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0xA000 | (x << 14)));
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.x = true;
    regs.d[1] = 0x8001;
    write16(0, 0160121);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x380);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.x = true;
    regs.d[1] = 0x8001;
    regs.d[3] = 2;
    write16(0, 0163161);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xE000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.x = true;
    regs.c = false;
    regs.d[1] = 0x8001;
    regs.d[3] = 0;
    write16(0, 0163161);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8001);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.x = true;
    regs.d[1] = 0x1234;
    regs.d[2] = 67;
    write16(0, 0160160 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2246);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 1;
    write16(0, 0161121);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.x = true;
    regs.d[1] = 1;
    write16(0, 0161121);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    write16(0, 0161121);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x8000);
    raw_write16(0, 0162322);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = 0x4000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_DATA_TEST_CASE(Imm, BIT, x) {
    regs.x = x;
    regs.d[1] = 0x80000001;
    write16(0, 0162221);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0xA0000000 | (x << 30)));
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.x = true;
    regs.d[1] = 0x80000001;
    write16(0, 0160221);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x3800000);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.x = true;
    regs.d[1] = 0x80000001;
    regs.d[3] = 2;
    write16(0, 0163261);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xE0000000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.x = true;
    regs.c = false;
    regs.d[1] = 0x80000001;
    regs.d[3] = 0;
    write16(0, 0163261);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80000001);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.x = true;
    regs.d[1] = 0x12345678;
    regs.d[2] = 67;
    write16(0, 0160260 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x22468ACF);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 1;
    write16(0, 0161221);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.x = true;
    regs.d[1] = 1;
    write16(0, 0161221);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    write16(0, 0161221);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()