#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE(rtd, InitFix) {
    regs.a[7] = 0x200;
    raw_write16(0, 0047164);
    raw_write16(2, -100);
    raw_write32(0x200, 0x1000);
    m68k_do_execute();
    BOOST_TEST(regs.a[7] == 0x200 -100 + 4);
    BOOST_TEST(regs.pc == 0x1000);
}
