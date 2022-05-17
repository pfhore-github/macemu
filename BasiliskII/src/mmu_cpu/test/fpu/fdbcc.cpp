#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FDBcc, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    auto cr = rand_reg();
    regs.d[cr] = 1;
    raw_write16(0, 0171110 | cr);
    raw_write16(2, 0);
    raw_write16(4, 0x1000);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x1004);
    BOOST_TEST(regs.d[cr] == 0);
}

BOOST_AUTO_TEST_CASE(t) {
    regs.d[1] = 1;
    raw_write16(0, 0171111);
    raw_write16(2, 0x000f);
    raw_write16(4, 0x1000);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 6);
    BOOST_TEST(regs.d[1] == 1);
}

BOOST_AUTO_TEST_CASE(loop_exit) {
    regs.d[1] = 0;
    raw_write16(0, 0171111);
    raw_write16(2, 0x0000);
    raw_write16(4, 0x1000);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 6);
    BOOST_TEST(regs.d[1] == 0xffff);
}

BOOST_AUTO_TEST_SUITE_END()