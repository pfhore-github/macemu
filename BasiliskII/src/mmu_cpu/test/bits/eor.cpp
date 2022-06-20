#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(EOR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)


BOOST_AUTO_TEST_CASE(operand) {
    regs.x = regs.v = regs.c = true;
    regs.d[2] = 0xff;
    regs.d[1] = 0x2f;
    raw_write16(0, 0131402);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xd0);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80;
    regs.d[2] = 0;
    raw_write16(0, 0131402);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0;
    raw_write16(0, 0131402);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 0xffff;
    regs.d[1] = 0x1234;
    raw_write16(0, 0132501);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xEDCB);
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    raw_write16(0, 0131502);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0;
    raw_write16(0, 0131502);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(ToReg) {
    regs.d[2] = 0xffffffff;
    regs.d[1] = 0x12345678;
    raw_write16(0, 0132601);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xEDCBA987);
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}


BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    raw_write16(0, 0131602);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0;
    raw_write16(0, 0131602);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()