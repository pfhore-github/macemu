#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(FMOVE_FROM, InitFix)
BOOST_AUTO_TEST_CASE(REG) {
    regs.fp[2] = 0.3;
    asm_m68k("fmove.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.3);
}

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(2), value) {
    regs.fp[2] = value ? -2 : 2;
    asm_m68k("fmove.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(2), value) {
    regs.fp[2] = value ? 0 : 2;
    asm_m68k("fmove.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.z == value);
}

BOOST_DATA_TEST_CASE(inf, boost::unit_test::data::xrange(2), value) {
    regs.fp[2] = value ? INFINITY : 2;
    asm_m68k("fmove.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.i == value);
}

BOOST_DATA_TEST_CASE(nan_, boost::unit_test::data::xrange(2), value) {
    regs.fp[2] = value ? NAN : 2;
    asm_m68k("fmove.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.nan == value);
}

BOOST_AUTO_TEST_CASE(Byte) {
    raw_write8(0x10, 3);
    regs.a[1] = 0x10;
    asm_m68k("fmove.b (%A1)+, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x11);
    BOOST_TEST(regs.fp[3] == 3);
}

BOOST_AUTO_TEST_CASE(Word) {
    raw_write16(0x10, 103);
    regs.a[1] = 0x10;
    asm_m68k("fmove.w (%A1)+, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x12);
    BOOST_TEST(regs.fp[3] == 103);
}

BOOST_AUTO_TEST_CASE(Long) {
    raw_write32(0x10, 10300);
    regs.a[1] = 0x10;
    asm_m68k("fmove.l (%A1)+, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(regs.fp[3] == 10300);
}

BOOST_AUTO_TEST_CASE(Single) {
    raw_write32(0x10, 0x40200000);
    regs.a[1] = 0x10;
    asm_m68k("fmove.s (%A1)+, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(regs.fp[3] == 2.5);
}

BOOST_AUTO_TEST_CASE(Double, * boost::unit_test::tolerance(0.0001)) {
    raw_write32(0x10, 0x3ff55555);
    raw_write32(0x14, 0xfc62af22);
    regs.a[1] = 0x10;
    asm_m68k("fmove.d (%A1)+, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(regs.fp[3] == 1.3333333);
}

BOOST_AUTO_TEST_CASE(Ext, * boost::unit_test::tolerance(0.0001)) {
    raw_write32(0x10, 16383 << 16);
    raw_write32(0x14, 0x2aaaaaaa);
    raw_write32(0x18, 0xfc62af22);
    regs.a[1] = 0x10;
    asm_m68k("fmove.x (%A1)+, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(regs.fp[3] == 1.3333333);
}

BOOST_AUTO_TEST_CASE(packed, * boost::unit_test::tolerance(0.0001)) {
    raw_write32(0x10, 0x00020001);
    raw_write32(0x14, 0x33333333);
    raw_write32(0x18, 0x44444444);
    regs.a[1] = 0x10;
    asm_m68k("fmove.p (%A1)+, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(regs.fp[3] == 1.333333334444e2);
}
BOOST_AUTO_TEST_SUITE_END()
