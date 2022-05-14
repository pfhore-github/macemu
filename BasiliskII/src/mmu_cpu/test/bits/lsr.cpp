#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(LSR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8), n) {
    auto dy = rand_reg();
    auto v1 = get_v8();
    regs.d[dy] = v1;
    write16(0, 0160010 | n << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == ((v1 >> (n ? n : 8) & 0xff)));
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [dx, dy] = rand_reg2();
    auto v1 = get_v8();
    int n = get_vn(1, 7);
    regs.d[dy] = v1;
    regs.d[dx] = n;
    write16(0, 0160050 | dx << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == ((v1 >> n) & 0xff));
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    write16(0, 0160050 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 1;
    write16(0, 0161011);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx_0) {
    regs.c = regs.x = true;
    regs.d[2] = 0xff;
    regs.d[1] = 0;
    write16(0, 0161052);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}


BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161011);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8), n) {
    auto dy = rand_reg();
    auto v1 = get_v16();
    regs.d[dy] = v1;
    write16(0, 0160110 | n << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == (v1 >> (n ? n : 8) & 0xffff));
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [dx, dy] = rand_reg2();
    auto v1 = get_v16();
    int n = get_vn(1, 15);
    regs.d[dy] = v1;
    regs.d[dx] = n;
    write16(0, 0160150 | dx << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == ((v1 >> n) & 0xffff));
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    write16(0, 0160150 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 1;
    write16(0, 0161111);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx_0) {
    regs.c = regs.x = true;
    regs.d[2] = 0xffff;
    regs.d[1] = 0;
    write16(0, 0161152);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161111);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ea) {
    auto ea_v = rand_reg();
    regs.a[ea_v] = 0x10;
    raw_write16(0x10, 0x8000);
    raw_write16(0, 0161320 | ea_v);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) = 0x4000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(Imm, bdata::xrange(0, 8), n) {
    auto dy = rand_reg();
    auto v1 = get_v32();
    regs.d[dy] = v1;
    write16(0, 0160210 | n << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == (v1 >> (n ? n : 8)));
}

BOOST_AUTO_TEST_CASE(Reg) {
    auto [dx, dy] = rand_reg2();
    auto v1 = get_v32();
    int n = get_vn(1, 31);
    regs.d[dy] = v1;
    regs.d[dx] = n;
    write16(0, 0160250 | dx << 9 | dy);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == v1 >> n);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 32;
    regs.d[2] = 66;
    write16(0, 0160250 | 2 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 8);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.c = regs.x = false;
    regs.d[1] = 1;
    write16(0, 0161211);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(cx_0) {
    regs.c = regs.x = true;
    regs.d[2] = 0xffffffff;
    regs.d[1] = 0;
    write16(0, 0161251);
    m68k_do_execute();
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    write16(0, 0161211);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()