#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(BFTST)
BOOST_AUTO_TEST_SUITE(REG)
BOOST_AUTO_TEST_CASE(z) {
    regs.d[2] = 0xf00fffff;
    raw_write16(0, 0164302);
    raw_write16(2, 4 << 6 | 8);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[2] = 0xf80fffff;
    raw_write16(0, 0164302);
    raw_write16(2, 4 << 6 | 8);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(width_reg) {
    regs.d[2] = 0xf80fffff;
    regs.d[3] = 8;
    raw_write16(0, 0164302);
    raw_write16(2, 1 << 5 | 4 << 6 | 3);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(off_reg) {
    regs.d[2] = 0xf80fffff;
    regs.d[3] = 4;
    raw_write16(0, 0164302);
    raw_write16(2, 1 << 11 | 3 << 6 | 8);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(MEM)
BOOST_AUTO_TEST_CASE(n) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x18);
    raw_write16(0, 0164321);
    raw_write16(2, 4 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x10);
    raw_write16(0, 0164321);
    raw_write16(2, 4 << 6 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
