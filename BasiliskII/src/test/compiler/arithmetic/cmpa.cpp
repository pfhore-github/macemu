#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(CMPA, InitFix)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    regs.a[2] = 1;
    regs.a[3] = 0;
    raw_write16(0, 0132313);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}
BOOST_AUTO_TEST_CASE(z) {
    regs.a[1] = 2400;
    regs.a[2] = 2400;
    raw_write16(0, 0131312);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(n) {
    regs.a[1] = -2500;
    regs.a[2] = -2400;
    raw_write16(0, 0131312);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.a[1] = 0x8000;
    regs.a[2] = 1;
    raw_write16(0, 0131312);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}
BOOST_AUTO_TEST_CASE(c) {
    regs.a[1] = 0;
    regs.a[2] = 1;
    raw_write16(0, 0131312);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(operand) {
    regs.a[1] = 1;
    regs.a[2] = 0;
    raw_write16(0, 0131712);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}
BOOST_AUTO_TEST_CASE(z) {
    regs.a[1] = 240000;
    regs.a[2] = 240000;
    raw_write16(0, 0131712);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(n) {
    regs.a[1] = 240000;
    regs.a[2] = 250000;
    raw_write16(0, 0131712);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.a[1] = 0x80000000;
    regs.a[2] = 1;
    raw_write16(0, 0131712);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}
BOOST_AUTO_TEST_CASE(c) {
    regs.a[1] = 0;
    regs.a[2] = 1;
    raw_write16(0, 0131712);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()