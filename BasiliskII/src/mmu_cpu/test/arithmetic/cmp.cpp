#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(CMP, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 24;
    regs.d[2] = 25;
    asm_m68k("cmpb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.d[1] = 24;
    regs.a[1] = 0x10;
    raw_write8(0x10, 25);
    asm_m68k("cmpb (%A1), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(imm) {
    regs.d[1] = 24;
    asm_m68k("cmpb #25, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Dreg) {
    regs.d[1] = 999;
    regs.d[2] = 1000;
    asm_m68k("cmpw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(Areg) {
    regs.d[1] = 999;
    regs.a[2] = 1000;
    asm_m68k("cmpw %A2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.d[1] = 999;
    regs.a[1] = 0x10;
    asm_m68k("cmpw (%A1), %D1");
    raw_write16(0x10, 1000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(imm) {
    regs.d[1] = 999;
    asm_m68k("cmpw #1000, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Dreg) {
    regs.d[2] = 100000001;
    regs.d[1] = 100000000;
    asm_m68k("cmpl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(Areg) {
    regs.a[2] = 100000001;
    regs.d[1] = 100000000;
    asm_m68k("cmpl %A2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.a[1] = 0x10;
    regs.d[1] = 99999999;
    asm_m68k("cmpl (%A1), %D1");
    raw_write32(0x10, 100000000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(imm) {
    regs.d[1] = 100000000;
    asm_m68k("cmpl #100000001, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
