#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(EORI, InitFix)

BOOST_DATA_TEST_CASE(opc, EA_D(), ea) {
    BOOST_TEST(opc_map[0005000 | ea] == opc_map[0005000]);
    BOOST_TEST(opc_map[0005100 | ea] == opc_map[0005100]);
    BOOST_TEST(opc_map[0005200 | ea] == opc_map[0005200]);
}

BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[1] = 0x78;
    raw_write16(0, 0005001);
    raw_write16(2, 0x8a);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x78 ^ 0x8a));
}

BOOST_DATA_TEST_CASE(ccr, BIT *BIT *BIT *BIT *BIT, x1, v1, c1, n1, z1) {
    raw_write16(0, 0005074);
    raw_write16(2, x1 << 4 | n1 << 3 | z1 << 2 | v1 << 1 | c1);
    m68k_do_execute();
    BOOST_TEST(regs.x == false);
    BOOST_TEST(regs.v == false);
    BOOST_TEST(regs.c == false);
    BOOST_TEST(regs.n == false);
    BOOST_TEST(regs.z == false);
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[1] = 0x5678;
    raw_write16(0, 0005101);
    raw_write16(2, 0xdead);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x5678 ^ 0xdead));
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[1] = 0x12345678;
    raw_write16(0, 0005201);
    raw_write32(2, 0xdeadbeaf);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345678 ^ 0xdeadbeaf));
}

BOOST_AUTO_TEST_SUITE_END()