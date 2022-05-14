#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MOVEA, InitFix)
BOOST_DATA_TEST_CASE(opc, REG(), an) {
    BOOST_TEST(opc_map[00201 | an << 3] == opc_map[00201]);
    BOOST_TEST(opc_map[00301 | an << 3] == opc_map[00301]);
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