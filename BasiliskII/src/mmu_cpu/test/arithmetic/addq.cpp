#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(ADDQ, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_SUITE(reg)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[1] = 33;
    asm_m68k("addqb #2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 35);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.d[1] = 33;
    asm_m68k("addqb #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 41);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(mem)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 33);
    asm_m68k("addqb #2, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 35);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 33);
    asm_m68k("addqb #8, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 41);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_SUITE(Dreg)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[1] = 3456;
    asm_m68k("addqw #1, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 3457);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.d[1] = 3456;
    asm_m68k("addqw #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 3464);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Areg)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 0x1234;
    asm_m68k("addqw #1, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1235);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.a[1] = 0x1234;
    asm_m68k("addqw #8, %A1");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == (0x1234 + 8));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(mem)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 0x10;
    raw_write16(0x10, 3300);
    asm_m68k("addqw #2, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 3302);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.a[1] = 0x10;
    raw_write16(0x10, 3300);
    asm_m68k("addqw #8, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 3308);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_SUITE(Dreg)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[1] = 123456789;
    asm_m68k("addql #1, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 123456790);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.d[1] = 123456789;
    asm_m68k("addql #8, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 123456797);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Areg)
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
BOOST_AUTO_TEST_SUITE(mem)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 0x10;
    raw_write32(0x10, 330000);
    asm_m68k("addql #2, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 330002);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.a[1] = 0x10;
    raw_write32(0x10, 330000);
    asm_m68k("addql #8, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 330008);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE_END()