#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(DIVS, InitFix)
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    int32_t v1 = get_v32();
    int32_t v2 = get_v32();
    if(v2 == 0) {
        v2 = 1;
    }
    if(v1 == std::numeric_limits<int32_t>::min()) {
        v1 = 1;
    }
    auto [ea, dr, dq] = rand_reg3();
    regs.d[dq] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0046100 | ea);
    raw_write16(2, 0x0800 | dq << 12 | dr);
    m68k_do_execute();
    BOOST_TEST( regs.d[dq] == ( v1 / v2));
    BOOST_TEST( regs.d[dr] == ( v1 % v2));
}
BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = -2;
    regs.d[2] = 10;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2802);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = -12345;
    regs.d[2] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2802);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = -2;
    regs.d[2] = 2;
    regs.d[3] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x3C02);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(div0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2802);
    exception_check(5);
}

BOOST_AUTO_TEST_CASE(dword) {
    regs.d[1] = 30;
    regs.d[2] = 0;
    regs.d[3] = -1;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2C03);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == -16);
    BOOST_TEST(regs.d[2] == -143165576);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    int32_t v1 = get_v16();
    int16_t v2 = get_v16();
    if(v2 == 0) {
        v2 = 1;
    }
    auto [ea, dn] = rand_reg2();
    regs.d[dn] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0100700 | dn << 9 | ea);
    m68k_do_execute();
    BOOST_TEST( regs.d[dn] == (uint16_t( v1 / v2) | uint16_t( v1 % v2) << 16) );
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 100;
    regs.d[2] = 0;
    raw_write16(0, 0102701);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 2;
    regs.d[2] = -1000000;
    raw_write16(0, 0102701);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(div0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    raw_write16(0, 0102701);
    exception_check(5);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()