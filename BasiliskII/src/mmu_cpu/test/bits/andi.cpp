#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(ANDI, InitFix)

BOOST_DATA_TEST_CASE(opc, EA_D(), ea) {
    BOOST_TEST(opc_map[0001000 | ea] == opc_map[0001000]);
    BOOST_TEST(opc_map[0001100 | ea] == opc_map[0001100]);
    BOOST_TEST(opc_map[0001200 | ea] == opc_map[0001200]);
}

BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[1] = 0xa8;
    raw_write16(0, 0001001);
    raw_write16(2, 0x8a);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0xa8 & 0x8a));
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

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[1] = 0xa678;
    raw_write16(0, 0001101);
    raw_write16(2, 0xdead);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0xa678 & 0xdead));
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

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[1] = 0x82345678;
    raw_write16(0, 0001201);
    raw_write32(2, 0xdeadbeaf);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x82345678 & 0xdeadbeaf));
}

BOOST_AUTO_TEST_SUITE_END()
