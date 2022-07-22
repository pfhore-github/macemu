#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(RTE)
BOOST_AUTO_TEST_CASE(format0) {
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    PUSH16(0);
    PUSH32(0x1000);
    PUSH16(1);
    raw_write16(0, 0047163);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[7] == 0x2000);
    BOOST_TEST(!regs.S);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.pc == 0x1000);
}

BOOST_AUTO_TEST_CASE(format1) {
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    PUSH16(0);
    PUSH32(0x1200);
    PUSH16(0);
    PUSH16(0x1000);
    PUSH32(0x1000);
    PUSH16(1 << 13);
    raw_write16(0, 0047163);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(!regs.S);
    BOOST_TEST(regs.a[7] == 0x2000);
    BOOST_TEST(regs.pc == 0x1200);
}

BOOST_AUTO_TEST_CASE(format2) {
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    PUSH16(0);
    PUSH32(0x1200);
    PUSH16(0);
    PUSH16(0x1000);
    PUSH32(0x1000);
    PUSH16(1 << 13);
    raw_write16(0, 0047163);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(!regs.S);
    BOOST_TEST(regs.a[7] == 0x2000);
    BOOST_TEST(regs.pc == 0x1200);
}

BOOST_AUTO_TEST_CASE(format3) {
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    PUSH32(0xffff);
    PUSH16(0x3fff);
    PUSH32(0x1200);
    PUSH16(0);
    raw_write16(0, 0047163);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(!regs.S);
    BOOST_TEST(regs.a[7] == 0x2000);
    BOOST_TEST(regs.pc == 0x1200);
}

BOOST_AUTO_TEST_CASE(format7_normal) {
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    regs.a[7] -= 2 * 23;
    PUSH16(0);
    PUSH32(0xffff);
    PUSH16(0x7fff);
    PUSH32(0x1200);
    PUSH16(0);
    raw_write16(0, 0047163);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(!regs.S);
    BOOST_TEST(regs.a[7] == 0x2000);
    BOOST_TEST(regs.pc == 0x1200);
}

BOOST_AUTO_TEST_CASE(format7_movem) {
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    regs.a[7] -= 2 * 23;
    PUSH16(1 << 12);
    PUSH32(0xffff);
    PUSH16(0x7fff);
    PUSH32(0x1200);
    PUSH16(0);
    raw_write16(0, 0047163);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(!regs.S);
    BOOST_TEST(regs.a[7] == 0x2000);
    BOOST_TEST(regs.pc == 0x1200);
}

BOOST_AUTO_TEST_CASE(formatx) {
    regs.S = true;
    regs.usp = 0x2000;
    regs.isp = regs.a[7] = 0x1000;
    PUSH16(0x5fff);
    PUSH32(0x1200);
    PUSH16(0);
    raw_write16(0, 0047163);

    regs.vbr = 0x3000;
    raw_write32(0x3000 + 14 * 4, 0x5000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.S);
    BOOST_TEST(regs.pc == 0x5000);
    BOOST_TEST((raw_read16(regs.a[7] + 6) & 0xfff) == 14 * 4);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()