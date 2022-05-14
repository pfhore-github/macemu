#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(DIVS, InitFix)
BOOST_DATA_TEST_CASE(opc, REG(), dn) {
    BOOST_TEST(opc_map[01007 | dn << 3] == opc_map[01007]);
}
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = -2;
    regs.d[2] = 10000;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2802);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == -5000);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = -12345;
    regs.d[2] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2802);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(overflow) {
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
BOOST_AUTO_TEST_CASE(mod) {
    regs.d[1] = 3;
    regs.d[2] = -10000;
    regs.d[3] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2803);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
    BOOST_TEST(regs.d[2] == -3333);
}

BOOST_AUTO_TEST_CASE(divsl) {
    regs.d[1] = 3;
    regs.d[2] = -10000;
    regs.d[3] = 1;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2803);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -1);
    BOOST_TEST(regs.d[2] == -3333);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(in_range) {
    regs.d[1] = 3;
    regs.d[2] = -1000;
    raw_write16(0, 0102701);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == ((-333 & 0xffff) | 0xffff << 16));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 100;
    regs.d[2] = 0;
    raw_write16(0, 0102701);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(overflow) {
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