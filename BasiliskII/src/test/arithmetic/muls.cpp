#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MULS, InitFix)
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[3] = -4;
    regs.d[4] = 0x40000000;
    raw_write16(0, 0046004);
    raw_write16(2, 0x0C00 | 3 << 12 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == -1);
    BOOST_TEST(regs.d[3] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = -2;
    regs.d[2] = 10;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2802);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = -12345;
    regs.d[2] = 0;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2802);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = -(0x10000);
    regs.d[2] = -(0x10000);
    raw_write16(0, 0046001);
    raw_write16(2, 0x2800);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = -24;
    regs.d[3] = 12;
    raw_write16(0, 0143702 );
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -288);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    raw_write16(0, 0142701);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()