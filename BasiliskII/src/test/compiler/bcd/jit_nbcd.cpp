#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(NBCD)

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[3] = 0x27;
    regs.z = true;
    regs.x = true;
    raw_write16(0, 0044003);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0x72);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.z = true;
    regs.x = false;
    raw_write16(0, 0044001);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.x);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()