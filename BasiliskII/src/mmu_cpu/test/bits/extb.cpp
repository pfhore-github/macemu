#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(EXTB, InitFix)
BOOST_DATA_TEST_CASE(opc, REG(), dn) {
    BOOST_TEST(opc_map[0044700 | dn] == opc_map[0044700]);
}
BOOST_AUTO_TEST_CASE(pos) {
    regs.d[1] = 44;
    raw_write16(0, 0044701);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 44);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 0;
    raw_write16(0, 0044701);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.n);
}
BOOST_AUTO_TEST_CASE(neg) {
    regs.d[1] = -22 & 0xff;
    raw_write16(0, 0044701);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -22);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()