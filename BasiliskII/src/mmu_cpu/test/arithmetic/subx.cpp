#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(SUBX, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(Reg, BIT, x) {
    regs.d[1] = 40;
    regs.d[2] = 30;
    regs.x = x;
    raw_write16(0, 0111402);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (10 - x));
}

BOOST_DATA_TEST_CASE(Memory, BIT, x) {
    regs.a[1] = 0x1001;
    regs.a[2] = 0x2001;
    raw_write8(0x1000, 40);
    raw_write8(0x2000, 30);
    regs.x = x;
    raw_write16(0, 0111412);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x1000) == (10 - x));
    BOOST_TEST(regs.a[1] == 0x1000);
    BOOST_TEST(regs.a[2] == 0x2000);
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    regs.d[2] = 2;
    raw_write16(0, 0111402);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    regs.d[2] = 0x1;
    raw_write16(0, 0111402);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfe;
    regs.d[2] = 0x1;
    raw_write16(0, 0111402);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_DATA_TEST_CASE(z, BIT *BIT, old_z, new_z) {
    regs.z = old_z;
    regs.d[1] = 2;
    regs.d[2] = 1 + new_z;
    raw_write16(0, 0111402);
    m68k_do_execute();
    BOOST_TEST(regs.z == (new_z ? old_z : false));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(Reg, BIT, x) {
    regs.d[1] = 1000;
    regs.d[2] = 700;
    regs.x = x;
    raw_write16(0, 0111502);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 300 - x);
}

BOOST_DATA_TEST_CASE(Memory, BIT, x) {
    regs.a[1] = 0x1002;
    regs.a[2] = 0x2002;
    raw_write16(0x1000, 1000);
    raw_write16(0x2000, 700);
    regs.x = x;
    raw_write16(0, 0111512);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x1000) == (300 - x));
    BOOST_TEST(regs.a[1] == 0x1000);
    BOOST_TEST(regs.a[2] == 0x2000);
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    regs.d[2] = 2;
    raw_write16(0, 0111502);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0x1;
    raw_write16(0, 0111502);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffe;
    regs.d[2] = 0x1;
    raw_write16(0, 0111502);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_DATA_TEST_CASE(z, BIT *BIT, old_z, new_z) {
    regs.z = old_z;
    regs.d[1] = 2;
    regs.d[2] = 1 + new_z;
    raw_write16(0, 0111502);
    m68k_do_execute();
    BOOST_TEST(regs.z == (new_z ? old_z : false));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(Reg, bdata::xrange(1), x) {
    regs.d[1] = 400000;
    regs.d[2] = 300000;
    regs.x = x;
    raw_write16(0, 0111602);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (100000 - x));
}

BOOST_DATA_TEST_CASE(Memory, bdata::xrange(1), x) {
    regs.a[1] = 0x1004;
    regs.a[2] = 0x2004;
    raw_write32(0x1000, 400000);
    raw_write32(0x2000, 300000);
    regs.x = x;
    raw_write16(0, 0111612);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x1000) == (100000 - x));
    BOOST_TEST(regs.a[1] == 0x1000);
    BOOST_TEST(regs.a[2] == 0x2000);
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    regs.d[2] = 2;
    raw_write16(0, 0111602);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0x1;
    raw_write16(0, 0111602);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffffffe;
    regs.d[2] = 0x1;
    raw_write16(0, 0111602);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_DATA_TEST_CASE(z, BIT *BIT, old_z, new_z) {
    regs.z = old_z;
    regs.d[1] = 2;
    regs.d[2] = 1 + new_z;
    raw_write16(0, 0111602 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z == (new_z ? old_z : false));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
