#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(ABCD, InitFix)
BOOST_DATA_TEST_CASE(value, boost::unit_test::data::xrange(1), x) {
    regs.d[1] = 0x51;

    regs.d[2] = 0x27;
    regs.x = x;
    asm_m68k("abcd %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x78 + x);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(carry) {
    regs.d[1] = 0x70;
    regs.d[2] = 0x70;
    asm_m68k("abcd %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x40);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x30;
    regs.d[2] = 0x70;
    regs.z = true;
    regs.x = false;
    asm_m68k("abcd %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()