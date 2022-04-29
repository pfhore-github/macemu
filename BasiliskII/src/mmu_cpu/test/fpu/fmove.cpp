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

BOOST_AUTO_TEST_CASE(Double, *boost::unit_test::tolerance(0.0001)) {
    raw_write32(0x10, 0x3ff55555);
    raw_write32(0x14, 0xfc62af22);
    regs.a[1] = 0x10;
    asm_m68k("fmove.d (%A1)+, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(regs.fp[3] == 1.3333333);
}

BOOST_AUTO_TEST_CASE(Ext, *boost::unit_test::tolerance(0.0001)) {
    raw_write32(0x10, 16383 << 16);
    raw_write32(0x14, 0x2aaaaaaa);
    raw_write32(0x18, 0xfc62af22);
    regs.a[1] = 0x10;
    asm_m68k("fmove.x (%A1)+, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(regs.fp[3] == 1.3333333);
}

BOOST_AUTO_TEST_CASE(packed, *boost::unit_test::tolerance(0.0001)) {
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
BOOST_FIXTURE_TEST_SUITE(FSMOVE, InitFix)
BOOST_AUTO_TEST_CASE(REG) {
    regs.fp[2] = 0.3f;
    asm_m68k("fsmove.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.3f);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDMOVE, InitFix)
BOOST_AUTO_TEST_CASE(REG) {
    regs.fp[2] = 0.3;
    asm_m68k("fdmove.x %FP2, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.3);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVE_CR, InitFix)
BOOST_AUTO_TEST_CASE(pi, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x00, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == M_PI);
}

BOOST_AUTO_TEST_CASE(log10_2, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x0b, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == M_LN2 / M_LN10);
}

BOOST_AUTO_TEST_CASE(e, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x0c, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == M_E);
}

BOOST_AUTO_TEST_CASE(log2_e, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x0d, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == M_LOG2E);
}

BOOST_AUTO_TEST_CASE(log10_e, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x0e, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == M_LOG10E);
}

BOOST_AUTO_TEST_CASE(zero, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x0f, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 0.0);
}

BOOST_AUTO_TEST_CASE(ln_2, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x30, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == M_LN2);
}

BOOST_AUTO_TEST_CASE(ln_10, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x31, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == M_LN10);
}

BOOST_AUTO_TEST_CASE(v10_0, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x32, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e0);
}

BOOST_AUTO_TEST_CASE(v10_1, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x33, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e1);
}

BOOST_AUTO_TEST_CASE(v10_2, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x34, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e2);
}
BOOST_AUTO_TEST_CASE(v10_4, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x35, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e4);
}
BOOST_AUTO_TEST_CASE(v10_8, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x36, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e8);
}
BOOST_AUTO_TEST_CASE(v10_16, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x37, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e16);
}
BOOST_AUTO_TEST_CASE(v10_32, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x38, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e32);
}
BOOST_AUTO_TEST_CASE(v10_64, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x39, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e64);
}
BOOST_AUTO_TEST_CASE(v10_128, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x3a, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e128);
}
BOOST_AUTO_TEST_CASE(v10_256, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x3b, %FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[3] == 1e256);
}

BOOST_AUTO_TEST_CASE(v10_512, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x3c, %FP3");
    m68k_do_execute();
    //    BOOST_TEST(regs.fp[3] == 1e512);
    BOOST_TEST(regs.fp[3] == INFINITY);
}

BOOST_AUTO_TEST_CASE(v10_1024, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x3d, %FP3");
    m68k_do_execute();
    //    BOOST_TEST(regs.fp[3] == 1e1024);
    BOOST_TEST(regs.fp[3] == INFINITY);
}

BOOST_AUTO_TEST_CASE(v10_2048, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x3e, %FP3");
    m68k_do_execute();
    //    BOOST_TEST(regs.fp[3] == 1e2048);
    BOOST_TEST(regs.fp[3] == INFINITY);
}

BOOST_AUTO_TEST_CASE(v10_4096, *boost::unit_test::tolerance(0.0001)) {
    asm_m68k("fmovecr.x #0x3f, %FP3");
    m68k_do_execute();
    //    BOOST_TEST(regs.fp[3] == 1e4096);
    BOOST_TEST(regs.fp[3] == INFINITY);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(FMOVE_TO, InitFix)

BOOST_AUTO_TEST_CASE(Byte) {
    regs.fp[3] = 3;
    regs.a[1] = 0x10;
    asm_m68k("fmove.b %FP3, (%A1)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x11);
    BOOST_TEST(raw_read8(0x10) == 3);
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.fp[3] = 103;
    regs.a[1] = 0x10;
    asm_m68k("fmove.w %FP3, (%A1)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x12);
    BOOST_TEST(raw_read16(0x10) == 103);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.fp[3] = 10300;
    regs.a[1] = 0x10;
    asm_m68k("fmove.l %FP3, (%A1)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(raw_read32(0x10) == 10300);
}

BOOST_AUTO_TEST_CASE(Single) {
    regs.fp[3] = 2.5;
    regs.a[1] = 0x10;
    asm_m68k("fmove.s %FP3, (%A1)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(raw_read32(0x10) == 0x40200000);
}

BOOST_AUTO_TEST_CASE(Double) {
    regs.fp[3] = 1.3333333333333333333333;
    regs.a[1] = 0x10;
    asm_m68k("fmove.d %FP3, (%A1)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST( raw_read32(0x10) == 0x3ff55555);
    BOOST_TEST( raw_read32(0x14) == 0x55555555);
}
BOOST_AUTO_TEST_CASE(Ext) {
    regs.fp[3] = 1.333333333333333333;
    regs.a[1] = 0x10;
    asm_m68k("fmove.x  %FP3, (%A1)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 16383 << 16);
    BOOST_TEST(raw_read32(0x14) == 0xaaaaaaaa);
    BOOST_TEST((raw_read32(0x18) &0xffff0000) == 0xaaaa0000);
}
BOOST_AUTO_TEST_SUITE(packed_fixed)
BOOST_AUTO_TEST_CASE(positive_k) {
    regs.fp[3] = 12345.678765;
    regs.a[1] = 0x10;
    asm_m68k("fmove.p %FP3, (%A1)+ {#3}");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001 );
    BOOST_TEST(raw_read32(0x14) == 0x23000000 );
    BOOST_TEST(raw_read32(0x18) == 0x00000000 );
}

BOOST_AUTO_TEST_CASE(negative_k) {
    regs.fp[3] = 12345.678765;
    regs.a[1] = 0x10;
    asm_m68k("fmove.p %FP3, (%A1)+ {#-3}");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001 );
    BOOST_TEST(raw_read32(0x14) == 0x23456790 );
    BOOST_TEST(raw_read32(0x18) == 0x00000000 );
}

BOOST_AUTO_TEST_CASE(negative_value) {
    regs.fp[3] = -12345.678765;
    regs.a[1] = 0x10;
    asm_m68k("fmove.p %FP3, (%A1)+ {#3}");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x80040001 );
    BOOST_TEST(raw_read32(0x14) == 0x23000000 );
    BOOST_TEST(raw_read32(0x18) == 0x00000000 );
}

BOOST_AUTO_TEST_CASE(negative_exp) {
    regs.fp[3] = 0.00012345678765;
    regs.a[1] = 0x10;
    asm_m68k("fmove.p %FP3, (%A1)+ {#3}");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x40040001 );
    BOOST_TEST(raw_read32(0x14) == 0x23000000 );
    BOOST_TEST(raw_read32(0x18) == 0x00000000 );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(packed_dyn)
BOOST_AUTO_TEST_CASE(positive_k) {
    regs.fp[3] = 12345.678765;
    regs.d[2] = 3;
    regs.a[1] = 0x10;
    asm_m68k("fmove.p %FP3, (%A1)+ {%D2}");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001 );
    BOOST_TEST(raw_read32(0x14) == 0x23000000 );
    BOOST_TEST(raw_read32(0x18) == 0x00000000 );
}

BOOST_AUTO_TEST_CASE(negative_k) {
    regs.fp[3] = 12345.678765;
    regs.d[2] = -3;
    regs.a[1] = 0x10;
    asm_m68k("fmove.p %FP3, (%A1)+ {%D2}");
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001 );
    BOOST_TEST(raw_read32(0x14) == 0x23456790 );
    BOOST_TEST(raw_read32(0x18) == 0x00000000 );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(FMOVE_CR_FROM, InitFix)
BOOST_AUTO_TEST_CASE(fpiar) {
    regs.a[3] = 0x100;
    asm_m68k("fmove.l %FPIAR, %A3");
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0);
}

BOOST_AUTO_TEST_CASE(fpsr) {
    memset(&regs.FPSR, 0, sizeof(regs.FPSR));
    regs.FPSR.n = true;
    asm_m68k("fmove.l %FPSR, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 1 << 27);
}

BOOST_AUTO_TEST_CASE(fpcr) {
    memset(&regs.FPCR, 0, sizeof(regs.FPCR));
    regs.FPCR.bsun = true;
    asm_m68k("fmove.l %FPCR, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x8000);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(FMOVE_CR_TO, InitFix)
BOOST_AUTO_TEST_CASE(fpiar) {
    regs.a[3] = 0x100;
    asm_m68k("fmove.l %A3, %FPIAR");
    m68k_do_execute();
    BOOST_TEST(regs.fpiar == 0x100);
}

BOOST_AUTO_TEST_CASE(fpsr) {
    memset(&regs.FPSR, 0, sizeof(regs.FPSR));
    regs.d[3] = 1 << 27;
    asm_m68k("fmove.l %D3, %FPSR");
    m68k_do_execute();
    BOOST_TEST(regs.FPSR.n);
}

BOOST_AUTO_TEST_CASE(fpcr) {
    memset(&regs.FPCR, 0, sizeof(regs.FPCR));
    regs.d[3] = 0x8000;
    asm_m68k("fmove.l %D3, %FPCR");
    m68k_do_execute();
    BOOST_TEST(regs.FPCR.bsun);
}
BOOST_AUTO_TEST_SUITE_END()