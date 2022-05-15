#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ORI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.d[ea] = v1;
    raw_write16(0, 0000000 | ea);
    raw_write16(2, v2);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (v1 | v2));
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80;
    raw_write16(0, 0000001);
    raw_write16(2, 0);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0000001);
    raw_write16(2, 0);

    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_DATA_TEST_CASE(ccr, BIT *BIT *BIT *BIT *BIT, x1, v1, c1, n1, z1) {
    regs.x = regs.v = regs.c = regs.n = regs.z = false;
    raw_write16(0, 0000074);
    raw_write16(2, x1 << 4 | n1 << 3 | z1 << 2 | v1 << 1 | c1);

    m68k_do_execute();
    BOOST_TEST(regs.x == x1);
    BOOST_TEST(regs.n == n1);
    BOOST_TEST(regs.z == z1);
    BOOST_TEST(regs.v == v1);
    BOOST_TEST(regs.c == c1);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.d[ea] = v1;
    raw_write16(0, 0000100 | ea);
    raw_write16(2, v2);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (v1 | v2));
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0000101);
    raw_write16(2, 0);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0000101);
    raw_write16(2, 0);

    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(sr_invalid) {
    regs.S = false;
    raw_write16(0, 0000174);
    raw_write16(2, 0xffff);
    exception_check(8);
}

BOOST_DATA_TEST_CASE(sr_valid, bdata::xrange(8), i) {
    regs.S = true;
    regs.M = false;
    regs.IM = 0;
    raw_write16(0, 0000174);
    raw_write16(2, 1 << 12 | i << 8);
    m68k_do_execute();
    BOOST_TEST(regs.M);
    BOOST_TEST(regs.IM == i);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[ea] = v1;
    raw_write16(0, 0000200 | ea);
    raw_write32(2, v2);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (v1 | v2));
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0000201);
    raw_write32(2, 0);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0000201);
    raw_write32(2, 0);

    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()