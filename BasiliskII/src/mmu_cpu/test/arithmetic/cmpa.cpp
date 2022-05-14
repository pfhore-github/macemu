#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(CMPA, InitFix)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    auto [an, ea] = rand_reg2();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.a[an] = v1;
    regs.a[ea] = v2;
    raw_write16(0, 0130310 | an << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.z == ( (int16_t)v1 == (int16_t)v2));
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
    auto [an, ea] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.a[an] = v1;
    regs.a[ea] = v2;
    raw_write16(0, 0130710 | an << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.z == ( v1 == v2));
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