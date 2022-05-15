#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ROXL, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8) * BIT, n, x) {
    auto dy = rand_reg();
    auto v1 = get_v8();
    regs.x = x;
    regs.d[dy] = v1;
    uint16_t expected = x << 8 | v1;
    int nn = n ? n : 8;
    expected = (expected << nn | expected >> (9 - nn)) & 0xff;
    write16(0, 0160420 | n << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == expected);
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [dx, dy] = rand_reg2();
    auto v1 = get_v8();
    regs.x = true;
    int n = get_vn(1, 7);
    regs.d[dy] = v1;
    regs.d[dx] = n;
    write16(0, 0160460 | dx << 9 | dy);
    m68k_do_execute();
    uint16_t expected = 0x100 | v1;
    expected = (expected << n | expected >> (9 - n)) & 0xff;
    BOOST_TEST(regs.d[dy] == expected);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0xc3;
    regs.d[2] = 67;
    write16(0, 0160460 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1b);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0x80;
    write16(0, 0161421);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(c_0) {
    regs.x = true;
    regs.c = false;
    regs.d[2] = 0xff;
    regs.d[1] = 0;
    write16(0, 0161462);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40;
    write16(0, 0161421);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161421);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8) * BIT, n, x) {
    auto dy = rand_reg();
    auto v1 = get_v16();
    regs.x = x;
    regs.d[dy] = v1;
    uint32_t expected = x << 16 | v1;
    int nn = n ? n : 8;
    expected = (expected << nn | expected >> (17 - nn)) & 0xffff;
    write16(0, 0160520 | n << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == expected);
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [dx, dy] = rand_reg2();
    auto v1 = get_v16();
    regs.x = true;
    int n = get_vn(1, 15);
    regs.d[dy] = v1;
    regs.d[dx] = n;
    write16(0, 0160560 | dx << 9 | dy);
    m68k_do_execute();
    uint32_t expected = 0x10000 | v1;
    expected = (expected << n | expected >> (17 - n)) & 0xffff;
    BOOST_TEST(regs.d[dy] == expected);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.x = true;
    regs.d[1] = 0x1234;
    regs.d[2] = 67;
    write16(0, 0160560 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x91A4);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0x8000;
    write16(0, 0161521);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(c_0) {
    regs.x = true;
    regs.c = false;
    regs.d[2] = 0xffff;
    regs.d[1] = 0;
    write16(0, 0161562);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x4000;
    write16(0, 0161521);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x8000;
    write16(0, 0161521);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}


BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    raw_write16(0, 0162722);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)


BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8) * BIT, n, x) {
    auto dy = rand_reg();
    auto v1 = get_v32();
    regs.x = x;
    regs.d[dy] = v1;
    uint64_t expected = (uint64_t)x << 32 | (uint64_t)v1;
    int nn = n ? n : 8;
    expected = (expected << nn | expected >> (33 - nn)) & 0xffffffff;
    write16(0, 0160620 | n << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == expected);
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [dx, dy] = rand_reg2();
    auto v1 = get_v32();
    regs.x = true;
    int n = get_vn(1, 31);
    regs.d[dy] = v1;
    regs.d[dx] = n;
    write16(0, 0160660 | dx << 9 | dy);
    m68k_do_execute();
    uint64_t expected = 0x100000000ll | v1;
    expected = (expected << n | expected >> (33 - n)) & 0xffffffff;
    BOOST_TEST(regs.d[dy] == expected);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.x = true;
    regs.d[1] = 0x12345678;
    regs.d[2] = 67;
    write16(0, 0160660 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x91A2B3C4);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0x80000000;
    write16(0, 0161621);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(c_0) {
    regs.x = true;
    regs.c = false;
    regs.d[2] = 0xffffffff;
    regs.d[1] = 0;
    write16(0, 0161662);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40000000;
    write16(0, 0161621);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x80000000;
    write16(0, 0161621);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
