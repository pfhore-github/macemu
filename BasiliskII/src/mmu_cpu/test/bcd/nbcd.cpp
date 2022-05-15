#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(NBCD, InitFix)

BOOST_AUTO_TEST_CASE(cx) {
    auto ea = rand_reg();
    regs.d[ea] = 0x27;
    regs.z = true;
    regs.x = true;
    raw_write16(0, 0044000 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0x72);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.z = true;
    regs.x = false;
    raw_write16(0, 0044001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.x);
}
BOOST_AUTO_TEST_SUITE_END()