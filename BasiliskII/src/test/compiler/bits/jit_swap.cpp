#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(SWAP)

BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 0x12345678;
    regs.z = true;
    regs.c = true;
    raw_write16(0, 0044102);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0x56781234);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0044101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}


BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0044101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()