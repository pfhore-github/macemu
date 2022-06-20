#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(DIVS, InitFix)
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[1] = -13;
    regs.d[3] = 4;
    raw_write16(0, 0046103);
    raw_write16(2, 0x0800 | 1 << 12 | 2);
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == -3);
    BOOST_TEST( regs.d[2] == -1);
}
BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = -2;
    regs.d[2] = 10;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2802);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = -12345;
    regs.d[2] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2802);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = -2;
    regs.d[2] = 2;
    regs.d[3] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x3C02);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(div0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2802);
    exception_check(5);
}

BOOST_AUTO_TEST_CASE(dword) {
    regs.d[1] = 30;
    regs.d[2] = 0;
    regs.d[3] = -1;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2C03);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -16);
    BOOST_TEST(regs.d[2] == -143165576);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[1] = -1000;
    regs.d[3] = 3;
    raw_write16(0, 0101703 );
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (uint16_t( -333) | uint16_t( -1) << 16) );
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 100;
    regs.d[2] = 0;
    raw_write16(0, 0102701);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 2;
    regs.d[2] = -1000000;
    raw_write16(0, 0102701);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(div0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    raw_write16(0, 0102701);
    exception_check(5);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()