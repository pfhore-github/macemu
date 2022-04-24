#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(OR, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_AUTO_TEST_CASE(RegtoReg) {
    regs.d[1] = 0x78;
    regs.d[2] = 0x2a;
    asm_m68k("orb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x78 | 0x2a));
}

BOOST_AUTO_TEST_CASE(ImmtoReg) {
    regs.d[1] = 0x78;
    asm_m68k("orb #0x2a, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x78 | 0x2a));
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 0x78;
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x2a);
    asm_m68k("orb (%A1), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x78 | 0x2a));
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.d[1] = 0x78;
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x2a);
    asm_m68k("orb %D1, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == (0x78 | 0x2a));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(RegtoReg) {
    regs.d[1] = 0x5678;
    regs.d[2] = 0xdead;
    asm_m68k("orw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x5678 | 0xdead));
}

BOOST_AUTO_TEST_CASE(ImmtoReg) {
    regs.d[1] = 0x5678;
    asm_m68k("orw #0xdead, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x5678 | 0xdead));
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 0x5678;
    regs.a[1] = 0x10;
    raw_write16(0x10, 0xdead);
    asm_m68k("orw (%A1), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x5678 | 0xdead));
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.d[1] = 0x5678;
    regs.a[1] = 0x10;
    raw_write16(0x10, 0x2a2b);
    asm_m68k("orw %D1, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == (0x5678 | 0x2a2b));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(RegtoReg) {
    regs.d[1] = 0x12345678;
    regs.d[2] = 0xdeadbeaf;
    asm_m68k("orl %D2, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345678 | 0xdeadbeaf));
}

BOOST_AUTO_TEST_CASE(ImmtoReg) {
    regs.d[1] = 0x12345678;
    asm_m68k("orl #0xdeadbeaf, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345678 | 0xdeadbeaf));
}

BOOST_AUTO_TEST_CASE(MemToReg) {
    regs.d[1] = 0x12345678;
    regs.a[1] = 0x10;
    raw_write32(0x10, 0xdeadbeaf);
    asm_m68k("orl (%A1), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345678 | 0xdeadbeaf));
}

BOOST_AUTO_TEST_CASE(RegToMem) {
    regs.d[1] = 0x12345678;
    regs.a[1] = 0x10;
    raw_write32(0x10, 0x2a2b2c2d);
    asm_m68k("orl %D1, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == (0x12345678 | 0x2a2b2c2d));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()