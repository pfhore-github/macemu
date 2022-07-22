#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(ADDI)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 11;
    raw_write16(0, 0003002);
    raw_write16(2, 22);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 33);
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xff;
    raw_write16(0x10, 0003001);
    raw_write16(0x12, 2);
    regs.pc = 0x10;
    jit_compile(0x10, 0x12);
    jit_jump(0x10);
    BOOST_TEST( regs.x );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.d[1] == 1);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7f;
    raw_write16(0, 0003001);
    raw_write16(2, 2);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.v );
    BOOST_TEST( regs.d[1] == 0x81);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xff;
    raw_write16(0, 0003001);
    raw_write16(2, 1);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.z );
    BOOST_TEST( regs.d[1] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7f;
    raw_write16(0, 0003001);
    raw_write16(2, 1);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.n );
    BOOST_TEST( regs.d[1] == 0x80);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 1111;
    raw_write16(0, 0003102);
    raw_write16(2, 2222);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 3333);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0003101);
    raw_write16(2, 2);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.x );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.d[1] == 1);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fff;
    raw_write16(0, 0003101);
    raw_write16(2, 2);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.v );
    BOOST_TEST( regs.d[1] == 0x8001);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0003101);
    raw_write16(2, 1);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.z );
    BOOST_TEST( regs.d[1] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fff;
    raw_write16(0, 0003101);
    raw_write16(2, 1);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.n );
    BOOST_TEST( regs.d[1] == 0x8000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(operand) {
    regs.d[2] = 111111;
    raw_write16(0, 0003202);
    raw_write32(2, 222222);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.d[2] == 333333);
}


BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0003201);
    raw_write32(2, 2);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.x );
    BOOST_TEST( regs.c );
    BOOST_TEST(regs.d[1] == 1);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fffffff;
    raw_write16(0, 0003201);
    raw_write32(2, 2);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.v );
    BOOST_TEST(regs.d[1] == 0x80000001);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0003201);
    raw_write32(2, 1);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.z );
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fffffff;
    raw_write16(0, 0003201);
    raw_write32(2, 1);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.n );
    BOOST_TEST(regs.d[1] == 0x80000000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()