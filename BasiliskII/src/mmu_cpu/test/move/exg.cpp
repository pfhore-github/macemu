#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(MOVE, InitFix)
BOOST_AUTO_TEST_CASE(DR2DR) {
    regs.d[2] = 0x12345678;
    regs.d[3] = 0x9abcdef0;
    raw_write16(0, 0142503);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x9abcdef0);
    BOOST_TEST(regs.d[3] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(AR2AR) {
    regs.a[2] = 0xa0000000;
    regs.a[3] = 0x40000000;
    raw_write16(0, 0142513);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x40000000);
    BOOST_TEST(regs.a[3] == 0xa0000000);
}

BOOST_AUTO_TEST_CASE(AR2DR) {
    regs.d[2] = 0x20000000;
    regs.a[3] = 0x40000000;
    raw_write16(0, 0142613 );
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x40000000);
    BOOST_TEST(regs.a[3] == 0x20000000);
}
BOOST_AUTO_TEST_SUITE_END()