#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(JSR, InitFix)
BOOST_AUTO_TEST_CASE(run) {
    auto ea = rand_ar();
    regs.a[ea] = 0x10;
    regs.a[7] = 0x204;
    raw_write16(0, 0047220 | ea);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x200) == 2);
    BOOST_TEST(regs.pc == 0x10);
}
BOOST_AUTO_TEST_SUITE_END()