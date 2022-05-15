#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(UNPK, InitFix)
BOOST_AUTO_TEST_CASE(dx) {
    auto [dx, dy] = rand_reg2();
    regs.d[dx] = 0x1234;
    raw_write16(0, 0100600 | dy << 9 | dx);
    raw_write16(2, 0x10);
    m68k_do_execute();
    BOOST_TEST(regs.d[dy] == 0x314);
}

BOOST_AUTO_TEST_CASE(ax) {
    auto [dx, dy] = rand_reg2();
    raw_write8(0x20, 0x12);
    regs.a[dx] = 0x21;
    regs.a[dy] = 0x12;
    raw_write16(0, 0100610 | dy << 9 | dx);
    raw_write16(2, 0x10);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0x0112);
    BOOST_TEST(regs.a[dx] == 0x20);
    BOOST_TEST(regs.a[dy] == 0x10);
}
BOOST_AUTO_TEST_SUITE_END()
