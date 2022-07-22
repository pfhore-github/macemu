#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(LSL)
#if 0
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.v = true;
    regs.d[2] = 0x1;
    raw_write16(0, 0163412);
    regs.v = true;
    m68k_do_execute();
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.d[2] == 0x8);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x1;
    raw_write16(0, 0160412);
    regs.v = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 4;
    regs.d[2] = 1;
    raw_write16(0, 0163452);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 1;
    regs.c = regs.x = true;
    raw_write16(0, 0163452);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 1);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}
BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    raw_write16(0, 0160450 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 0x80;
    raw_write16(0, 0161411);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40;
    raw_write16(0, 0161411);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161411);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE(Word)
#if 0
BOOST_AUTO_TEST_CASE(Imm) {
    regs.v = true;
    regs.d[2] = 0x80;
    raw_write16(0, 0163512);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x400);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x1;
    raw_write16(0, 0160512);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 4;
    regs.d[2] = 0x80;
    raw_write16(0, 0163552);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x800);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[2] = 1;
    regs.c = regs.x = true;
    raw_write16(0, 0163552);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 1);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    raw_write16(0, 0160550 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 0x8000;
    raw_write16(0, 0161511);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x4000;
    raw_write16(0, 0161511);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161511);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
#endif
BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    raw_write16(0, 0161722);
     jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()
#if 0
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x10000;
    raw_write16(0, 0163612);
    regs.v = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x80000);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[3] = 4;
    regs.d[1] = 0x10000;
    raw_write16(0, 0163651);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x100000);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[3] = 0;
    regs.d[1] = 0x10000;
    regs.c = regs.x = true;
    raw_write16(0, 0163651);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
}
BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    raw_write16(0, 0162651);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 128);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 0x80000000;
    raw_write16(0, 0161611);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx_0) {
    regs.c = regs.x = true;
    regs.d[2] = 0xffffffff;
    regs.d[1] = 0;
    raw_write16(0, 0161652);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40000000;
    raw_write16(0, 0161611);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161611);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()