#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(AND, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[1] = 0xff;
    regs.d[2] = 0x2f;
    raw_write16(0, 0141002);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2f);
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
    BOOST_TEST( !regs.n);
    BOOST_TEST( !regs.z);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[1] = 0x2f;
    raw_write16(0, 0141422);
    raw_write8(0x10, 0xff);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x2f);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xff;
    regs.d[2] = 0x80;
    raw_write16(0, 0141002);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x80);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0xff;
    raw_write16(0, 0141002);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[1] = 0xffff;
    regs.d[2] = 0x2fff;
    raw_write16(0, 0141102);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2fff);
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
    BOOST_TEST( !regs.n);
    BOOST_TEST( !regs.z);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[1] = 0x2fff;
    raw_write16(0, 0141522);
    raw_write16(0x10, 0xffff);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0x2fff);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xffff;
    regs.d[2] = 0x8000;
    raw_write16(0, 0141102);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0xffff;
    raw_write16(0, 0141102);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[1] = 0xffffffff;
    regs.d[2] = 0x2fffffff;
    raw_write16(0, 0141202);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2fffffff);
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
    BOOST_TEST( !regs.n);
    BOOST_TEST( !regs.z);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[1] = 0x2fffffff;
    raw_write16(0, 0141622);
    raw_write32(0x10, 0xffffffff);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0x2fffffff);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xffffffff;
    regs.d[2] = 0x80000000;
    raw_write16(0, 0141202);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0xffffffff;
    raw_write16(0, 0141202);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
