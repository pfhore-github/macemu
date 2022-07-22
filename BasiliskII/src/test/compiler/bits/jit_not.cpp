#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(NOT)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 0x12;
    raw_write16(0, 0043002);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0xED);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0043001);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xff;
    raw_write16(0, 0043001);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 0x1234;
    raw_write16(0, 0043102);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0xEDCB);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0043101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0043101);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 0x12345678;
    raw_write16(0, 0043202);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0xEDCBA987);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0043201);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0043201);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
