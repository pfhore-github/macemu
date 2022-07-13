#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(ANDI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    regs.d[2] = 0xff;
    raw_write16(0, 0001002);
    raw_write16(2, 0x2f);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x2f);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xff;
    raw_write16(0, 0001001);
    raw_write16(2, 0x80);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x80);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xff;
    raw_write16(0, 0001001);
    raw_write16(2, 0);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_DATA_TEST_CASE(ccr, BIT *BIT *BIT *BIT *BIT, x1, v1, c1, n1, z1) {
    regs.x = regs.v = regs.c = regs.n = regs.z = true;
    raw_write16(0, 0001074);
    raw_write16(2, x1 << 4 | n1 << 3 | z1 << 2 | v1 << 1 | c1);
    m68k_do_execute();
    BOOST_TEST(regs.x == x1);
    BOOST_TEST(regs.v == v1);
    BOOST_TEST(regs.c == c1);
    BOOST_TEST(regs.n == n1);
    BOOST_TEST(regs.z == z1);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    regs.d[2] = 0xffff;
    raw_write16(0, 0001102);
    raw_write16(2, 0x2fff);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x2fff);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0001101);
    raw_write16(2, 0x8000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0001101);
    raw_write16(2, 0);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}
BOOST_AUTO_TEST_CASE(sr_invalid) {
    regs.S = false;
    raw_write16(0, 0001174);
    raw_write16(2, 0xffff);
    exception_check(8);
}

BOOST_AUTO_TEST_CASE(sr_valid) {
    regs.S = true;
    regs.M = false;
    regs.IM = 7;
    raw_write16(0, 0001174);
    raw_write16(2, 0);
    m68k_do_execute();
    BOOST_TEST(!regs.S);
    BOOST_TEST(regs.IM == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    regs.d[2] = 0xffffffff;
    raw_write16(0, 0001202);
    raw_write32(2, 0x2fffffff);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x2fffffff);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0001201);
    raw_write32(2, 0x80000000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0001201);
    raw_write32(2, 0);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
