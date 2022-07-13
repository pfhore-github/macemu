#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(PACK, InitFix)
BOOST_AUTO_TEST_CASE(dx) {
    regs.d[2] = 0x1234;
    raw_write16(0, 0103502);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x34);
}

BOOST_AUTO_TEST_CASE(ax) {
    raw_write16(0x20, 0x1234);
    regs.a[2] = 0x22;
    regs.a[3] = 0x11;
    raw_write16(0, 0103512 );
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x34);
    BOOST_TEST(regs.a[2] == 0x20);
    BOOST_TEST(regs.a[3] == 0x10);
}
BOOST_AUTO_TEST_SUITE_END()