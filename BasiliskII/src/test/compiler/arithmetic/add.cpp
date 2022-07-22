#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ADD, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[2] = 33;
    regs.d[3] = 29;
    raw_write16(0, 0152003);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 62);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[3] = 0x10;
    regs.d[4] = 33;
    raw_write16(0, 0154423);
    raw_write8(0x10, 29);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 62);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[2] = 0xff;
    regs.d[1] = 2;
    raw_write16(0, 0152001);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[2] == 1);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[2] = 0x7f;
    regs.d[1] = 1;
    raw_write16(0, 0152001);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.d[2] == 0x80);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[2] = 0;
    regs.d[1] = 0;
    raw_write16(0, 0152001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[2] = 0x80;
    regs.d[1] = 1;
    raw_write16(0, 0152001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[2] == 0x81);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[2] = 1111;
    regs.d[3] = 2222;
    raw_write16(0, 0152103);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 3333);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[3] = 1111;
    raw_write16(0, 0153522);
    raw_write16(0x10, 2222);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 3333);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[2] = 0xffff;
    regs.d[1] = 2;
    raw_write16(0, 0152101);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[2] == 0x0001);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[2] = 0x7fff;
    regs.d[1] = 1;
    raw_write16(0, 0152101);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.d[2] == 0x8000);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[2] = 0;
    regs.d[1] = 0;
    raw_write16(0, 0152101);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[2] = 0x8000;
    regs.d[1] = 1;
    raw_write16(0, 0152101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[2] == 0x8001);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[1] = 111111;
    regs.d[2] = 222222;
    raw_write16(0, 0151202);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 333333);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[1] = 0x10;
    regs.d[2] = 111111;
    raw_write16(0, 0152621);
    raw_write32(0x10, 222222);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 333333);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[2] = 0xffffffff;
    regs.d[1] = 2;
    raw_write16(0, 0152201);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[2] == 1);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[2] = 0x7fffffff;
    regs.d[1] = 1;
    raw_write16(0, 0152201);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.d[2] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[2] = 0;
    regs.d[1] = 0;
    raw_write16(0, 0152201);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[2] = 0x80000000;
    regs.d[1] = 1;
    raw_write16(0, 0152201);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[2] == 0x80000001);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
