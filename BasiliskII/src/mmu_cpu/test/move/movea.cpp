#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(MOVEA, InitFix)

BOOST_AUTO_TEST_CASE(Word) {
    auto dst = rand_ar();
    auto src = rand_reg();
    int16_t v1 = get_v16();
    regs.d[src] = v1 & 0xffff;
    raw_write16(0, 0030100 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.a[dst] == (int32_t)v1);
}

BOOST_AUTO_TEST_CASE(Long) {
    auto dst = rand_ar();
    auto src = rand_reg();
    auto v1 = get_v32();
    regs.d[src] = v1;
    raw_write16(0, 0020100 | dst << 9 | src);
    m68k_do_execute();
    BOOST_TEST(regs.a[dst] == v1);
}
BOOST_AUTO_TEST_SUITE_END()