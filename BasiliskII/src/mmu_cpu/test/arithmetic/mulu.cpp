#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MULU, InitFix)
BOOST_DATA_TEST_CASE(opc, REG(), dn) {
    BOOST_TEST(opc_map[01403 | dn << 3] == opc_map[01403]);
}
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 1234;
    regs.d[2] = 3456;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 1234 * 3456);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(overflow) {
    regs.d[1] = 0x10000;
    regs.d[2] = 0x10000;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(high) {
    regs.d[1] = 0x10000;
    regs.d[2] = 0x10000;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2403);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(regs.d[3] == 1);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 1234;
    regs.d[2] = 3456;
    raw_write16(0, 0142301);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 1234 * 3456);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    raw_write16(0, 0142301);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
