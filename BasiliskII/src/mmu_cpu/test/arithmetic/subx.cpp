#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(SUBX, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(Reg, bdata::xrange(1), x) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.d[yr] = v1;
    regs.d[xr] = v2;
    regs.x = x;
    raw_write16(0, 0110400 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[yr] == ((v1 - v2 - x) & 0xff));
}

BOOST_DATA_TEST_CASE(Memory, bdata::xrange(1), x) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.a[yr] = 0x1001;
    regs.a[xr] = 0x2001;
    raw_write8(0x1000, v1);
    raw_write8(0x2000, v2);
    regs.x = x;
    raw_write16(0, 0110410 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x1000) == ((v1 - v2 - x) & 0xff));
    BOOST_TEST(regs.a[yr] == 0x1000);
    BOOST_TEST(regs.a[xr] == 0x2000);
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    regs.d[2] = 2;
    raw_write16(0, 0110400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    regs.d[2] = 0x1;
    raw_write16(0, 0110400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfe;
    regs.d[2] = 0x1;
    raw_write16(0, 0110400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.z = true;
    regs.d[1] = 2;
    regs.d[2] = 1;
    raw_write16(0, 0110400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(Reg, bdata::xrange(1), x) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.d[yr] = v1;
    regs.d[xr] = v2;
    regs.x = x;
    raw_write16(0, 0110500 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[yr] == ((v1 - v2 - x) & 0xffff));
}

BOOST_DATA_TEST_CASE(Memory, bdata::xrange(1), x) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.a[yr] = 0x1002;
    regs.a[xr] = 0x2002;
    raw_write16(0x1000, v1);
    raw_write16(0x2000, v2);
    regs.x = x;
    raw_write16(0, 0110510 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x1000) == ((v1 - v2 - x) & 0xffff));
    BOOST_TEST(regs.a[yr] == 0x1000);
    BOOST_TEST(regs.a[xr] == 0x2000);
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    regs.d[2] = 2;
    raw_write16(0, 0110500 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0x1;
    raw_write16(0, 0110500 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffe;
    regs.d[2] = 0x1;
    raw_write16(0, 0110500 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.z = true;
    regs.d[1] = 2;
    regs.d[2] = 1;
    raw_write16(0, 0110500 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(Reg, bdata::xrange(1), x) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[yr] = v1;
    regs.d[xr] = v2;
    regs.x = x;
    raw_write16(0, 0110600 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(regs.d[yr] == (v1 - v2 - x));
}

BOOST_DATA_TEST_CASE(Memory, bdata::xrange(1), x) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.a[yr] = 0x1004;
    regs.a[xr] = 0x2004;
    raw_write32(0x1000, v1);
    raw_write32(0x2000, v2);
    regs.x = x;
    raw_write16(0, 0110610 | yr << 9 | xr);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x1000) == (v1 - v2 - x));
    BOOST_TEST(regs.a[yr] == 0x1000);
    BOOST_TEST(regs.a[xr] == 0x2000);
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    regs.d[2] = 2;
    raw_write16(0, 0110600 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0x1;
    raw_write16(0, 0110600 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffffffe;
    regs.d[2] = 0x1;
    raw_write16(0, 0110600 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.z = true;
    regs.d[1] = 2;
    regs.d[2] = 1;
    raw_write16(0, 0110600 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
