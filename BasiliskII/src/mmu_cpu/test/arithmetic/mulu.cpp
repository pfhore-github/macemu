#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MULU, InitFix)
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 0x80000000;
    regs.d[4] = 2;
    raw_write16(0, 0046004);
    raw_write16(2, 0x0400 | 2 << 12 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 1);
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x10000;
    regs.d[2] = 0x10000;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[3] = 120;
    regs.d[2] = 3;
    raw_write16(0, 0142303 );
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 360);
    BOOST_TEST(!regs.v);
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
