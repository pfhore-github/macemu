#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(LINK, InitFix)
BOOST_AUTO_TEST_CASE(Long) {
    regs.a[7] = 0x204;
    auto ar = rand_ar();
    regs.a[ar] = 0x200000;
    raw_write16(0, 0044010 | ar);
    raw_write32(2, 100000);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x200) == 0x200000);
    BOOST_TEST(regs.a[ar] == 0x200);
    BOOST_TEST(regs.a[7] == 0x200 + 100000);
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.a[7] = 0x204;
    auto ar = rand_ar();
    regs.a[ar] = 0x2000;
    raw_write16(0, 0047120 | ar);
    raw_write16(2, 1000);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x200) == 0x2000);
    BOOST_TEST(regs.a[ar] == 0x200);
    BOOST_TEST(regs.a[7] == 0x200 + 1000);
}
BOOST_AUTO_TEST_SUITE_END()