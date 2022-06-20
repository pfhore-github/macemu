#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(EXTB, InitFix)
BOOST_AUTO_TEST_CASE(positive) {
    regs.d[1] = 0x20;
    raw_write16(0, 0044701);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x20);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 0;
    raw_write16(0, 0044701);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.n);
}
BOOST_AUTO_TEST_CASE(neg) {
    uint8_t v1 = 0xc0;
    regs.d[1] = v1;
    raw_write16(0, 0044701);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffffc0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()