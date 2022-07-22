#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(BFINS)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(range) {
    regs.d[2] = 0;
    regs.d[3] = 0x12;
    raw_write16(0, 0167702);
    raw_write16(2, 3 << 12 | 4 << 6 | 8);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0x01200000);
}

BOOST_AUTO_TEST_CASE(overwrap) {
    regs.d[2] = 0;
    regs.d[3] = 0x1234;
    raw_write16(0, 0167702);
    raw_write16(2, 3 << 12 | 24 << 6 | 16);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0x34000012);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x12345678;
    regs.d[3] = 0x80;
    raw_write16(0, 0167701);
    raw_write16(2, 3 << 12 | 4 << 6 | 8);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x12345678;
    regs.d[3] = 0;
    raw_write16(0, 0167701);
    raw_write16(2, 3 << 12 | 4 << 6 | 8);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    regs.d[3] = 0x07;
    raw_write8(0x10, 0x11);
    raw_write16(0, 0167721);
    raw_write16(2, 3 << 12 | 4 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) = 0x17);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    regs.d[3] = 0xab;
    raw_write16(0, 0167721);
    raw_write16(2, 3 << 12 | 4 << 6 | 8);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1a);
    BOOST_TEST(raw_read8(0x11) == 0xb4);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    raw_write8(0x12, 0x56);
    regs.d[3] = 0xabcd;
    raw_write16(0, 0167721);
    raw_write16(2, 3 << 12 | 4 << 6 | 16);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1a);
    BOOST_TEST(raw_read8(0x11) == 0xbc);
    BOOST_TEST(raw_read8(0x12) == 0xd6);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    raw_write8(0x12, 0x56);
    raw_write8(0x13, 0x78);
    regs.d[3] = 0xabcdef;
    raw_write16(0, 0167721);
    raw_write16(2, 3 << 12 | 4 << 6 | 24);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1a);
    BOOST_TEST(raw_read8(0x11) == 0xbc);
    BOOST_TEST(raw_read8(0x12) == 0xde);
    BOOST_TEST(raw_read8(0x13) == 0xf8);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    raw_write8(0x12, 0x56);
    raw_write8(0x13, 0x78);
    raw_write8(0x14, 0x9a);
    regs.d[3] = 0xabcdef01;
    raw_write16(0, 0167721);
    raw_write16(2, 3 << 12 | 4 << 6 | 32);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 0x1a);
    BOOST_TEST(raw_read8(0x11) == 0xbc);
    BOOST_TEST(raw_read8(0x12) == 0xde);
    BOOST_TEST(raw_read8(0x13) == 0xf0);
    BOOST_TEST(raw_read8(0x14) == 0x1a);
}

BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x1F);
    regs.d[3] = 0x4;
    raw_write16(0, 0167721);
    raw_write16(2, 3 << 12 | 1 << 11 | 2 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) = 0x14);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x19);
    regs.d[3] = 0x7;
    raw_write16(0, 0167721);
    raw_write16(2, 3 << 12 | 12 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x11) == 0x17);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()