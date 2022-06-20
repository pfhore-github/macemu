#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(CMPI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 24;
    raw_write16(0, 0006001);
    raw_write16(2, 24);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 24;
    raw_write16(0, 0006001);
    raw_write16(2, 25);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    raw_write16(0, 0006001);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}
BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0;
    raw_write16(0, 0006001);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 2400;
    raw_write16(0, 0006101);
    raw_write16(2, 2400);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 2400;
    raw_write16(0, 0006101);
    raw_write16(2, 2500);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0006101);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}
BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0;
    regs.d[2] = 1;
    raw_write16(0, 0006101);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 240000;
    raw_write16(0, 0006201);
    raw_write32(2, 240000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 240000;
    raw_write16(0, 0006201);
    raw_write32(2, 250000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0006201);
    raw_write32(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}
BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0;
    raw_write16(0, 0006201);
    raw_write32(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()