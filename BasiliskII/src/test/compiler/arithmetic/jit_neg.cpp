#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(NEG)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 11;
    raw_write16(0, 0042002);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0xf5);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0042001);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0042001);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0042001);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    raw_write16(0, 0042001);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[3] = 1200;
    raw_write16(0, 0042103);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[3] == 0xfb50);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 200;
    raw_write16(0, 0042101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0042101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0042101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0042101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[1] = 100000;
    raw_write16(0, 0042201);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[1] == -100000);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0042201);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0042201);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.x);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0042201);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0042201);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()