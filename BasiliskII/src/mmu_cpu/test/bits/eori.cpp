#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>



BOOST_FIXTURE_TEST_SUITE(EORI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x78;
    asm_m68k("eorib #0x8a, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x78 ^ 0x8a));
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    asm_m68k("eorib #0x2a, (%A1)");
    raw_write8(0x10, 0x12);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == (0x12 ^ 0x2a));
}

BOOST_AUTO_TEST_CASE(ccr) {
    regs.x = regs.v = regs.c = regs.n = regs.z = true;
    asm_m68k("eorib #ff, %CCR");
    m68k_do_execute();
    BOOST_TEST(regs.x == false);
    BOOST_TEST(regs.v == false);
    BOOST_TEST(regs.c == false);
    BOOST_TEST(regs.n == false);
    BOOST_TEST(regs.z == false);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x5678;
    asm_m68k("eoriw #0xdead, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x5678 ^ 0xdead));
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    asm_m68k("eoriw #0x2abc, (%A1)");
    raw_write16(0x10, 0x1234);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == (0x1234 ^ 0x2abc));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x12345678;

    asm_m68k("eoril #0xdeadbeaf, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345678 ^ 0xdeadbeaf));
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    asm_m68k("eoril #0x2abc5412, (%A1)");
    raw_write32(0x10, 0x1234dead);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == (0x2abc5412 ^ 0x1234dead));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()