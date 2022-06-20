#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(ADDX, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(Reg, bdata::xrange(1), x) {
    regs.d[2] = 11;
    regs.d[3] = 22;
    regs.x = x;
    raw_write16(0, 0152403);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == (33 + x));
}

BOOST_DATA_TEST_CASE(Memory, bdata::xrange(1), x) {
    regs.a[2] = 0x1001;
    regs.a[3] = 0x2001;
    raw_write8(0x1000, 22);
    raw_write8(0x2000, 33);
    regs.x = x;
    raw_write16(0, 0152413);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x1000) == (55 + x));
    BOOST_TEST(regs.a[2] == 0x1000);
    BOOST_TEST(regs.a[3] == 0x2000);
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xff;
    regs.d[2] = 2;
    raw_write16(0, 0151402);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[1] == 1);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7f;
    regs.d[2] = 0x2;
    raw_write16(0, 0151402);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.d[1] == 0x81);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7f;
    regs.d[2] = 0x1;
    raw_write16(0, 0151402);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x80);
}

BOOST_DATA_TEST_CASE(z, BIT *BIT, old_z, new_z) {
    regs.z = old_z;
    regs.d[1] = 0xfe;
    regs.d[2] = 1;
    regs.x = new_z;
    raw_write16(0, 0151402);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (new_z ? 0 : 0xff));
    BOOST_TEST(regs.z == (new_z ? old_z : false));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(Reg, bdata::xrange(1), x) {
    regs.d[2] = 1111;
    regs.d[3] = 2222;
    regs.x = x;
    raw_write16(0, 0152503);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == (3333 + x));
}

BOOST_DATA_TEST_CASE(Memory, bdata::xrange(1), x) {
    regs.a[2] = 0x1002;
    regs.a[3] = 0x2002;
    raw_write16(0x1000, 2222);
    raw_write16(0x2000, 3333);
    regs.x = x;
    raw_write16(0, 0152513);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x1000) == (5555 + x));
    BOOST_TEST(regs.a[2] == 0x1000);
    BOOST_TEST(regs.a[3] == 0x2000);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffff;
    regs.d[2] = 2;
    raw_write16(0, 0151502);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.d[1] == 1);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fff;
    regs.d[2] = 0x2;
    raw_write16(0, 0151502);
    m68k_do_execute();
    BOOST_TEST(regs.v);
     BOOST_TEST(regs.d[1] == 0x8001);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fff;
    regs.d[2] = 0x1;
    raw_write16(0, 0151502);
    m68k_do_execute();
    BOOST_TEST(regs.n);
        BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_DATA_TEST_CASE(z, BIT *BIT, old_z, new_z) {
    regs.z = old_z;
    regs.d[1] = 0xfffe;
    regs.d[2] = 1;
    regs.x = new_z;
    raw_write16(0, 0151502);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (new_z ? 0 : 0xffff));
    BOOST_TEST(regs.z == (new_z ? old_z : false));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(Reg, bdata::xrange(1), x) {
    regs.d[2] = 111111;
    regs.d[3] = 222222;
    regs.x = x;
    raw_write16(0, 0152603);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == (333333 + x));
}

BOOST_DATA_TEST_CASE(Memory, bdata::xrange(1), x) {
    regs.a[2] = 0x1004;
    regs.a[3] = 0x2004;
    raw_write32(0x1000, 222222);
    raw_write32(0x2000, 333333);
    regs.x = x;
    raw_write16(0, 0152613);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x1000) == (555555 + x));
    BOOST_TEST(regs.a[2] == 0x1000);
    BOOST_TEST(regs.a[3] == 0x2000);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffffffff;
    regs.d[2] = 2;
    raw_write16(0, 0151602);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST( regs.d[1] == 1);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fffffff;
    regs.d[2] = 0x2;
    raw_write16(0, 0151602);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST( regs.d[1] == 0x80000001);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fffffff;
    regs.d[2] = 0x1;
    raw_write16(0, 0151602);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST( regs.d[1] == 0x80000000);
}

BOOST_DATA_TEST_CASE(z, BIT *BIT, old_z, new_z) {
    regs.z = old_z;
    regs.d[1] = 0xfffffffe;
    regs.d[2] = 1;
    regs.x = new_z;
    raw_write16(0, 0151602);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (new_z ? 0 : 0xffffffff));
    BOOST_TEST(regs.z == (new_z ? old_z : false));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
