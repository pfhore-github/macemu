#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>

BOOST_FIXTURE_TEST_SUITE(BFFFO, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(both_imm) {
    auto [ea, dn] = rand_reg2();
    auto v1 = get_v32();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    regs.d[ea] = v1;
    write16(0, 0166700 | ea);
    write16(2, dn << 12 | off << 6 | w);
    m68k_do_execute();
    uint32_t mask = 0xffffffffU << (32 - w) >> (32 - w);
    uint32_t vv = std::rotl<uint32_t>(v1, w + off) & mask;
    BOOST_TEST(regs.d[dn] == off + (w + std::countl_zero(vv) - 32));
}

BOOST_AUTO_TEST_CASE(width_reg) {
    auto [ea, wr, dn] = rand_reg3();
    auto v1 = get_v32();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    regs.d[ea] = v1;
    regs.d[wr] = w;
    write16(0, 0166700 | ea);
    write16(2, dn << 12 | 1 << 5 | off << 6 | wr);
    m68k_do_execute();
    uint32_t mask = 0xffffffffU << (32 - w) >> (32 - w);
    uint32_t vv = std::rotl<uint32_t>(v1, w + off) & mask;
    BOOST_TEST(regs.d[dn] == off + (w + std::countl_zero(vv) - 32));
}

BOOST_AUTO_TEST_CASE(off_reg) {
    auto [ea, ofr, dn] = rand_reg3();
    auto v1 = get_v32();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    regs.d[ea] = v1;
    regs.d[ofr] = off;
    write16(0, 0166700 | ea);
    write16(2, dn << 12 | 1 << 11 | ofr << 6 | w);
    m68k_do_execute();
    uint32_t mask = 0xffffffffU << (32 - w) >> (32 - w);
    uint32_t vv = std::rotl<uint32_t>(v1, w + off) & mask;
    BOOST_TEST(regs.d[dn] == off + (w + (vv==0?32:std::countl_zero(vv)) - 32));
}

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