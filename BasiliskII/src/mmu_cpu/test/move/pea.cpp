#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(PEA, InitFix)
BOOST_AUTO_TEST_CASE(run) {
    auto ar = rand_ar();
    regs.a[7] = 0x104;
    regs.a[ar] = 0x10;
    raw_write16(0, 0044120 | ar);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x100) == 0x10);
    BOOST_TEST(regs.a[7] == 0x100);
}

BOOST_AUTO_TEST_SUITE_END()