#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(ABCD, InitFix)

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x51;
    regs.d[2] = 0x27;
    regs.x = false;
    raw_write16(0, 0141402);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x78);
}

BOOST_AUTO_TEST_CASE(Memory) {
    regs.a[1] = 0x1001;
    regs.a[2] = 0x2001;
    raw_write8(0x1000, 0x51);
    raw_write8(0x2000, 0x27);
    regs.x = false;
    raw_write16(0, 0141412);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x1000) == 0x78);
    BOOST_TEST(regs.a[1] == 0x1000);
    BOOST_TEST(regs.a[2] == 0x2000);
}

BOOST_DATA_TEST_CASE(X, BIT, x) {
    regs.d[0] = 0x51;
    regs.d[1] = 0x27;
    regs.x = x;
    raw_write16(0, 0140401);
    m68k_do_execute();
    BOOST_TEST(regs.d[0] == 0x78 + x);
}

BOOST_AUTO_TEST_CASE(carry) {
    regs.d[1] = 0x70;
    regs.d[2] = 0x70;
    raw_write16(0, 0140400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x40);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_DATA_TEST_CASE(z, BIT * BIT, old_z, new_z) {
    regs.d[1] = 0x30;
    regs.d[2] = 0x69 + new_z;
    regs.z = old_z;
    regs.x = false;
    raw_write16(0, 0140400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z == (new_z ? false : old_z));
}

BOOST_AUTO_TEST_SUITE_END()
