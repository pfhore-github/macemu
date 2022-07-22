#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(ROL)
#if 0
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x8B;
    raw_write16(0, 0162432);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x2E);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x8B;
    raw_write16(0, 0160432);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x8B);
}
BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[1] = 0x8B;
    regs.d[3] = 2;
    raw_write16(0, 0163471);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x2E);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[1] = 0x8B;
    regs.d[3] = 0;
    regs.c = true;
    raw_write16(0, 0163471);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x8B);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x39;
    regs.d[2] = 68;
    raw_write16(0, 0162471);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x93);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0x80;
    raw_write16(0, 0161431);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40;
    raw_write16(0, 0161431);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161431);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE(Word)
#if 0
BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[2] = 0x1234;
    raw_write16(0, 0164532);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x2341);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[2] = 0x1234;
    raw_write16(0, 0160532);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x3412);
}
BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[2] = 0x1234;
    regs.d[3] = 4;
    raw_write16(0, 0163572);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x2341);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[2] = 0x1234;
    regs.d[3] = 0;
    regs.c = true;
    raw_write16(0, 0163572);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x1234);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x3456;
    regs.d[2] = 68;
    raw_write16(0, 0162571);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x4563);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0161531);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x4000;
    raw_write16(0, 0161531);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161531);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
#endif
BOOST_AUTO_TEST_CASE(ea) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0x4000);
    raw_write16(0, 0163722);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read16(0x10) = 0x8000);
}

BOOST_AUTO_TEST_SUITE_END()
#if 0
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(Imm) {
    regs.d[1] = 0x12345678;
    raw_write16(0, 0164631);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x23456781);
}

BOOST_AUTO_TEST_CASE(Imm8) {
    regs.d[1] = 0x12345678;
    raw_write16(0, 0160631);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x34567812);
}

BOOST_AUTO_TEST_CASE(Reg) {
    regs.d[2] = 0x12345678;
    regs.d[3] = 4;
    raw_write16(0, 0163672);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x23456781);
}

BOOST_AUTO_TEST_CASE(Reg0) {
    regs.d[2] = 0x12345678;
    regs.d[3] = 0;
    regs.c = true;
    raw_write16(0, 0163672);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12345678);
    BOOST_TEST(!regs.c);
}

BOOST_AUTO_TEST_CASE(RegOver) {
    regs.d[1] = 0x3456789A;
    regs.d[2] = 68;
    raw_write16(0, 0162671);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x456789A3);
}

BOOST_AUTO_TEST_CASE(c) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0161631);
    m68k_do_execute();
    BOOST_TEST(regs.c);
}


BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x40000000;
    raw_write16(0, 0161631);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0161631);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
