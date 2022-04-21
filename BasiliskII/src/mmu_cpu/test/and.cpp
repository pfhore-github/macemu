#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(ANDI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    regs.v = regs.c = regs.x = regs.z = true;
    regs.n = false;

    regs.d[1] = 0x123456a8;
    asm_m68k("andib #0x8a, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x123456a8 & 0xffffff8a));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.a[1] = 0x10;
    asm_m68k("andib #0x2a, (%A1)");
    raw_write8(0x10, 0x12);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == (0x12 & 0x2a));
}

BOOST_AUTO_TEST_CASE(ccr) {
    regs.x = regs.v = regs.c = regs.n = regs.z = true;
    asm_m68k("andib #0, %CCR");
    m68k_do_execute();
    BOOST_TEST(regs.x == false);
    BOOST_TEST(regs.v == false);
    BOOST_TEST(regs.c == false);
    BOOST_TEST(regs.n == false);
    BOOST_TEST(regs.z == false);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    regs.v = regs.c = regs.x = regs.z = true;
    regs.n = false;
    regs.d[1] = 0x1234a678;

    asm_m68k("andiw #0xdead, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x1234a678 & 0xffffdead));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;

    asm_m68k("andiw #0x2abc, (%A1)");
    raw_write16(0x10, 0x1234);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == (0x1234 & 0x2abc));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    regs.v = regs.c = regs.x = regs.z = true;
    regs.n = false;
    regs.d[1] = 0x82345678;

    asm_m68k("andil #0xdeadbeaf, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x82345678 & 0xdeadbeaf));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;

    asm_m68k("andil #0x2abc5412, (%A1)");
    raw_write32(0x10, 0x1234dead);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == (0x2abc5412 & 0x1234dead));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(AND, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.v = regs.c = regs.x = regs.z = true;
    regs.n = false;

    regs.d[1] = 0x123456a8;
    regs.d[2] = 0x8a;
    asm_m68k("andb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x123456a8 & 0xffffff8a));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.a[1] = 0x10;
    regs.d[2] = 0x2a;
    asm_m68k("andb (%A1), %D2");
    raw_write8(0x10, 0x12);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == (0x12 & 0x2a));
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.a[1] = 0x10;
    regs.d[2] = 0x2a;
    asm_m68k("andb %D2, (%A1)");
    raw_write8(0x10, 0x12);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == (0x12 & 0x2a));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.v = regs.c = regs.x = regs.z = true;
    regs.n = false;
    regs.d[1] = 0x1234a678;
    regs.d[2] = 0xdead;
    asm_m68k("andw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x1234a678 & 0xffffdead));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.a[1] = 0x10;
    regs.d[2] = 0x2abc;
    asm_m68k("andw (%A1), %D2");
    raw_write16(0x10, 0x1234);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == (0x1234 & 0x2abc));
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.a[1] = 0x10;
    regs.d[2] = 0x2abc;
    asm_m68k("andw %D2, (%A1)");
    raw_write16(0x10, 0x1234);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == (0x1234 & 0x2abc));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.v = regs.c = regs.x = regs.z = true;
    regs.n = false;
    regs.d[1] = 0x82345678;
    regs.d[2] = 0xdeadbeaf;
    asm_m68k("andl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x82345678 & 0xdeadbeaf));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.a[1] = 0x10;
    regs.d[2] = 0x2abc5412;
    asm_m68k("andl (%A1), %D2");
    raw_write32(0x10, 0x1234dead);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == (0x2abc5412 & 0x1234dead));
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.a[1] = 0x10;
    regs.d[2] = 0x2abc5412;
    asm_m68k("andl %D2, (%A1)");
    raw_write32(0x10, 0x1234dead);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == (0x2abc5412 & 0x1234dead));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
