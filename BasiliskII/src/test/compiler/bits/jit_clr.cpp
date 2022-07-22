#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(CLR)
BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[2] = 44;
    regs.x = regs.c = regs.v = regs.n = true;
    raw_write16(0, 0041002);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.n);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[2] = 4400;
    regs.x = regs.c = regs.v = regs.n = true;
    raw_write16(0, 0041102);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.n);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[2] = 4400000;
    regs.x = regs.c = regs.v = regs.n = true;
    raw_write16(0, 0041202);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.n);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()