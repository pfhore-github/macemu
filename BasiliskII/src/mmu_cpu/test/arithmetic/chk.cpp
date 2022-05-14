#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(CHK, InitFix)
BOOST_DATA_TEST_CASE(opc, REG(), dn ){
    BOOST_TEST(opc_map[00404 | dn << 3] == opc_map[00404]);
    BOOST_TEST(opc_map[00406 | dn << 3] == opc_map[00406]);
}

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[2] = 0x4000;
    regs.d[1] = 0x6000;
    raw_write16(0, 0042601);
    exception_check(0);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.d[2] = 0xC000;
    regs.d[1] = 0x1000;
    raw_write16(0, 0042601);
    exception_check(6);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.d[2] = 0x5555;
    regs.d[1] = 0x3000;
    raw_write16(0, 0042601);
    exception_check(6);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(in_range) {
    regs.vbr = 0x100;
    regs.d[2] = 0x400000;
    regs.d[1] = 0x600000;
    raw_write16(0, 0042401);
    exception_check(0);
}

BOOST_AUTO_TEST_CASE(under) {
    regs.d[2] = 0xC0000000;
    regs.d[1] = 0x10000000;
    raw_write16(0, 0042401);
    exception_check(6);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(over) {
    regs.d[2] = 0x70000000;
    regs.d[1] = 0x30000000;
    raw_write16(0, 0042401);
    exception_check(6);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()