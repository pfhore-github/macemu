#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(EORI)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[1] = 0x23;
    raw_write16(0, 0005001);
    raw_write16(2, 0xff);
    regs.x = regs.v = regs.c = true;
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[1] == 0xDC);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80;
    raw_write16(0, 0005001);
    raw_write16(2, 0);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x80;
    raw_write16(0, 0005001);
    raw_write16(2, 0x80);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}

BOOST_DATA_TEST_CASE(ccr, BIT *BIT *BIT *BIT *BIT, x1, v1, c1, n1, z1) {
    raw_write16(0, 0005074);
    raw_write16(2, x1 << 4 | n1 << 3 | z1 << 2 | v1 << 1 | c1);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.x == x1);
    BOOST_TEST(regs.v == v1);
    BOOST_TEST(regs.c == c1);
    BOOST_TEST(regs.n == n1);
    BOOST_TEST(regs.z == z1);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 0x5432;
    raw_write16(0, 0005102);
    raw_write16(2, 0xffff);
    regs.x = regs.v = regs.c = true;
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 0xABCD);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0005101);
    raw_write16(2, 0x0000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0005101);
    raw_write16(2, 0x8000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(sr_invalid) {
    regs.S = false;
    raw_write16(0, 0005174);
    raw_write16(2, 0xffff);
    jit_compile(0, 2);
    jit_exception_check(8);
}

BOOST_AUTO_TEST_CASE(sr_valid) {
    regs.S = true;
    regs.M = false;
    regs.IM = 7;
    raw_write16(0, 0005174);
    raw_write16(2, 0x2f00);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(!regs.S);
    BOOST_TEST(regs.IM == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(operand) {
    regs.d[1] = 0x12345678;
    raw_write16(0, 0005201);
    raw_write32(2, 0xffffffff);
    regs.x = regs.v = regs.c = true;
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[1] == 0xEDCBA987);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0005201);
    raw_write32(2, 0x00000000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0005201);
    raw_write32(2, 0x80000000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()