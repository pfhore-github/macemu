#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(CHK, InitFix)

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(in_range) {
    auto [ea, dn] = rand_reg2();
    regs.d[dn] = 0x4000;
    regs.d[ea] = 0x6000;
    raw_write16(0, 0040600 | dn << 9 | ea);
    exception_check(0);
}

BOOST_AUTO_TEST_CASE(under) {
    auto [ea, dn] = rand_reg2();
    regs.d[dn] = 0xC000;
    regs.d[ea] = 0x1000;
    raw_write16(0, 0040600 | dn << 9 | ea);
    exception_check(6);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(over) {
    auto [ea, dn] = rand_reg2();
    regs.d[dn] = 0x5555;
    regs.d[ea] = 0x3000;
    raw_write16(0, 0040600 | dn << 9 | ea);
    exception_check(6);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(in_range) {
    auto [ea, dn] = rand_reg2();
    regs.d[dn] = 0x400000;
    regs.d[ea] = 0x600000;
    raw_write16(0, 0040400 | dn << 9 | ea);
    exception_check(0);
}

BOOST_AUTO_TEST_CASE(under) {
    auto [ea, dn] = rand_reg2();
    regs.d[dn] = 0xC0000000;
    regs.d[ea] = 0x10000000;
    raw_write16(0, 0040400 | dn << 9 | ea);
    exception_check(6);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(over) {
    auto [ea, dn] = rand_reg2();
    regs.d[dn] = 0x70000000;
    regs.d[ea] = 0x30000000;
    raw_write16(0, 0040400 | dn << 9 | ea);
    exception_check(6);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()