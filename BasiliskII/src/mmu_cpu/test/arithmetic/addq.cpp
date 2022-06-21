#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ADDQ, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[1] = 11;
    raw_write16(0, 0053001);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 14);
}
BOOST_AUTO_TEST_CASE(eight) {
    regs.d[1] = 11;
    raw_write16(0, 0050001);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 19);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xff;
    raw_write16(0, 0052001);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.d[1] == 1);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7f;
    raw_write16(0, 0052001);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.d[1] == 0x81);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xff;
    raw_write16(0, 0051001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7f;
    raw_write16(0, 0051001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x80);
}

BOOST_AUTO_TEST_CASE(err_an) {
    raw_write16(0, 0051011);
    raw_write16(2, 0);
    exception_check(4);
}

BOOST_AUTO_TEST_CASE(err_imm) {
    raw_write16(0, 0051074);
    raw_write16(2, 0);
    exception_check(4);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[1] = 1111;
    raw_write16(0, 0053101);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 1114);
}
BOOST_AUTO_TEST_CASE(eight) {
    regs.d[1] = 1111;
    raw_write16(0, 0050101);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 1119);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0052101);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.d[1] == 1);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fff;
    raw_write16(0, 0052101);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.d[1] == 0x8001);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0051101);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fff;
    raw_write16(0, 0051101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x8000);
}

BOOST_AUTO_TEST_CASE(err_imm) {
    raw_write16(0, 0051174);
    raw_write16(2, 0);
    exception_check(4);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(AWord)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[2] = 0xffff0000;
    raw_write16(0, 0052112);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0xffff0002);
}
BOOST_AUTO_TEST_CASE(eight) {
    regs.a[2] = 0xffff0000;
    raw_write16(0, 0050112);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0xffff0008);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[1] = 111111;
    raw_write16(0, 0053201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 111114);
}
BOOST_AUTO_TEST_CASE(eight) {
    regs.d[1] = 111111;
    raw_write16(0, 0050201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 111119);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0052201);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.d[1] == 1);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fffffff;
    raw_write16(0, 0052201);
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.d[1] == 0x80000001);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0051201);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[1] == 0);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fffffff;
    raw_write16(0, 0051201);
    m68k_do_execute();
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.d[1] == 0x80000000);
}

BOOST_AUTO_TEST_CASE(err_imm) {
    raw_write16(0, 0051274);
    raw_write16(2, 0);
    exception_check(4);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ALong)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[2] = 0x10000000;
    raw_write16(0, 0052212);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10000002);
}
BOOST_AUTO_TEST_CASE(eight) {
    regs.a[2] = 0x10000000;
    raw_write16(0, 0050212);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10000008);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()