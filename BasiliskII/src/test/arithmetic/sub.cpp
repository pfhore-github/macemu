
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(SUB, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[2] = 50;
    regs.d[3] = 24;
    raw_write16(0, 0112003);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 26);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[1] = 24;
    raw_write16(0, 0111422);
    raw_write8(0x10, 50);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 26);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[0] = 1;
    regs.d[1] = 2;
    raw_write16(0, 0110001);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[0] = 0x80;
    regs.d[1] = 1;
    raw_write16(0, 0110001);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[0] = 1;
    regs.d[1] = 1;
    raw_write16(0, 0110001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[0] = 0xf3;
    regs.d[1] = 1;
    raw_write16(0, 0110001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[1] = 1000;
    regs.d[2] = 200;
    raw_write16(0, 0111102);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 800);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[3] = 200;
    raw_write16(0, 0113522);
    raw_write16(0x10, 1000);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 800);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[0] = 1;
    regs.d[1] = 2;
    raw_write16(0, 0110100 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[0] = 0x8000;
    regs.d[1] = 1;
    raw_write16(0, 0110100 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[0] = 1;
    regs.d[1] = 1;
    raw_write16(0, 0110100 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[0] = 0xfffe;
    regs.d[1] = 1;
    raw_write16(0, 0110100 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[3] = 100000;
    regs.d[2] = 40000;
    raw_write16(0, 0113202);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 60000);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[4] = 40000;
    raw_write16(0, 0114622);
    raw_write32(0x10, 100000);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 60000);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[0] = 1;
    regs.d[1] = 2;
    raw_write16(0, 0110200 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[0] = 0x80000000;
    regs.d[1] = 1;
    raw_write16(0, 0110200 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[0] = 1;
    regs.d[1] = 1;
    raw_write16(0, 0110200 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[0] = 0xfffffffe;
    regs.d[1] = 1;
    raw_write16(0, 0110200 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
