#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(ADDI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 0x12345600 | 33;
    asm_m68k("addib #25, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x12345600 | (33 + 25));
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.a[1] = 0x10;
    asm_m68k("addib #25, (%A1)");
    raw_write8(0x10, 66);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 66 + 25);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 0x12340000 | 3456;
    asm_m68k("addiw #1000, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12340000 | 4456));
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.a[1] = 0x10;
    asm_m68k("addiw #1000, (%A1)");
    raw_write16(0x10, 3456);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 4456);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 123456789;
    asm_m68k("addil #100000000, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 223456789);
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.a[1] = 0x10;
    asm_m68k("addil #100000000, (%A1)");
    raw_write32(0x10, 123456789);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 223456789);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(ADDQ, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 0x12345600 | 33;
    asm_m68k("addqb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x12345600 | (33 + 2));
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 0x12345600 | 33;
    asm_m68k("addqb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x12345600 | (33 + 8));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 0x12340000 | 3456;
    asm_m68k("addqw #1, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12340000 | 3457));
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 0x12340000 | 3456;
    asm_m68k("addqw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12340000 | 3464));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 123456789;
    asm_m68k("addql #1, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 123456790);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 123456789;
    asm_m68k("addql #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 123456797);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(WordA)

BOOST_AUTO_TEST_CASE(value) {
    regs.a[1] = 0x1234;
    asm_m68k("addqw #1, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1235);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.a[1] = 0x1234;
    asm_m68k("addqw #8, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == (0x1234 + 8));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LongA)

BOOST_AUTO_TEST_CASE(value) {
    regs.a[1] = 0x20000000;
    asm_m68k("addql #1, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x20000001);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.a[1] = 0x20000000;
    asm_m68k("addql #8, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x20000008);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ADD, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.d[1] = 0x12345600 | 33;
    regs.d[2] = 25;
    asm_m68k("addb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x12345600 | (33 + 25));
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 0x12345600 | 33;
    raw_write8(0x10, 25);    
    regs.a[2] = 25;
    asm_m68k("addb (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x12345600 | (33 + 25));
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.a[1] = 0x10;
    regs.d[2] = 25;
    asm_m68k("addb %D2, (%A1)");
    raw_write8(0x10, 66);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 66 + 25);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.d[1] = 0x12340000 | 3456;
    regs.d[2] = 1000;
    asm_m68k("addw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12340000 | 4456));
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 0x12340000 | 3456;    
    raw_write16( 0x10, 1000);
    regs.a[2] = 0x10;
    asm_m68k("addw (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12340000 | 4456));
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.d[2] = 1000;
    regs.a[1] = 0x10;
    asm_m68k("addw %D2, (%A1)");
    raw_write16(0x10, 3456);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 4456);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(RegToReg) {
    regs.d[1] = 123456789;
    regs.d[2] = 100000000;
    asm_m68k("addl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 223456789);
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 123456789;
    regs.a[2] = 0x10;
    raw_write32(0x10, 100000000);
    asm_m68k("addl (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 223456789);
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.a[1] = 0x10;
    regs.d[2] = 100000000;
    asm_m68k("addl %D2, (%A1)");
    raw_write32(0x10, 123456789);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 223456789);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()