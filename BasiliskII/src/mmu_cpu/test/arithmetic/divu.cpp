#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(DIVU, InitFix)
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[3] = 10000;
    regs.d[2] = 3;
    raw_write16(0, 0046102);
    raw_write16(2, 0x0000 | 3 << 12 | 1);
    m68k_do_execute();
    BOOST_TEST( regs.d[3] == 3333);
    BOOST_TEST( regs.d[1] == 1);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 1;
    regs.d[2] = 0x80000000;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2002);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2002);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 2;
    regs.d[2] = 2;
    regs.d[3] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x3402);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(div0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2002);
    exception_check(5);
}

BOOST_AUTO_TEST_CASE(dword) {
    regs.d[1] = 30;
    regs.d[2] = 0;
    regs.d[3] = 1;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2403);

    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 16);
    BOOST_TEST(regs.d[2] == 143165576);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 3;
    regs.d[2] = 1000;
    raw_write16(0, 0102301);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == (333 | 1 << 16));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 100;
    regs.d[2] = 0;
    raw_write16(0, 0102301);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(overflow) {
    regs.d[1] = 2;
    regs.d[2] = 1000000;
    raw_write16(0, 0102301);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(div0) {
    regs.vbr = 0x100;
    regs.d[2] = 1;
    regs.d[1] = 0;
    raw_write16(0, 0102301);
    exception_check(5);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
