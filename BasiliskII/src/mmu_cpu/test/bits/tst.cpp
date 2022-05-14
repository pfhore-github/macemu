#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(TST, InitFix)
BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[1] = 0x80;
    raw_write16(0, 0045001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0045101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0045201);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()