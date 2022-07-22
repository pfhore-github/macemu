#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MOVEQ, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    raw_write16(0, 0x76f0);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0xfffffff0);
}
BOOST_AUTO_TEST_CASE(n) {
    raw_write16(0, 0071377);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == -1);
}

BOOST_AUTO_TEST_CASE(z) {
    raw_write16(0, 0071000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}


BOOST_AUTO_TEST_SUITE_END()