#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(CMPM, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(z) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write8(0x10, 10);
    raw_write8(0x20, 10);
    raw_write16(0, 0131412);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.a[1] == 0x11);
    BOOST_TEST(regs.a[2] == 0x21);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write8(0x10, 10);
    raw_write8(0x20, 20);
    raw_write16(0, 0131412);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write8(0x10, 0x80);
    raw_write8(0x20, 1);
    raw_write16(0, 0131412);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write8(0x10, 0);
    raw_write8(0x20, 1);
    raw_write16(0, 0131412);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(z) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write16(0x10, 1000);
    raw_write16(0x20, 1000);
    raw_write16(0, 0131512);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.a[1] == 0x12);
    BOOST_TEST(regs.a[2] == 0x22);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write16(0x10, 1000);
    raw_write16(0x20, 2000);
    raw_write16(0, 0131512);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write16(0x10, 0x8000);
    raw_write16(0x20, 1);
    raw_write16(0, 0131512);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write16(0x10, 0);
    raw_write16(0x20, 1);
    raw_write16(0, 0131512);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(z) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write32(0x10, 100000);
    raw_write32(0x20, 100000);
    raw_write16(0, 0131612);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(regs.a[2] == 0x24);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write32(0x10, 100000);
    raw_write32(0x20, 200000);
    raw_write16(0, 0131612);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write32(0x10, 0x80000000);
    raw_write32(0x20, 1);
    raw_write16(0, 0131612);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.a[1] = 0x10;
    regs.a[2] = 0x20;
    raw_write32(0x10, 0);
    raw_write32(0x20, 1);
    raw_write16(0, 0131612);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
