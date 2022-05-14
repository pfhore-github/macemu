#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(EOR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)


BOOST_AUTO_TEST_CASE(operand) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.x = regs.v = regs.c = true;
    regs.d[ea] = v1;
    regs.d[dn] = v2;
    raw_write16(0, 0130400 | dn << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (v1^v2));
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80;
    regs.d[2] = 0;
    raw_write16(0, 0130400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0;
    raw_write16(0, 0130400 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    auto [dn, ea] = rand_reg2();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0130500 | dn << 9 | ea);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (v1 ^ v2));
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    regs.d[2] = 0;
    raw_write16(0, 0130500 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0;
    raw_write16(0, 0130500 | 1 << 9 | 2);
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
    raw_write16(0, 0130600 | dn << 9 | ea);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (v1 ^ v2));
    BOOST_TEST( regs.x);
    BOOST_TEST( !regs.c);
    BOOST_TEST( !regs.v);
}


BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80000000;
    regs.d[2] = 0;
    raw_write16(0, 0130600 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.d[2] = 0;
    raw_write16(0, 0130600 | 1 << 9 | 2);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()