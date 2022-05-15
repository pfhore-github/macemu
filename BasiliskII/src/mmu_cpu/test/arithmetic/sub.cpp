
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(SUB, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(ToReg) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0110000 | dn << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[dn] == ((v1 - v2) & 0xff));
}

BOOST_AUTO_TEST_CASE(ToMem) {
    int dn = rand_reg();
    int ea = rand_reg();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.a[ea] = 0x10;
    regs.d[dn] = v1;
    raw_write16(0, 0110420 | dn << 9 | ea);
    raw_write8(0x10, v2);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == ((v2 - v1) & 0xff));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[0] = 1;
    regs.d[1] = 2;
    raw_write16(0, 0110000 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[0] = 0x80;
    regs.d[1] = 1;
    raw_write16(0, 0110000 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[0] = 1;
    regs.d[1] = 1;
    raw_write16(0, 0110000 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[0] = 0xf3;
    regs.d[1] = 1;
    raw_write16(0, 0110000 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(ToReg) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0110100 | dn << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[dn] == ((v1 - v2) & 0xffff));
}

BOOST_AUTO_TEST_CASE(ToMem) {
    int dn = rand_reg();
    int ea = rand_reg();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.a[ea] = 0x10;
    regs.d[dn] = v1;
    raw_write16(0, 0110520 | dn << 9 | ea);
    raw_write16(0x10, v2);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == ((v2 - v1) & 0xffff));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[0] = 1;
    regs.d[1] = 2;
    raw_write16(0, 0110100 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[0] = 0x8000;
    regs.d[1] = 1;
    raw_write16(0, 0110100 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[0] = 1;
    regs.d[1] = 1;
    raw_write16(0, 0110100 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[0] = 0xfffe;
    regs.d[1] = 1;
    raw_write16(0, 0110100 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(ToReg) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0110200 | dn << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[dn] == (v1 - v2));
}

BOOST_AUTO_TEST_CASE(ToMem) {
    int dn = rand_reg();
    int ea = rand_reg();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.a[ea] = 0x10;
    regs.d[dn] = v1;
    raw_write16(0, 0110620 | dn << 9 | ea);
    raw_write32(0x10, v2);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == (v2 - v1));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[0] = 1;
    regs.d[1] = 2;
    raw_write16(0, 0110200 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[0] = 0x80000000;
    regs.d[1] = 1;
    raw_write16(0, 0110200 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[0] = 1;
    regs.d[1] = 1;
    raw_write16(0, 0110200 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[0] = 0xfffffffe;
    regs.d[1] = 1;
    raw_write16(0, 0110200 | 0 << 9 | 1);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
