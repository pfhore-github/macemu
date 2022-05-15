#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(PACK, InitFix)
BOOST_AUTO_TEST_CASE(dx) {
    auto [dx, dy] = rand_reg2();    
    regs.d[dx] = 0x1234;
    raw_write16(0, 0100500 | dy << 9 | dx);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == 0x34);
}

BOOST_AUTO_TEST_CASE(ax) {
    auto [dx, dy] = rand_reg2();    
    raw_write16(0x20, 0x1234);
    regs.a[dx] = 0x22;
    regs.a[dy] = 0x11;
    raw_write16(0, 0100510 | dy << 9 | dx);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0x34);
    BOOST_TEST(regs.a[dx] == 0x20);
    BOOST_TEST(regs.a[dy] == 0x10);
}
BOOST_AUTO_TEST_SUITE_END()