#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MOVEA, InitFix)
BOOST_DATA_TEST_CASE(opc, REG() * EA_D_R(), an, ea_src) {
    BOOST_TEST(opc_map[0020100 | an << 9 | ea_src] ==
               opc_map[0020100]);
    BOOST_TEST(opc_map[0030100 | an << 9 | ea_src] ==
               opc_map[0030100]);
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[1] = 0x8040;
    raw_write16(0, 0032101);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0xffff8040);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[1] = 0x12345678;
    raw_write16(0, 0022101);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x12345678);
}
BOOST_AUTO_TEST_SUITE_END()