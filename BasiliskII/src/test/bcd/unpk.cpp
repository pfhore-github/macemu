#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(UNPK, InitFix)
BOOST_AUTO_TEST_CASE(dx) {
    regs.d[2] = 0x1234;
    raw_write16(0, 0103602 );
    raw_write16(2, 0x10);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x314);
}

BOOST_AUTO_TEST_CASE(ax) {
    raw_write8(0x20, 0x12);
    regs.a[2] = 0x21;
    regs.a[3] = 0x12;
    raw_write16(0, 0103612);
    raw_write16(2, 0x10);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0x0112);
    BOOST_TEST(regs.a[2] == 0x20);
    BOOST_TEST(regs.a[3] == 0x10);
}
BOOST_AUTO_TEST_SUITE_END()
