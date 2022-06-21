
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(SUBI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 100;
    raw_write16(0, 0002002);
    raw_write16(2, 10);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 90);
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0002001);
    raw_write16(2, 2);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    raw_write16(0, 0002001);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    raw_write16(0, 0002001);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfe;
    raw_write16(0, 0002001);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 1000;
    raw_write16(0, 0002102);
    raw_write16(2, 300);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 700);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0002101);
    raw_write16(2, 2);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0002101);
    raw_write16(2, 2);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    raw_write16(0, 0002101);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffe;
    raw_write16(0, 0002101);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(operand) {
    regs.d[3] = 100000;
    raw_write16(0, 0002203);
    raw_write32(2, 60000);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 40000);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0002201);
    raw_write32(2, 2);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0002201);
    raw_write32(2, 2);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    raw_write16(0, 0002201);
    raw_write32(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffffffe;
    raw_write16(0, 0002201);
    raw_write32(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()