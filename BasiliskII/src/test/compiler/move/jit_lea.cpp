#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(LEA)

BOOST_AUTO_TEST_CASE(ill_an) {
    raw_write16(0, 0043712);
    jit_compile(0, 2);
    jit_exception_check(4);
}

BOOST_AUTO_TEST_CASE(an) {
    regs.a[2] = 0x1000;
    raw_write16(0, 0043722);
    jit_compile(0, 2);
    jit_jump(0);

    BOOST_TEST(regs.a[3] == 0x1000);
}

BOOST_AUTO_TEST_CASE(ill_incr) {
    raw_write16(0, 0043732);
    jit_compile(0, 2);
    jit_exception_check(4);
}

BOOST_AUTO_TEST_CASE(ill_decr) {
    raw_write16(0, 0043742);
    jit_compile(0, 2);
    jit_exception_check(4);
}

BOOST_AUTO_TEST_CASE(d16) {
    regs.a[2] = 0x1000;
    raw_write16(0, 0043752);
    raw_write16(2, -100);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == 0x1000 - 100);
}

BOOST_AUTO_TEST_CASE(xn_w) {
    regs.d[3] = (-6) & 0xffff;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | (-16 & 0xff));
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == 0x1000 - 6 - 16);
}

BOOST_AUTO_TEST_CASE(xn_l) {
    regs.d[3] = 10;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == 0x1000 + 10 + 4);
}

BOOST_DATA_TEST_CASE(xn_cc, bdata::xrange(4), c) {
    regs.d[3] = 8;
    regs.a[2] = 0x1000;
    raw_write16(0, 0044762);
    raw_write16(2, 3 << 12 | 1 << 11 | c << 9 | 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[4] == 0x1004 + (8 << c));
}

BOOST_AUTO_TEST_CASE(ill_bit3) {
    raw_write16(0, 0044762);
    raw_write16(2, 1 << 8 | 1 << 3);
    jit_compile(0, 2);
    jit_exception_check(4);
}

BOOST_AUTO_TEST_CASE(bd_err) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 1 << 8 | 0 << 4);
    jit_compile(0, 2);
    jit_exception_check(4);
}

BOOST_AUTO_TEST_CASE(bd_z) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 1 << 8 | 1 << 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == 0x1000 + 4);
}

BOOST_AUTO_TEST_CASE(bd_w) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 1 << 8 | 2 << 4);
    raw_write16(4, -60);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == 0x1000 + 4 - 60);
}

BOOST_AUTO_TEST_CASE(bd_l) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 1 << 8 | 3 << 4);
    raw_write32(4, 1000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == 0x1000 + 4 + 1000);
}

BOOST_DATA_TEST_CASE(ai, BIT, ai) {
    regs.d[3] = 10;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 3 << 12 | 1 << 11 | 1 << 8 | ai << 7 | 1 << 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == (ai ? 0 : 0x1000) + 10);
}

BOOST_DATA_TEST_CASE(ri, BIT, ri) {
    regs.d[4] = 10;
    regs.a[2] = 0x1000;
    raw_write16(0, 0043762);
    raw_write16(2, 4 << 12 | 1 << 11 | 1 << 8 | ri << 6 | 1 << 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == 0x1000 + (ri ? 0 : 10));
}

BOOST_AUTO_TEST_CASE(post_od_z) {
    regs.d[2] = 10;
    regs.a[3] = 0x1000;
    raw_write16(0, 0044763);
    raw_write16(2, 2 << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | 1);
    raw_write16(4, 0x100);
    raw_write32(0x1000 + 10 * 4 + 0x100, 0x2000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[4] == 0x2000);
}

BOOST_AUTO_TEST_CASE(post_od_w) {
    regs.d[2] = 10;
    regs.a[3] = 0x1000;
    raw_write16(0, 0044763);
    raw_write16(2, 2 << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | 2);
    raw_write16(4, 0x100);
    raw_write16(6, -40);
    raw_write32(0x1000 + 10 * 4 + 0x100, 0x2000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[4] == 0x2000 - 40);
}

BOOST_AUTO_TEST_CASE(post_od_l) {
    regs.d[2] = 10;
    regs.a[3] = 0x1000;
    raw_write16(0, 0044763);
    raw_write16(2, 2 << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | 3);
    raw_write16(4, 0x100);
    raw_write32(6, 2000);
    raw_write32(0x1000 + 10 * 4 + 0x100, 0x2000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[4] == 0x2000 + 2000);
}

BOOST_AUTO_TEST_CASE(pre_od_err) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0044762);
    raw_write16(2, 3 << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | 1 << 2 | 0);
    raw_write16(4, 0x100);
    raw_write32(0x1000 + 0x100, 0x2000);
    jit_compile(0, 2);
    jit_exception_check(4);
}

BOOST_AUTO_TEST_CASE(pre_wo_an_ri) {
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write16(0, 0044762);
    raw_write16(2, 1 << 8 | 3 << 6 | 2 << 4 | 1 << 2 | 1);
    raw_write16(4, 0x100);
    raw_write32(0x1000 + 0x100, 0x2000);
    jit_compile(0, 2);
    jit_exception_check(4);
}

BOOST_AUTO_TEST_CASE(pre_od_z) {
    regs.d[2] = 10;
    regs.a[3] = 0x1000;
    raw_write16(0, 0044763);
    raw_write16(2, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 1);
    raw_write16(4, 0x100);
    raw_write32(0x1000 + 0x100, 0x2000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[4] == 0x2000 + 10);
}

BOOST_AUTO_TEST_CASE(pre_od_w) {
    regs.d[2] = 10;
    regs.a[3] = 0x1000;
    raw_write16(0, 0044763);
    raw_write16(2, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 2);
    raw_write16(4, 0x100);
    raw_write16(6, -100);
    raw_write32(0x1000 + 0x100, 0x2000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[4] == 0x2000 + 10 - 100);
}

BOOST_AUTO_TEST_CASE(pre_od_l) {
    regs.d[2] = 10;
    regs.a[3] = 0x1000;
    raw_write16(0, 0044763);
    raw_write16(2, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4 | 1 << 2 | 3);
    raw_write16(4, 0x100);
    raw_write32(6, 100);
    raw_write32(0x1000 + 0x100, 0x2000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[4] == 0x2000 + 10 + 100);
}

BOOST_AUTO_TEST_CASE(imm_w) {
    raw_write16(0, 0043770);
    raw_write16(2, -200);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == -200);
}

BOOST_AUTO_TEST_CASE(imm_l) {
    raw_write16(0, 0043771);
    raw_write32(2, 0x10000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[3] == 0x10000);
}

BOOST_AUTO_TEST_CASE(pc_d16) {
    regs.pc = 0x1000;
    raw_write16(0x1000, 0043772);
    raw_write16(0x1002, -100);
    jit_compile(0x1000, 0x1002);
    jit_jump(0x1000);
    BOOST_TEST(regs.a[3] == 0x1002 - 100);
}

BOOST_AUTO_TEST_CASE(pc_brief) {
    regs.d[2] = 5;
    regs.pc = 0x1000;
    raw_write16(0x1000, 0043773);
    raw_write16(0x1002, 2 << 12 | 1 << 11 | (-6 & 0xff));
    jit_compile(0x1000, 0x1002);
    jit_jump(0x1000);
    BOOST_TEST(regs.a[3] == (0x1002 - 6 + 5));
}

BOOST_AUTO_TEST_CASE(pc_extend) {
    regs.d[2] = 5;
    regs.pc = 0x1000;
    raw_write16(0x1000, 0043773);
    raw_write16(0x1002, 2 << 12 | 1 << 11 | 1 << 8 | 2 << 4);
    raw_write16(0x1004, 100);
    jit_compile(0x1000, 0x1002);
    jit_jump(0x1000);
    BOOST_TEST(regs.a[3] == 0x1002 + 5 + 100);
}

BOOST_AUTO_TEST_CASE(pc_post) {
    regs.d[2] = 5;
    regs.pc = 0x1000;
    raw_write16(0x1000, 0043773);
    raw_write16(0x1002, 2 << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | 2);
    raw_write16(0x1004, 0x100);
    raw_write16(0x1006, 0x200);
    raw_write32(0x1002 + 5 * 4 + 0x100, 0x2000);
    jit_compile(0x1000, 0x1002);
    jit_jump(0x1000);
    BOOST_TEST(regs.a[3] == 0x2000 + 0x200);
}

BOOST_AUTO_TEST_CASE(pc_pre) {
    regs.d[3] = 5;
    regs.pc = 0x1000;
    raw_write16(0x1000, 0043773);
    raw_write16(0x1002,
                3 << 12 | 1 << 11 | 2 << 9 | 1 << 8 | 2 << 4 | 1 << 2 | 2);
    raw_write16(0x1004, 0x100);
    raw_write16(0x1006, 0x200);
    raw_write32(0x1002 + 0x100, 0x2000);
    jit_compile(0x1000, 0x1002);
    jit_jump(0x1000);
    BOOST_TEST(regs.a[3] == 0x2000 + 5 * 4 + 0x200);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()