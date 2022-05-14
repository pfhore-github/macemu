#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>

BOOST_FIXTURE_TEST_SUITE(BFINS, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(both_imm) {
    auto [xr, yr] = rand_reg2();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[xr] = v1;
    regs.d[yr] = v2;
    write16(0, 0167700 | xr);
    write16(2, yr << 12 | off << 6 | w);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == ((v1 & ~mask) | (std::rotr(v2, off + w) & mask)));
}

BOOST_AUTO_TEST_CASE(width_reg) {
    auto [xr, yr, zr] = rand_reg3();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[xr] = v1;
    regs.d[zr] = v2;
    regs.d[yr] = w;
    write16(0, 0167700 | xr);
    write16(2, zr << 12 | 1 << 5 | off << 6 | yr);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == ((v1 & ~mask) | (std::rotr(v2, off + w) & mask)));
}

BOOST_AUTO_TEST_CASE(off_reg) {
    auto [xr, yr, zr] = rand_reg3();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[xr] = v1;
    regs.d[zr] = v2;
    regs.d[yr] = off;
    write16(0, 0167700 | xr);
    write16(2, zr << 12 | 1 << 11 | yr << 6 | w);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == ((v1 & ~mask) | (std::rotr(v2, off + w) & mask)));
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x12345678;
    regs.d[3] = 0x80;
    write16(0, 0167701);
    write16(2, 3 << 12 | 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x12345678;
    regs.d[3] = 0;
    write16(0, 0167701);
    write16(2, 3 << 12 | 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    regs.d[3] = 0x07;
    raw_write8(0x10, 0x11);
    write16(0, 0167721);
    write16(2, 3 << 12 | 4 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) = 0x17);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    regs.d[3] = 0xab;
    write16(0, 0167721);
    write16(2, 3 << 12 | 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1a);
    BOOST_TEST(raw_read8(0x11) == 0xb4);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    raw_write8(0x11, 0x34);
    raw_write8(0x12, 0x56);
    regs.d[3] = 0xabcd;
    write16(0, 0167721);
    write16(2, 3 << 12 | 4 << 6 | 16);
    m68k_do_execute();
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
    write16(0, 0167721);
    write16(2, 3 << 12 | 4 << 6 | 24);
    m68k_do_execute();
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
    write16(0, 0167721);
    write16(2, 3 << 12 | 4 << 6 | 32);
    m68k_do_execute();
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
    write16(0, 0167721);
    write16(2, 3 << 12 | 1 << 11 | 2 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) = 0x14);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x19);
    regs.d[3] = 0x7;
    write16(0, 0167721);
    write16(2, 3 << 12 | 12 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x11) == 0x17);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()