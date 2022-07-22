#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ROR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x31;
    raw_write16(0, 0162031);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4c);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x31;
    raw_write16(0, 0160031);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x31);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x31;
    regs.d[3] = 2;
    raw_write16(0, 0163071);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4c);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x31;
    regs.d[3] = 0;
    regs.c = true;
    raw_write16(0, 0163071);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x31);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x39;
    regs.d[2] = 68;
    raw_write16(0, 0162071);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 1;
    raw_write16(0, 0161031);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 1;
    raw_write16(0, 0161031);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161031);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x1234;
    raw_write16(0, 0164131);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4123);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x1234;
    raw_write16(0, 0160131);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x3412);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x1234;
    regs.d[2] = 4;
    raw_write16(0, 0162171);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4123);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x1234;
    regs.d[2] = 0;
    regs.c = true;
    raw_write16(0, 0162171);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1234);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x3456;
    regs.d[2] = 68;
    raw_write16(0, 0162171);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x6345);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 1;
    raw_write16(0, 0161131);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 1;
    raw_write16(0, 0161131);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161131);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(ea) {
    regs.a[1] = 0x10;
    raw_write16(0x10, 0x4000);
    raw_write16(0, 0163321);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x12345678;
    raw_write16(0, 0164232);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x81234567);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x12345678;
    raw_write16(0, 0160232);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x78123456);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x12345678;
    regs.d[2] = 4;
    raw_write16(0, 0162271);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x81234567);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x12345678;
    regs.d[2] = 0;
    regs.c = true;
    raw_write16(0, 0162271);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x12345678);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x3456789A;
    regs.d[2] = 68;
    raw_write16(0, 0162271);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xA3456789);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 1;
    raw_write16(0, 0161231);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 1;
    raw_write16(0, 0161231);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161231);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
