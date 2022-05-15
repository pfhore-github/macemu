#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>
BOOST_FIXTURE_TEST_SUITE(SWAP, InitFix)

BOOST_AUTO_TEST_CASE(operand) {
    auto dn = rand_reg();
    uint32_t v = get_v32();
    regs.d[dn] = v;
    regs.z = true;
    regs.c = true;
    raw_write16(0, 0044100 | dn);
    m68k_do_execute();
    BOOST_TEST(regs.d[dn] == std::rotl<uint32_t>(v, 16));
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0044101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}


BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0044101);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()