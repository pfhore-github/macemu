#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ADDI, InitFix)

BOOST_DATA_TEST_CASE(opc, EA_D(), ea) {
    BOOST_TEST(opc_map[0003000 | ea] == opc_map[0003000]);
    BOOST_TEST(opc_map[0003100 | ea] == opc_map[0003100]);
    BOOST_TEST(opc_map[0003200 | ea] == opc_map[0003200]);
}

BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[1] = 33;
    raw_write16(0, 0003001);
    raw_write16(2, 25);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 58);
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[1] = 3456;
    raw_write16(0, 0003101);
    raw_write16(2, 1000);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 4456);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[1] = 123456789;
    raw_write16(0, 0003201);
    raw_write32(2, 100000000);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 223456789);
}
BOOST_AUTO_TEST_SUITE_END()