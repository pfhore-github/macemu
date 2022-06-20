#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>

BOOST_FIXTURE_TEST_SUITE(BFFFO, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_SUITE(range)

BOOST_AUTO_TEST_CASE(exist) {
    regs.d[2] = 0xf01fffff;
    write16(0, 0166702);
    write16(2, 3 << 12 | 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 11);
}

BOOST_AUTO_TEST_CASE(non_exist) {
    regs.d[2] = 0xf00fffff;
    write16(0, 0166702);
    write16(2, 3 << 12 | 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 12);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(wrap)
BOOST_AUTO_TEST_CASE(exist) {
    regs.d[2] = 0x01ffff00;
    write16(0, 0166702);
    write16(2, 3 << 12 | 24 << 6 | 16);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 39);
}

BOOST_AUTO_TEST_CASE(non_exist) {
    regs.d[2] = 0x00ffff00;
    write16(0, 0166702);
    write16(2, 3 << 12 | 24 << 6 | 16);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 40);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    write16(0, 0166721);
    write16(2, 3 << 12 | 4 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = 7);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x13);
    write16(0, 0166721);
    write16(2, 3 << 12 | 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 11);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x00);
    raw_write8(0x12, 0x15);
    write16(0, 0166721);
    write16(2, 3 << 12 | 4 << 6 | 16);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 19);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x00);
    raw_write8(0x12, 0x00);
    raw_write8(0x13, 0x17);
    write16(0, 0166721);
    write16(2, 3 << 12 | 4 << 6 | 24);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 27);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x00);
    raw_write8(0x12, 0x00);
    raw_write8(0x13, 0x00);
    raw_write8(0x14, 0x19);
    write16(0, 0166721);
    write16(2, 3 << 12 | 4 << 6 | 32);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = 35);
}
BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x1F);
    write16(0, 0166721);
    write16(2, 3 << 12 | 1 << 11 | 2 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] = -1);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x11);
    write16(0, 0166721);
    write16(2, 3 << 12 | 12 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 15);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()