#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ORI, InitFix)
BOOST_DATA_TEST_CASE(opc, EA_D(), ea) {
    BOOST_TEST(opc_map[0000000 | ea] == opc_map[0000000]);
    BOOST_TEST(opc_map[0000100 | ea] == opc_map[0000100]);
    BOOST_TEST(opc_map[0000200 | ea] == opc_map[0000200]);
}

BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[1] = 0x98;
    raw_write16(0, 0000001);
    raw_write16(2, 0x2a);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x98 | 0x2a));
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

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[1] = 0x5678;
    raw_write16(0, 0000101);
    raw_write16(2, 0xdead);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x5678 | 0xdead));
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

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[1] = 0x12345678;
    raw_write16(0, 0000201);
    raw_write32(2, 0xdeadbeaf);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345678 | 0xdeadbeaf));
}

BOOST_AUTO_TEST_SUITE_END()