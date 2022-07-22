
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"


BOOST_FIXTURE_TEST_SUITE(SUBA, InitFix)

BOOST_AUTO_TEST_CASE(Word) {
    regs.a[2] = 3456;
    regs.d[4] = 100;
    raw_write16(0, 0112304);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 3356);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.a[2] = 123456789;
    regs.d[3] = 100000000;
    raw_write16(0, 0112703);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 23456789);
}
BOOST_AUTO_TEST_SUITE_END()
