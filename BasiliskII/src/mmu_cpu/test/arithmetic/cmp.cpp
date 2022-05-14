#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(CMP, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(operand) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0130000 | dn << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.z == ( v1 == v2));
}
BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 24;
    regs.d[2] = 24;
    raw_write16(0, 0131002);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 24;
    regs.d[2] = 25;
    raw_write16(0, 0131002);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    regs.d[2] = 1;
    raw_write16(0, 0131002);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}
BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0;
    regs.d[2] = 1;
    raw_write16(0, 0131002);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0130100 | dn << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.z == ( v1 == v2));
}
BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 2400;
    regs.d[2] = 2400;
    raw_write16(0, 0131102);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 2400;
    regs.d[2] = 2500;
    raw_write16(0, 0131102);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    regs.d[2] = 1;
    raw_write16(0, 0131102);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}
BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0;
    regs.d[2] = 1;
    raw_write16(0, 0131102);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0130200 | dn << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.z == ( v1 == v2));
}
BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 240000;
    regs.d[2] = 240000;
    raw_write16(0, 0131202);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 240000;
    regs.d[2] = 250000;
    raw_write16(0, 0131202);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 1;
    raw_write16(0, 0131202);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}
BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0;
    regs.d[2] = 1;
    raw_write16(0, 0131202);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
