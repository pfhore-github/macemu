#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(BFEXTU)
BOOST_AUTO_TEST_SUITE(REG)
BOOST_AUTO_TEST_SUITE(range)
BOOST_AUTO_TEST_CASE(positive) {
    regs.d[2] = 0x12345678;
    raw_write16(0, 0164702);
    raw_write16(2, 3 << 12 | 4 << 6 | 12);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0x234);
}

BOOST_AUTO_TEST_CASE(negative) {
    regs.d[2] = 0x89ABCDEF;
    raw_write16(0, 0164702);
    raw_write16(2, 3 << 12 | 4 << 6 | 12);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] ==0x9AB);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(wrap)
BOOST_AUTO_TEST_CASE(positive) {
    regs.d[2] = 0x12345678;
    raw_write16(0, 0164702);
    raw_write16(2, 3 << 12 | 24 << 6 | 12);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0x781);
}

BOOST_AUTO_TEST_CASE(negative) {
    regs.d[2] = 0x89ABCDEF;
    raw_write16(0, 0164702);
    raw_write16(2, 3 << 12 | 24 << 6 | 12);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] ==0xEF8);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write16(0, 0164721);
    raw_write16(2, 2 << 12 | 4 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0xf);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xF3);
    raw_write16(0, 0164721);
    raw_write16(2, 3 << 12 | 4 << 6 | 8);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0xff);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xFF);
    raw_write8(0x12, 0xF5);
    raw_write16(0, 0164721);
    raw_write16(2, 3 << 12 | 4 << 6 | 16);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0xffff);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xFF);
    raw_write8(0x12, 0xFF);
    raw_write8(0x13, 0xF7);
    raw_write16(0, 0164721);
    raw_write16(2, 3 << 12 | 4 << 6 | 24);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0xffffff);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xFF);
    raw_write8(0x12, 0xFF);
    raw_write8(0x13, 0xFF);
    raw_write8(0x14, 0xF9);
    raw_write16(0, 0164721);
    raw_write16(2, 3 << 12 | 4 << 6 | 32);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0xffffffff);
}
BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x1F);
    raw_write16(0, 0164721);
    raw_write16(2, 3 << 12 | 1 << 11 | 2 << 6 | 4);

    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0xf);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x1F);
    raw_write16(0, 0164721);
    raw_write16(2, 3 << 12 | 12 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0xf);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()