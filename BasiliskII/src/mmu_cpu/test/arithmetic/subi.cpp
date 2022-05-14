
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(SUBI, InitFix)
BOOST_AUTO_TEST_SUITE()
BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[1] = 66;
    raw_write16(0, 0002001);
    raw_write16(2, 25);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 41);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 3456;
    raw_write16(0, 0002101);
    raw_write16(2, 1000);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 2456);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 123456789;
    raw_write16(0, 0002201);
    raw_write32(2, 100000000);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 23456789);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()