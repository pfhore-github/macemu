#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(ASR)
#if 0
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x80;
    raw_write16(0, 0163002);
    m68k_do_execute();
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.d[2] == 0xF0);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x80;
    raw_write16(0, 0160002);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 2;
    regs.d[2] = 0x80;
    raw_write16(0, 0163042);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xE0);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 0x80;
    regs.c = regs.x = true;
    raw_write16(0, 0163042);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x80);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}
BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x80;
    regs.d[2] = 66;
    raw_write16(0, 0162041);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xE0);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 0x81;
    raw_write16(0, 0161001);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[1] == 0xC0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80;
    raw_write16(0, 0161001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0xC0);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE(Word)
#if 0
BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x8000;
    raw_write16(0, 0163102);
    m68k_do_execute();
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.d[2] == 0xF000);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x8000;
    raw_write16(0, 0160102);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff80);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 2;
    regs.d[2] = 0x8000;
    raw_write16(0, 0163142);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xE000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 0x8001;
    regs.c = regs.x = true;
    raw_write16(0, 0163142);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x8001);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x8000;
    regs.d[2] = 66;
    raw_write16(0, 0162141);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xE000);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 0x8001;
    raw_write16(0, 0161101);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[1] == 0xC000);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0161101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0xC000);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    raw_write16(0, 0161101);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}
#endif
BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x8000);
    raw_write16(0, 0160322);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read16(0x10) = 0xC000);
}

BOOST_AUTO_TEST_SUITE_END()
#if 0
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x80000000;
    raw_write16(0, 0163202);
    m68k_do_execute();
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.d[2] == 0xF0000000);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x80000000;
    raw_write16(0, 0160202);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff800000);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 2;
    regs.d[2] = 0x80000000;
    raw_write16(0, 0163242);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xE0000000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 0x80000001;
    regs.c = regs.x = true;
    raw_write16(0, 0163242);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x80000001);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}
BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 66;
    raw_write16(0, 0162241);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xE0000000);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 0x80000001;
    raw_write16(0, 0161201);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[1] == 0xC0000000);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0161201);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0xC0000000);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    raw_write16(0, 0161201);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()