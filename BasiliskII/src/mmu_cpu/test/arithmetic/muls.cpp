#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MULS, InitFix)
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    int32_t v1 = get_v32();
    int32_t v2 = get_v32();
    auto [ea, dh, dl] = rand_reg3();
    regs.d[dl] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0046000 | ea);
    raw_write16(2, 0x0C00 | dl << 12 | dh);
    int64_t mx = int64_t(v1) * v2;
    m68k_do_execute();
    BOOST_TEST( regs.d[dh] == (uint32_t)(mx >> 32));
    BOOST_TEST( regs.d[dl] == (uint32_t)mx);
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
    regs.d[1] = -(0x10000);
    regs.d[2] = -(0x10000);
    raw_write16(0, 0046001);
    raw_write16(2, 0x2800);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(in_range) {
    auto [ea, dl] = rand_reg2();
    regs.d[ea] = -1234;
    regs.d[dl] = 3456;
    raw_write16(0, 0140700 | dl << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(static_cast<int32_t>(regs.d[dl]) == -1234 * 3456);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    raw_write16(0, 0142701);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()