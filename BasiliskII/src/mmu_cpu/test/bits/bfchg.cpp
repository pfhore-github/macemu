#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>
BOOST_FIXTURE_TEST_SUITE(BFCHG, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(both_imm) {
    auto ea = rand_reg();
    auto v1 = get_v32();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    regs.d[ea] = v1;
    write16(0, 0165300 | ea);
    write16(2, off << 6 | w);
    m68k_do_execute();
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    BOOST_TEST(regs.d[ea] == (v1 ^ mask));
}

BOOST_AUTO_TEST_CASE(width_reg) {
    auto [ea, wr] = rand_reg2();
    auto v1 = get_v32();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    regs.d[ea] = v1;
    regs.d[wr] = w;
    write16(0, 0165300 | ea);
    write16(2, 1 << 5 | off << 6 | wr);
    m68k_do_execute();
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    BOOST_TEST(regs.d[ea] == (v1 ^ mask));
}

BOOST_AUTO_TEST_CASE(off_reg) {
    auto [ea, ofr] = rand_reg2();
    auto v1 = get_v32();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    regs.d[ea] = v1;
    regs.d[ofr] = off;
    write16(0, 0165300 | ea);
    write16(2, 1 << 11 | ofr << 6 | w);
    m68k_do_execute();
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    BOOST_TEST(regs.d[ea] == (v1 ^ mask));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(byte1) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x13);
    write16(0, 0165321);
    write16(2, 4 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1C);
}

BOOST_AUTO_TEST_CASE(byte2) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    write16(0, 0165321);
    write16(2, 4 << 6 | 8);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1e);
    BOOST_TEST(raw_read8(0x11) == 0xD3);
}

BOOST_AUTO_TEST_CASE(byte3) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x11);
    raw_write8(0x11, 0x23);
    raw_write8(0x12, 0x45);
    write16(0, 0165321);
    write16(2, 4 << 6 | 16);
    m68k_do_execute();
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
    write16(0, 0165321);
    write16(2, 4 << 6 | 24);
    m68k_do_execute();
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
    write16(0, 0165321);
    write16(2, 4 << 6 | 32);
    m68k_do_execute();
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
    write16(0, 0165321);
    write16(2, 1 << 11 | 2 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x1C);
}

BOOST_AUTO_TEST_CASE(ex_offset) {
    regs.a[1] = 0x10;
    raw_write8(0x11, 0x13);
    write16(0, 0165321);
    write16(2, 12 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x11) == 0x1c);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()