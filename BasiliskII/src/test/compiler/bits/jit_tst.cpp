#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(TST)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(n, BIT, n) {
    regs.x = regs.v = regs.c = true;
    regs.d[1] = n << 7;
    raw_write16(0, 0045001);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n == n);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_DATA_TEST_CASE(z, BIT, z) {
    regs.d[1] = z ? 0 : 1;
    raw_write16(0, 0045001);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z == z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(n, BIT, n) {
    regs.x = regs.v = regs.c = true;
    regs.d[1] = n << 15;
    raw_write16(0, 0045101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n == n);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_DATA_TEST_CASE(z, BIT, z) {
    regs.d[1] = z ? 0 : 1;
    raw_write16(0, 0045101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z == z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(n, BIT, n) {
    regs.x = regs.v = regs.c = true;
    regs.d[1] = n << 31;
    raw_write16(0, 0045201);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n == n);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_DATA_TEST_CASE(z, BIT, z) {
    regs.d[1] = z ? 0 : 1;
    raw_write16(0, 0045201);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z == z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
