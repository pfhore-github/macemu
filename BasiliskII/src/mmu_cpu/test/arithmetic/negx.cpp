#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(NEGX, InitFix)

BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(oldx, BIT, x) {
    regs.d[2] = 10;
    regs.x = x;
    raw_write16(0, 0040002);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == (0xf6 - x));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    regs.x = false;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    regs.x = false;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.x = false;
    raw_write16(0, 0040001);
    m68k_do_execute();
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 3;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(oldx, BIT, x) {
    regs.d[2] = 1000;
    regs.x = x;
    raw_write16(0, 0040102);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == (0xfc18 - x));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    regs.x = false;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.x = false;
    raw_write16(0, 0040101);
    m68k_do_execute();
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 3;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(oldx, BIT, x) {
    regs.d[2] = 100000;
    regs.x = x;
    raw_write16(0, 0040202);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == - 100000 - x);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    regs.x = false;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.x = false;
    raw_write16(0, 0040201);
    m68k_do_execute();
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 3;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()