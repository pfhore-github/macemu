#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>

BOOST_FIXTURE_TEST_SUITE(BFTST, InitFix)
BOOST_AUTO_TEST_SUITE(REG)

BOOST_AUTO_TEST_CASE(both_imm) {
    auto ea = rand_reg();
    auto v1 = get_v32();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    regs.d[ea] = v1;
    write16(0, 0164300 | ea);
    write16(2, off << 6 | w);
    m68k_do_execute();
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    BOOST_TEST(regs.n == (v1 >> (31 - off) & 1));
    BOOST_TEST(regs.z == ((v1 & mask) == 0));
}

BOOST_AUTO_TEST_CASE(width_reg) {
    auto [ea, wr] = rand_reg2();
    auto v1 = get_v32();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    regs.d[ea] = v1;
    regs.d[wr] = w;
    write16(0, 0164300 | ea);
    write16(2, 1 << 5 | off << 6 | wr);
    m68k_do_execute();
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    BOOST_TEST(regs.n == (v1 >> (31 - off) & 1));
    BOOST_TEST(regs.z == ((v1 & mask) == 0));
}

BOOST_AUTO_TEST_CASE(off_reg) {
    auto [ea, wr] = rand_reg2();
    auto v1 = get_v32();
    auto off = get_vn(0, 31);
    auto w = get_vn(1, 31);
    regs.d[ea] = v1;
    regs.d[wr] = off;
    write16(0, 0164300 | ea);
    write16(2, 1 << 11 | wr << 6 | w);
    m68k_do_execute();
    uint32_t mask = std::rotr<uint32_t>(0xffffffffU << (32 - w), off);
    BOOST_TEST(regs.n == (v1 >> (31 - off) & 1));
    BOOST_TEST(regs.z == ((v1 & mask) == 0));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(n) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x18);
    write16(0, 0164321);
    write16(2, 4 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    write16(0, 0164321);
    write16(2, 4 << 6 | 4);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
