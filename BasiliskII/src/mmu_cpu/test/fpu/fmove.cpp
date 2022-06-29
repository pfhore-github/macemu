#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>

BOOST_FIXTURE_TEST_SUITE(FMOVE_FROM, InitFix)
BOOST_AUTO_TEST_CASE(REG) { fpu_test2(0, 1.0, 1.0); }
BOOST_AUTO_TEST_CASE(Byte) {
    raw_write8(0x10, -4);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 6 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x11);
    BOOST_TEST(mpfr_get_si(fpu.fp[2].ptr(), MPFR_RNDN) == -4.0);
}

BOOST_AUTO_TEST_CASE(Word) {
    raw_write16(0x10, -100);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 4 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x12);
    BOOST_TEST(mpfr_get_si(fpu.fp[2].ptr(), MPFR_RNDN) == -100.0);
}

BOOST_AUTO_TEST_CASE(Long) {
    raw_write32(0x10, -10000);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 0 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(mpfr_get_si(fpu.fp[2].ptr(), MPFR_RNDN) == -10000);
}
BOOST_AUTO_TEST_SUITE(Single)
BOOST_DATA_TEST_CASE(Normal, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31 | ((3 + 127) << 23 | 0x555555));
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 1 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(mpfr_get_flt(fpu.fp[2].ptr(), MPFR_RNDN) ==
               copysignf(0x1.AAAAAA8p3f, sg));
}

BOOST_DATA_TEST_CASE(DeNormal, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31 | 0x200000);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 1 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(mpfr_get_flt(fpu.fp[2].ptr(), MPFR_RNDN) ==
               copysignf(0x0.4p-126f, sg));
}

BOOST_DATA_TEST_CASE(Zero, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31 | 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 1 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == signbit(sg));
}

BOOST_DATA_TEST_CASE(Inf, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31 | 255 << 23);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 1 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(fpu.fp[2].is_inf());
    BOOST_TEST(fpu.fp[2].signbit() == signbit(sg));
}

BOOST_AUTO_TEST_CASE(qNAN) {
    raw_write32(0x10, 255 << 23 | 0x700000);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 1 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[2].get_payload() == 0x7000000000000000LLU);
}

BOOST_AUTO_TEST_CASE(sNAN) {
    raw_write32(0x10, 255 << 23 | 0x300000);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 1 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[2].get_payload() == 0x7000000000000000LLU);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Double)
BOOST_DATA_TEST_CASE(Normal, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31 | ((3 + 1023) << 20 | 0x55555));
    raw_write32(0x14, 0x55555555);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 5 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_CHECK_CLOSE(mpfr_get_d(fpu.fp[2].ptr(), MPFR_RNDN),
                      copysign(0x1.5555555555555p3, sg), 1e-08);
}

BOOST_DATA_TEST_CASE(DeNormal, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31 | 0x20000);
    raw_write32(0x14, 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 5 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_CHECK_CLOSE(mpfr_get_d(fpu.fp[2].ptr(), MPFR_RNDN),
                      copysign(0x0.2p-1022, sg), 1e-08);
}

BOOST_DATA_TEST_CASE(Zero, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31);
    raw_write32(0x14, 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 5 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == signbit(sg));
}

BOOST_DATA_TEST_CASE(Infinity, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31 | 2047 << 20);
    raw_write32(0x14, 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 5 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(fpu.fp[2].is_inf());
    BOOST_TEST(fpu.fp[2].signbit() == signbit(sg));
}

BOOST_AUTO_TEST_CASE(qNAN) {
    raw_write32(0x10, 2047 << 20 | 0xF0000);
    raw_write32(0x14, 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 5 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[2].get_payload() == 0x7800000000000000LLU);
}

BOOST_AUTO_TEST_CASE(sNAN) {
    raw_write32(0x10, 2047 << 20 | 0x70000);
    raw_write32(0x14, 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 5 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[2].get_payload() == 0x7800000000000000LLU);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_DATA_TEST_CASE(Normal, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31 | (16383 + 3) << 16);
    raw_write32(0x14, 0xD5555555);
    raw_write32(0x18, 0x55555555);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 2 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_CHECK_CLOSE(mpfr_get_d(fpu.fp[2].ptr(), MPFR_RNDN),
                      copysign(0x1.AAAAAAAAAAAAp3, sg), 1e-08);
}

BOOST_DATA_TEST_CASE(DeNormal, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31);
    raw_write32(0x14, 0x10000000);
    raw_write32(0x18, 0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 2 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    auto [sgv, ue, exp] = fpu.fp[2].get_zexp();
    BOOST_TEST(signbit(sg) == sgv);
    BOOST_TEST(ue == 0x1000000000000000LLU);
    BOOST_TEST(exp == 0);
}

BOOST_DATA_TEST_CASE(Zero, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31);
    raw_write32(0x14, 0);
    raw_write32(0x18, 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 2 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == signbit(sg));
}

BOOST_DATA_TEST_CASE(Infinity, SIGN, sg) {
    raw_write32(0x10, (sg == -1) << 31 | 32767 << 16);
    raw_write32(0x14, 0);
    raw_write32(0x18, 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 2 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1C);
    BOOST_TEST(fpu.fp[2].is_inf());
    BOOST_TEST(fpu.fp[2].signbit() == signbit(sg));
}

BOOST_AUTO_TEST_CASE(qNAN) {
    raw_write32(0x10, 2047 << 20 | 32767 << 16);
    raw_write32(0x14, 0x70000000);
    raw_write32(0x18, 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 2 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1C);
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[2].get_payload() == 0x7000000000000000LLU);
}

BOOST_AUTO_TEST_CASE(sNAN) {
    raw_write32(0x10, 2047 << 20 | 32767 << 16);
    raw_write32(0x14, 0x30000000);
    raw_write32(0x18, 0);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 2 << 10 | 2 << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1C);
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[2].get_payload() == 0x7000000000000000LLU);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(PACKED)
BOOST_DATA_TEST_CASE(normal, SIGN, sg) {
    raw_write32(0x10, 0x00020001 | (sg == -1.0) << 31);
    raw_write32(0x14, 0x33333333);
    raw_write32(0x18, 0x44444444);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 3 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_CHECK_CLOSE(mpfr_get_d(fpu.fp[2].ptr(), MPFR_RNDN),
                      copysign(1.3333333344444445e+2, sg), 1e-08);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    raw_write32(0x10, 0x40000000 | (sg == -1.0) << 31);
    raw_write32(0x14, 0);
    raw_write32(0x18, 0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 3 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == signbit(sg));
}

BOOST_DATA_TEST_CASE(Infinity, SIGN, sg) {
    raw_write32(0x10, 0x7FFF0000 | (sg == -1.0) << 31);
    raw_write32(0x14, 0);
    raw_write32(0x18, 0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 3 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(fpu.fp[2].is_inf());
    BOOST_TEST(fpu.fp[2].signbit() == signbit(sg));
}

BOOST_AUTO_TEST_CASE(qNAN) {
    raw_write32(0x10, 0x7FFF0000);
    raw_write32(0x14, 0x70000000);
    raw_write32(0x18, 0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 3 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[2].get_payload() == 0x7000000000000000LLU);
}

BOOST_AUTO_TEST_CASE(sNAN) {
    raw_write32(0x10, 0x7FFF0000);
    raw_write32(0x14, 0x30000000);
    raw_write32(0x18, 0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 3 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[2].get_payload() == 0x7000000000000000LLU);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(FSMOVE, InitFix)
BOOST_AUTO_TEST_CASE(operand) { fpu_test2(0x40, 0.1f, 0.1f); }
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDMOVE, InitFix)
BOOST_AUTO_TEST_CASE(operand) { fpu_test2(0x44, 0.5, 0.5); }
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVE_TO, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(in_range) {
    set_fpu_reg(1, -3.0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 6 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x11);
    BOOST_TEST(static_cast<int8_t>(raw_read8(0x10)) == -3);
    BOOST_TEST(!fpu.FPSR.inex2);
}

BOOST_AUTO_TEST_CASE(inex) {
    set_fpu_reg(1, 1.3);
    regs.a[1] = 0x10;
    raw_write16(0, 0171021);
    raw_write16(2, 3 << 13 | 6 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 1);
    BOOST_TEST(fpu.FPSR.inex2);
}

BOOST_AUTO_TEST_CASE(out_of_range) {
    set_fpu_reg(1, 300.0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171021);
    raw_write16(2, 3 << 13 | 6 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(in_range) {
    set_fpu_reg(1, -300.0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 4 << 10 | 1 << 7);
    fpu.FPSR.inex2 = false;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x12);
    BOOST_TEST(static_cast<int16_t>(raw_read16(0x10)) == -300);
    BOOST_TEST(!fpu.FPSR.inex2);
}

BOOST_AUTO_TEST_CASE(inex) {
    set_fpu_reg(1, 1.3);
    regs.a[1] = 0x10;
    raw_write16(0, 0171021);
    raw_write16(2, 3 << 13 | 4 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 1);
    BOOST_TEST(fpu.FPSR.inex2);
}

BOOST_AUTO_TEST_CASE(out_of_range) {
    set_fpu_reg(1, 300000.0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171021);
    raw_write16(2, 3 << 13 | 4 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST(fpu.FPSR.operr);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(in_range) {
    set_fpu_reg(1, 12345678.0);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 0 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(static_cast<int32_t>(raw_read32(0x10)) == 12345678.0);
}

BOOST_AUTO_TEST_CASE(inex) {
    set_fpu_reg(1, 1.3);
    regs.a[1] = 0x10;
    raw_write16(0, 0171021);
    raw_write16(2, 3 << 13 | 0 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 1);
    BOOST_TEST(fpu.FPSR.inex2);
}

BOOST_AUTO_TEST_CASE(out_of_range) {
    set_fpu_reg(1, 1e+222);
    regs.a[1] = 0x10;
    raw_write16(0, 0171021);
    raw_write16(2, 3 << 13 | 0 << 10 | 1 << 7);
    m68k_do_execute();
    BOOST_TEST(fpu.FPSR.operr);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Single)
BOOST_DATA_TEST_CASE(Normal, SIGN, sg) {
    set_fpu_reg(2, copysignf(0x1.AAAAAA8p3f, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 1 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(raw_read32(0x10) ==
               static_cast<uint32_t>(
                   ((sg == -1) << 31 | ((3 + 127) << 23 | 0x555555))));
}

BOOST_DATA_TEST_CASE(DeNormal, SIGN, sg) {
    set_fpu_reg(2, copysignf(0x0.4p-126f, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 1 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(raw_read32(0x10) ==
               static_cast<uint32_t>(((sg == -1) << 31 | 0x200000)));
}

BOOST_DATA_TEST_CASE(Zero, SIGN, sg) {
    set_fpu_reg(2, copysignf(0.0, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 1 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(raw_read32(0x10) ==
               static_cast<uint32_t>(((sg == -1) << 31 | 0)));
}

BOOST_DATA_TEST_CASE(Inf, SIGN, sg) {
    set_fpu_reg(2, copysignf(INFINITY, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 1 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(raw_read32(0x10) ==
               static_cast<uint32_t>((sg == -1) << 31 | 255 << 23));
}

BOOST_AUTO_TEST_CASE(qNAN) {
    fpu.fp[2].set_nan(0x7000000000000000LLU);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 1 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(raw_read32(0x10) == static_cast<uint32_t>(255 << 23 | 0x700000));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Double)
BOOST_DATA_TEST_CASE(Normal, SIGN, sg) {
    set_fpu_reg(2, copysign(0x1.5555555555555p3, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 5 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(
        raw_read32(0x10) ==
        static_cast<uint32_t>((sg == -1) << 31 | ((3 + 1023) << 20 | 0x55555)));
    BOOST_TEST(raw_read32(0x14) == 0x55555555);
}

BOOST_DATA_TEST_CASE(DeNormal, SIGN, sg) {
    set_fpu_reg(2, copysign(0x0.2p-1022, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 5 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(raw_read32(0x10) ==
               static_cast<uint32_t>((sg == -1) << 31 | 0x20000));
    BOOST_TEST(raw_read32(0x14) == 0);
}

BOOST_DATA_TEST_CASE(Zero, SIGN, sg) {
    set_fpu_reg(2, copysign(0.0, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 5 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(raw_read32(0x10) == static_cast<uint32_t>((sg == -1) << 31 | 0));
    BOOST_TEST(raw_read32(0x14) == 0);
}

BOOST_DATA_TEST_CASE(Infinity, SIGN, sg) {
    set_fpu_reg(2, copysign(INFINITY, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 5 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(raw_read32(0x10) ==
               static_cast<uint32_t>((sg == -1) << 31 | 2047 << 20));
    BOOST_TEST(raw_read32(0x14) == 0);
}

BOOST_AUTO_TEST_CASE(qNAN) {
    fpu.fp[2].set_nan(0x7800000000000000LLU);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 5 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_TEST(raw_read32(0x10) == static_cast<uint32_t>(2047 << 20 | 0xF0000));
    BOOST_TEST(raw_read32(0x14) == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Ext)
BOOST_DATA_TEST_CASE(Normal, SIGN, sg) {
    if(sg == -1) {
        mpfr_strtofr(fpu.fp[2].ptr(), "-1.AAAAAAAAAAAAAAAAAAAAAp3", nullptr, 16,
                     MPFR_RNDN);
    } else {
        mpfr_strtofr(fpu.fp[2].ptr(), "1.AAAAAAAAAAAAAAAAAAAAAp3", nullptr, 16,
                     MPFR_RNDN);
    }
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 2 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) ==
               static_cast<uint32_t>((sg == -1) << 31 | (16383 + 3) << 16));
    BOOST_TEST(raw_read32(0x14) == 0xD5555555);
    BOOST_TEST(raw_read32(0x18) == 0x55555555);
}

BOOST_DATA_TEST_CASE(DeNormal, SIGN, sg) {
    if(sg == -1) {
        mpfr_strtofr(fpu.fp[2].ptr(), "-0.2p-16383", nullptr, 16, MPFR_RNDN);
    } else {
        mpfr_strtofr(fpu.fp[2].ptr(), "0.2p-16383", nullptr, 16, MPFR_RNDN);
    }
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 2 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == (sg == -1) << 31);
    BOOST_TEST(raw_read32(0x14) == 0x10000000);
    BOOST_TEST(raw_read32(0x18) == 0);
}

BOOST_DATA_TEST_CASE(Zero, SIGN, sg) {
    set_fpu_reg(2, copysign(0.0, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 2 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == static_cast<uint32_t>((sg == -1) << 31));
    BOOST_TEST(raw_read32(0x14) == 0);
    BOOST_TEST(raw_read32(0x18) == 0);
}

BOOST_DATA_TEST_CASE(Infinity, SIGN, sg) {
    set_fpu_reg(2, copysign(INFINITY, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 2 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == static_cast<uint32_t>((sg == -1) << 31 | 32767 << 16));
    BOOST_TEST(raw_read32(0x14) == 0);
    BOOST_TEST(raw_read32(0x18) == 0);
}

BOOST_AUTO_TEST_CASE(qNAN) {
    fpu.fp[2].set_nan(0x7000000000000000LLU);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 2 << 10 | 2 << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == static_cast<uint32_t>(32767 << 16));
    BOOST_TEST(raw_read32(0x14) == 0x70000000);
    BOOST_TEST(raw_read32(0x18) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(packed_fixed)
BOOST_AUTO_TEST_CASE(positive_k) {
    set_fpu_reg(2, 12345.678765);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | 2 << 7 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001);
    BOOST_TEST(raw_read32(0x14) == 0x23000000);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_AUTO_TEST_CASE(negative_k) {
    set_fpu_reg(2, 12345.678765);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | 2 << 7 | 0x7D);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001);
    BOOST_TEST(raw_read32(0x14) == 0x23456790);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_AUTO_TEST_CASE(negative_value) {
    set_fpu_reg(2, -12345.678765);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | 2 << 7 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x80040001);
    BOOST_TEST(raw_read32(0x14) == 0x23000000);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_AUTO_TEST_CASE(negative_exp) {
    set_fpu_reg(2, 0.00012345678765);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | 2 << 7 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x40040001);
    BOOST_TEST(raw_read32(0x14) == 0x23000000);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    set_fpu_reg(2, copysign(0.0, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | 2 << 7 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == static_cast<uint32_t>((sg==-1)<<31));
    BOOST_TEST(raw_read32(0x14) == 0);
    BOOST_TEST(raw_read32(0x18) == 0);
}

BOOST_DATA_TEST_CASE(Infinity, SIGN, sg) {
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    set_fpu_reg(2, copysign(INFINITY, sg));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | 2 << 7 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x7FFF0000 | static_cast<uint32_t>((sg==-1)<<31) );
    BOOST_TEST(raw_read32(0x14) == 0);
    BOOST_TEST(raw_read32(0x18) == 0);
}


BOOST_AUTO_TEST_CASE(qNAN) {
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    fpu.fp[2].set_nan(0x7000000000000000LLU);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | 2 << 7 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x7FFF0000 );
    BOOST_TEST(raw_read32(0x14) == 0x70000000);
    BOOST_TEST(raw_read32(0x18) == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(packed_dyn)
BOOST_AUTO_TEST_CASE(positive_k) {
    set_fpu_reg(2, 12345.678765);
    regs.a[1] = 0x10;
    regs.d[3] = 3;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 7 << 10 | 2 << 7 | 3 << 4);
    m68k_do_execute();

    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001);
    BOOST_TEST(raw_read32(0x14) == 0x23000000);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_AUTO_TEST_CASE(negative_k) {
    set_fpu_reg(2, 12345.678765);
    regs.a[1] = 0x10;
    regs.d[3] = -3;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 7 << 10 | 2 << 7 | 3 << 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001);
    BOOST_TEST(raw_read32(0x14) == 0x23456790);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}


BOOST_DATA_TEST_CASE(zero, SIGN, sg) {
    set_fpu_reg(2, copysign(0.0, sg));
    regs.a[1] = 0x10;
    regs.d[3] = 3;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 7 << 10 | 2 << 7 | 3 << 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == static_cast<uint32_t>((sg==-1)<<31));
    BOOST_TEST(raw_read32(0x14) == 0);
    BOOST_TEST(raw_read32(0x18) == 0);
}

BOOST_DATA_TEST_CASE(Infinity, SIGN, sg) {
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    set_fpu_reg(2, copysign(INFINITY, sg));
    regs.a[1] = 0x10;
    regs.d[3] = 3;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 7 << 10 | 2 << 7 | 3 << 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x7FFF0000 | static_cast<uint32_t>((sg==-1)<<31) );
    BOOST_TEST(raw_read32(0x14) == 0);
    BOOST_TEST(raw_read32(0x18) == 0);
}


BOOST_AUTO_TEST_CASE(qNAN) {
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    fpu.fp[2].set_nan(0x7000000000000000LLU);
    regs.a[1] = 0x10;
    regs.d[3] = 3;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 7 << 10 | 2 << 7 | 3 << 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x7FFF0000 );
    BOOST_TEST(raw_read32(0x14) == 0x70000000);
    BOOST_TEST(raw_read32(0x18) == 0);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVE_CR_FROM, InitFix)
BOOST_AUTO_TEST_CASE(fpiar) {
    regs.a[2] = 0x100;
    raw_write16(0, 0171012);
    raw_write16(2, 0xA000 | 1 << 10);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0);
}

BOOST_AUTO_TEST_CASE(fpsr) {
    memset(&fpu.FPSR, 0, sizeof(fpu.FPSR));
    fpu.FPSR.n = true;
    raw_write16(0, 0171003);
    raw_write16(2, 0xA000 | 1 << 11);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 1 << 27);
}

BOOST_AUTO_TEST_CASE(fpcr) {
    memset(&fpu.FPCR, 0, sizeof(fpu.FPCR));
    fpu.FPCR.bsun = true;
    raw_write16(0, 0171003);
    raw_write16(2, 0xA000 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x8000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVE_CR_TO, InitFix)
BOOST_AUTO_TEST_CASE(fpiar) {
    regs.a[3] = 0x100;
    raw_write16(0, 0171013);
    raw_write16(2, 0x8000 | 1 << 10);
    m68k_do_execute();
    BOOST_TEST(fpu.fpiar == 0x100);
}

BOOST_AUTO_TEST_CASE(fpsr) {
    memset(&fpu.FPSR, 0, sizeof(fpu.FPSR));
    regs.d[3] = 1 << 27;
    raw_write16(0, 0171003);
    raw_write16(2, 0x8000 | 1 << 11);
    m68k_do_execute();
    BOOST_TEST(fpu.FPSR.n);
}

BOOST_AUTO_TEST_CASE(fpcr) {
    memset(&fpu.FPCR, 0, sizeof(fpu.FPCR));
    regs.d[3] = 0x8000;
    raw_write16(0, 0171003);
    raw_write16(2, 0x8000 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(fpu.FPCR.bsun);
}
BOOST_AUTO_TEST_SUITE_END()
