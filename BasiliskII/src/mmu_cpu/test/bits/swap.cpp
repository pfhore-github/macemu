#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(SWAP, InitFix)

BOOST_DATA_TEST_CASE(opc, REG(), dn) {
    BOOST_TEST(opc_map[0044100 | dn] == opc_map[0044100]);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x12348678;
    regs.z = true;
    regs.c = true;
    raw_write16(0, 0044101);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x86781234);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0044101);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()