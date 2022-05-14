#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MOVEQ, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    auto rg = rand_reg();
    auto v = get_v8() & 0x7f;
    int32_t vx = (v & 0x40 ? 0xffffff80 : 0) | v;
    raw_write16(0, 0x7000 | rg << 9 | v);
    m68k_do_execute();
    BOOST_TEST(regs.d[rg] == vx);
}
BOOST_AUTO_TEST_CASE(n) {
    raw_write16(0, 0071377);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == -1);
}

BOOST_AUTO_TEST_CASE(z) {
    raw_write16(0, 0071000);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}


BOOST_AUTO_TEST_SUITE_END()