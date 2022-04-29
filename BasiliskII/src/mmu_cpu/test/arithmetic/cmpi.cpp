#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(CMPI, InitFix)

BOOST_DATA_TEST_CASE(opc, EA_D_R(), ea) {
    BOOST_TEST(opc_map[0006000 | ea] == opc_map[0006000]);
    BOOST_TEST(opc_map[0006100 | ea] == opc_map[0006100]);
    BOOST_TEST(opc_map[0006200 | ea] == opc_map[0006200]);
}

BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[1] = 24;
    raw_write16(0, 0006001);
    raw_write16(2, 25);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[1] = 1000;
    raw_write16(0, 0006101);
    raw_write16(2, 1001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[1] = 100000000;
    raw_write16(0, 0006201);
    raw_write32(2, 100000001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()