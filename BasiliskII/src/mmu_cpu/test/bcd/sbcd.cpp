#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(SBCD, InitFix)
BOOST_DATA_TEST_CASE(Reg, bdata::xrange(1), x) {
    auto [xr, yr] = rand_reg2();
    regs.d[yr] = 0x51;
    regs.d[xr] = 0x27;
    regs.x = x;
    raw_write16(0, 0100400 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[yr] == 0x24 - x);
    BOOST_TEST(!regs.z);
}


BOOST_DATA_TEST_CASE(Memory, bdata::xrange(1), x) {
    auto [xr, yr] = rand_reg2();
    regs.a[yr] = 0x1001;
    regs.a[xr] = 0x2001;
    raw_write8(0x1000, 0x51);
    raw_write8(0x2000, 0x27);
    regs.x = x;
    raw_write16(0, 0100410 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x1000) == 0x24 - x);
    BOOST_TEST(regs.a[yr] == 0x1000);
    BOOST_TEST(regs.a[xr] == 0x2000);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(borrow) {
    regs.d[1] = 0x30;
    regs.d[2] = 0x50;
    raw_write16(0, 0100400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x80);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x30;
    regs.d[2] = 0x30;
    regs.z = true;
    regs.x = false;
    raw_write16(0, 0100400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()