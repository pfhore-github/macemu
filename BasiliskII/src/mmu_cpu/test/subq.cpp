#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>

BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 0x12345600 | 33;
    asm_m68k("subqb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x12345600 | (33 - 2));
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 0x12345600 | 33;
    asm_m68k("subqb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x12345600 | (33 - 8));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 0x12340000 | 3456;
    asm_m68k("subqw #1, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12340000 | 3455));
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 0x12340000 | 3456;
    asm_m68k("subqw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12340000 | 3448));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 123456789;
    asm_m68k("subql #1, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 123456788);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 123456789;
    asm_m68k("subql #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 123456781);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(WordA)

BOOST_AUTO_TEST_CASE(value) {
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x1234;

    asm_m68k("subqw #1, %A1");

    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1233);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.a[1] = 0x1234;
    asm_m68k("subqw #8, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == (0x1234 - 8));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LongA)

BOOST_AUTO_TEST_CASE(value) {
    regs.a[1] = 0x20000002;

    asm_m68k("subql #1, %A1");

    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x20000001);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x20000008;

    asm_m68k("subql #8, %A1");

    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x20000000);
}

BOOST_AUTO_TEST_SUITE_END()
