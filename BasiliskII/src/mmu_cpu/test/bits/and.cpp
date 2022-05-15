#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(AND, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(ToReg) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0140000 | dn << 9 | ea);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[dn] == (v1 & v2));
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.a[ea] = 0x10;
    regs.d[dn] = v1;
    raw_write16(0, 0140420 | dn << 9 | ea);
    raw_write8(0x10, v2);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == (v1&v2));
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80;
    regs.d[2] = 0x80;
    raw_write16(0, 0140000 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0x80;
    raw_write16(0, 0140000 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(ToReg) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0140100 | dn << 9 | ea);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[dn] == (v1 & v2));
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    auto [xr, yr] = rand_reg2();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.a[xr] = 0x10;
    regs.d[yr] = v1;
    raw_write16(0, 0140520 | yr << 9 | xr);
    raw_write16(0x10, v2);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == (v1&v2));
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0x8000;
    raw_write16(0, 0140100 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0x8000;
    raw_write16(0, 0140100 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(ToReg) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0140200 | dn << 9 | ea);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[dn] == (v1 & v2));
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}

BOOST_AUTO_TEST_CASE(ToMem) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.a[ea] = 0x10;
    regs.d[dn] = v1;
    raw_write16(0, 0140620 | dn << 9 | ea);
    raw_write32(0x10, v2);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == (v1&v2));
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0x80000000;
    raw_write16(0, 0140200 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0x80000000;
    raw_write16(0, 0140200 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
