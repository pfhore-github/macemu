#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ROR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8), n) {
    auto dy = rand_reg();
    auto v1 = get_v8();
    regs.d[dy] = v1;
    write16(0, 0160030 | n << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == std::rotr<uint8_t>(v1, (n ? n : 8)));
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [dx, dy] = rand_reg2();
    auto v1 = get_v8();
    int n = get_vn(1, 7);
    regs.d[dy] = v1;
    regs.d[dx] = n;
    write16(0, 0160070 | dx << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == std::rotr<uint8_t>(v1, n));
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x39;
    regs.d[2] = 68;
    write16(0, 0162071);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 1;
    write16(0, 0161031);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c_0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    write16(0, 0161073);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 1;
    write16(0, 0161031);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161031);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8), n) {
    auto dy = rand_reg();
    auto v1 = get_v16();
    regs.d[dy] = v1;
    write16(0, 0160130 | n << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == std::rotr<uint16_t>(v1, (n ? n : 8)));
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [dx, dy] = rand_reg2();
    auto v1 = get_v16();
    int n = get_vn(1, 15);
    regs.d[dy] = v1;
    regs.d[dx] = n;
    write16(0, 0160170 | dx << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == std::rotr<uint16_t>(v1, n));
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x3456;
    regs.d[2] = 68;
    write16(0, 0162171);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x6345);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 1;
    write16(0, 0161131);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c_0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    write16(0, 0161173);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 1;
    write16(0, 0161131);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161131);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(ea) {
    auto dy = rand_ar();
    regs.a[dy] = 0x10;
    raw_write16(0x10, 0x4000);
    write16(0, 0163320 | dy);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8), n) {
    auto dy = rand_reg();
    auto v1 = get_v32();
    regs.d[dy] = v1;
    write16(0, 0160230 | n << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == std::rotr<uint32_t>(v1, (n ? n : 8)));
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [dx, dy] = rand_reg2();
    auto v1 = get_v32();
    int n = get_vn(1, 31);
    regs.d[dy] = v1;
    regs.d[dx] = n;
    write16(0, 0160270 | dx << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == std::rotr<uint32_t>(v1, n));
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x3456789A;
    regs.d[2] = 68;
    write16(0, 0162271);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xA3456789);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 1;
    write16(0, 0161231);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(c_0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    write16(0, 0161123);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 1;
    write16(0, 0161231);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161231);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
