#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MULU, InitFix)
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    auto v1 = get_v32();
    auto v2 = get_v32();
    auto [ea, dh, dl] = rand_reg3();
    regs.d[dl] = v1;
    regs.d[ea] = v2;
    raw_write16(0, 0046000 | ea);
    raw_write16(2, 0x0400 | dl << 12 | dh);
    uint64_t mx = uint64_t(v1) * v2;
    m68k_do_execute();
    BOOST_TEST(regs.d[dh] == (uint32_t)(mx >> 32));
    BOOST_TEST(regs.d[dl] == (uint32_t)mx);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x10000;
    regs.d[2] = 0x10000;
    raw_write16(0, 0046001);
    raw_write16(2, 0x2000);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    auto [ea, dl] = rand_reg2();
    auto v1 = get_v16();
    auto v2 = get_v16();

    regs.d[ea] = v1;
    regs.d[dl] = v2;
    raw_write16(0, 0140300 | dl << 9 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[dl] == v1 * v2);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    raw_write16(0, 0142301);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
