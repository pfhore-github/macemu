#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(BFCHG, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(range) {
    regs.d[2] = 0xAAAAAAAA;
    raw_write16(0, 0165302);
    raw_write16(2, 4 << 6 | 16);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0xA5555AAA);
}

BOOST_AUTO_TEST_CASE(wrap) {
    regs.d[2] = 0xAAAAAAAA;
    raw_write16(0, 0165302);
    raw_write16(2, 24 << 6 | 16);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0x55AAAA55);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x13);
    raw_write16(0, 0165321);
    raw_write16(2, 4 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1C);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write16(0, 0165321);
    raw_write16(2, 4 << 6 | 8);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1e);
    BOOST_TEST(raw_read8(0x11) == 0xD3);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write16(0, 0165321);
    raw_write16(2, 4 << 6 | 16);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1e);
    BOOST_TEST(raw_read8(0x11) == 0xDc);
    BOOST_TEST(raw_read8(0x12) == 0xb5);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    raw_write16(0, 0165321);
    raw_write16(2, 4 << 6 | 24);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1e);
    BOOST_TEST(raw_read8(0x11) == 0xDc);
    BOOST_TEST(raw_read8(0x12) == 0xba);
    BOOST_TEST(raw_read8(0x13) == 0x97);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    raw_write8(0x13, 0x67);
    raw_write8(0x14, 0x89);
    raw_write16(0, 0165321);
    raw_write16(2, 4 << 6 | 32);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1e);
    BOOST_TEST(raw_read8(0x11) == 0xDc);
    BOOST_TEST(raw_read8(0x12) == 0xba);
    BOOST_TEST(raw_read8(0x13) == 0x98);
    BOOST_TEST(raw_read8(0x14) == 0x79);
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x13);
    raw_write16(0, 0165321);
    raw_write16(2, 1 << 11 | 2 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1C);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x13);
    raw_write16(0, 0165321);
    raw_write16(2, 12 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x11) == 0x1c);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()