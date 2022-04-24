#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(TST, InitFix)
BOOST_AUTO_TEST_SUITE(Byte )
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x80;
    asm_m68k("tstb %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x80);
    asm_m68k("tstb (%A1)");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x8000;
    asm_m68k("tstw %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    raw_write16(0x10, 0x8000);
    asm_m68k("tstw (%A1)");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x80000000;
    asm_m68k("tstl %D1");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(mem) {
    regs.a[1] = 0x10;
    raw_write32(0x10, 0x80000000);
    asm_m68k("tstl (%A1)");
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()