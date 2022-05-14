#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"


BOOST_FIXTURE_TEST_CASE(rtr, InitFix) {
    regs.a[7] = 0x200;
    raw_write16(0, 0047167);
    raw_write16(0x200, 0x1);
    raw_write32(0x202, 0x1000);
    m68k_do_execute();
    BOOST_TEST(regs.c );
    BOOST_TEST(regs.pc == 0x1000);
}