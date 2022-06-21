#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(OR, InitFix)

BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[2] = 0x12;
    regs.d[3] = 0x23;
    raw_write16(0, 0102003);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x33);
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x12;
    raw_write16(0, 0103422);
    raw_write8(0x10, 0x23);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x33);
}


BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80;
    regs.d[2] = 0;
    raw_write16(0, 0101002);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0;
    raw_write16(0, 0101002);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[2] = 0x1234;
    regs.d[3] = 0xbeaf;
    raw_write16(0, 0102103);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xbebf);
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x1234;
    raw_write16(0, 0103522);
    raw_write16(0x10, 0xbeaf);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0xbebf);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    raw_write16(0, 0100100 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0;
    raw_write16(0, 0100100 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[1] = 0x12345678;
    regs.d[2] = 0xdeaddead;
    raw_write16(0, 0101202);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xdebddefd);
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    regs.a[2] = 0x10;
    regs.d[3] = 0x12345678;
    raw_write16(0, 0103622);
    raw_write32(0x10, 0xdeaddead);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0xdebddefd);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    raw_write16(0, 0100200 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0;
    raw_write16(0, 0100200 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()