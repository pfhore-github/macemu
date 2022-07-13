#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MOVEA, InitFix)

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[2] = 0xC000;
    raw_write16(0, 0033102);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0xFFFFC000);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[2] = 0x10000;
    raw_write16(0, 0023102);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x10000);
}
BOOST_AUTO_TEST_SUITE_END()