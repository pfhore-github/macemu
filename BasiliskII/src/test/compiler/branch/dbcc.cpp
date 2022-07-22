#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(DBCC, InitFix)
BOOST_AUTO_TEST_CASE(t) {
    raw_write16(0, 0x50C9);
    raw_write16(2, 0x10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x4);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.d[1] = 2;
    raw_write16(0, 0x51C9);
    raw_write16(2, 0x10);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 1);
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.d[1] = 0;
    raw_write16(0, 0x51C9);
    raw_write16(2, 0x10);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff);
    BOOST_TEST(regs.pc == 0x4);
}
BOOST_AUTO_TEST_SUITE_END()