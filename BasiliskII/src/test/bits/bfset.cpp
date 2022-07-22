#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(BFSET, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(range) {
    regs.d[2] = 0;
    raw_write16(0, 0167302);
    raw_write16(2, 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x0ff00000);
}

BOOST_AUTO_TEST_CASE(wrap) {
    regs.d[2] = 0;
    raw_write16(0, 0167302);
    raw_write16(2, 24 << 6 | 16);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff0000ff);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x13);
    raw_write16(0, 0167321);
    raw_write16(2, 4 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write16(0, 0167321);
    raw_write16(2, 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
    BOOST_TEST(raw_read8(0x11) == 0xf3);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write16(0, 0167321);
    raw_write16(2, 4 << 6 | 16);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
    BOOST_TEST(raw_read8(0x11) == 0xff);
    BOOST_TEST(raw_read8(0x12) == 0xf5);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    raw_write16(0, 0167321);
    raw_write16(2, 4 << 6 | 24);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
    BOOST_TEST(raw_read8(0x11) == 0xff);
    BOOST_TEST(raw_read8(0x12) == 0xff);
    BOOST_TEST(raw_read8(0x13) == 0xf7);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    raw_write8(0x14, 0x89);
    raw_write16(0, 0167321);
    raw_write16(2, 4 << 6 | 32);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
    BOOST_TEST(raw_read8(0x11) == 0xff);
    BOOST_TEST(raw_read8(0x12) == 0xff);
    BOOST_TEST(raw_read8(0x13) == 0xff);
    BOOST_TEST(raw_read8(0x14) == 0xf9);
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x13);
    raw_write16(0, 0167321);
    raw_write16(2, 1 << 11 | 2 << 6 | 4);

    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1f);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x13);
    raw_write16(0, 0167321);
    raw_write16(2, 12 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x11) == 0x1f);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
