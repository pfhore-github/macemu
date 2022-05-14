#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>

BOOST_FIXTURE_TEST_SUITE(BFEXTS, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(both_imm) {
    auto [xr, yr] = rand_reg2();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    regs.d[xr] = mask;
    write16(0, 0165700 | xr);
    write16(2, yr << 12 | off << 6 | w);
    m68k_do_execute();
    BOOST_TEST(regs.d[yr] == -1);
}

BOOST_AUTO_TEST_CASE(width_reg) {
    auto [xr, yr, zr] = rand_reg3();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    regs.d[xr] = mask;
    regs.d[yr] = w;
    write16(0, 0165700 | xr);
    write16(2, zr << 12 | 1 << 5 | off << 6 | yr);
    m68k_do_execute();
    BOOST_TEST(regs.d[zr] == -1);
}

BOOST_AUTO_TEST_CASE(off_reg) {
    auto [xr, yr, zr] = rand_reg3();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    regs.d[xr] = mask;
    regs.d[yr] = off;
    write16(0, 0165700 | xr);
    write16(2, zr << 12 | 1 << 11 | yr << 6 | w);
    m68k_do_execute();
    BOOST_TEST(regs.d[zr] == -1);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    write16(0, 0165721);
    write16(2, 2 << 12 | 4 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == -1);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xF3);
    write16(0, 0165721);
    write16(2, 3 << 12 | 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xFF);
    raw_write8(0x12, 0xF5);
    write16(0, 0165721);
    write16(2, 3 << 12 | 4 << 6 | 16);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_CASE(byte4) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xFF);
    raw_write8(0x12, 0xFF);
    raw_write8(0x13, 0xF7);
    write16(0, 0165721);
    write16(2, 3 << 12 | 4 << 6 | 24);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_CASE(byte5) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x1F);
    raw_write8(0x11, 0xFF);
    raw_write8(0x12, 0xFF);
    raw_write8(0x13, 0xFF);
    raw_write8(0x14, 0xF9);
    write16(0, 0165721);
    write16(2, 3 << 12 | 4 << 6 | 32);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}
BOOST_AUTO_TEST_CASE(neg_offset) {
    regs.a[1] = 0x11;
    regs.d[2] = -4;
    raw_write8(0x10, 0x1F);
    write16(0, 0165721);
    write16(2, 3 << 12 | 1 << 11 | 2 << 6 | 4);

    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x1F);
    write16(0, 0165721);
    write16(2, 3 << 12 | 12 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()