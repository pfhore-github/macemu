#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(CMPA, InitFix)
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(Dreg) {
    regs.a[1] = 998;
    regs.d[2] = 1000;
    asm_m68k("cmpaw %D2, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(Areg) {
    regs.a[1] = 998;
    regs.a[2] = 1000;
    asm_m68k("cmpaw %A2, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.a[2] = 998;
    regs.a[1] = 0x10;
    asm_m68k("cmpaw (%A1), %A2");
    raw_write16(0x10, 1000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(imm) {
    regs.a[2] = 998;
    asm_m68k("cmpaw #1000, %A2");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(Dreg) {
    regs.a[1] = 0x10000;
    regs.d[2] = 0x20000;
    asm_m68k("cmpal %D2, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(Areg) {
    regs.a[1] = 0x10000;
    regs.a[2] = 0x20000;
    asm_m68k("cmpal %A2, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_CASE(memory) {
    regs.a[2] = 0x10000;
    regs.a[1] = 0x10;
    asm_m68k("cmpal (%A1), %A2");
    raw_write32(0x10, 0x20000);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(imm) {
    regs.a[2] = 998;
    asm_m68k("cmpal #0x20000, %A2");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()