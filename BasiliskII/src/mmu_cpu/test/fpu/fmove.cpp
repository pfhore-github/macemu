#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <bit>

BOOST_FIXTURE_TEST_SUITE(FMOVE_FROM, InitFix)
BOOST_AUTO_TEST_CASE(REG) {
    double c = get_rx(-2.0, 2.0);
    fpu_test(0, c, 0.0, c);
}
BOOST_AUTO_TEST_CASE(Byte) {
    auto dstr = rand_reg();
    int8_t v = get_v8();
    raw_write8(0x10, v);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 6 << 10 | dstr << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x11);
    BOOST_TEST(mpfr_get_si(regs.fpu.fp[dstr], MPFR_RNDN) == v);
}

BOOST_AUTO_TEST_CASE(Word) {
    auto dstr = rand_reg();
    int16_t v = get_v16();
    raw_write16(0x10, v);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 4 << 10 | dstr << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x12);
    BOOST_TEST(mpfr_get_si(regs.fpu.fp[dstr], MPFR_RNDN) == v);
}

BOOST_AUTO_TEST_CASE(Long) {
    auto dstr = rand_reg();
    int32_t v = get_v32();
    raw_write32(0x10, v);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 0 << 10 | dstr << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(mpfr_get_si(regs.fpu.fp[dstr], MPFR_RNDN) == v);
}

BOOST_AUTO_TEST_CASE(Single) {
    auto dstr = rand_reg();
    float v = get_rx(-2.0, 2.0);
    raw_write32(0x10, std::bit_cast<uint32_t>(v));
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 1 << 10 | dstr << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_CHECK_CLOSE(mpfr_get_flt(regs.fpu.fp[dstr], MPFR_RNDN), v, 1e-04);
}

BOOST_AUTO_TEST_CASE(Double) {
    auto dstr = rand_reg();
    double v = get_rx(-2.0, 2.0);
    uint64_t vi = std::bit_cast<uint64_t>(v);
    raw_write32(0x10, vi >> 32);
    raw_write32(0x14, vi);
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 5 << 10 | dstr << 7);
    regs.a[1] = 0x10;
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    BOOST_CHECK_CLOSE(mpfr_get_d(regs.fpu.fp[dstr], MPFR_RNDN), v, 1e-08);
}

BOOST_AUTO_TEST_CASE(Ext) {
    auto dstr = rand_reg();
    raw_write32(0x10, 16383 << 16);
    raw_write32(0x14, 0xaaaaaaaa);
    raw_write32(0x18, 0xaaaaaaaa);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 2 << 10 | dstr << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_CHECK_CLOSE(mpfr_get_d(regs.fpu.fp[dstr], MPFR_RNDN), 1.3333333333,
                      1e-08);
}

BOOST_AUTO_TEST_CASE(packed, *boost::unit_test::tolerance(0.0001)) {
    auto dstr = rand_reg();
    raw_write32(0x10, 0x00020001);
    raw_write32(0x14, 0x33333333);
    raw_write32(0x18, 0x44444444);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 3 << 10 | dstr << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_CHECK_CLOSE(mpfr_get_d(regs.fpu.fp[dstr], MPFR_RNDN),
                      1.3333333344444445e+2, 1e-08);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(FSMOVE, InitFix)
BOOST_AUTO_TEST_CASE(operand) {
    float c = get_rx(-2.0, 2.0);
    fpu_test(0x40, c, 0.0f, c);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FDMOVE, InitFix)
BOOST_AUTO_TEST_CASE(REG) {
    double c = get_rx(-2.0, 2.0);
    fpu_test(0x44, c, 0.0, c);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVE_TO, InitFix)

BOOST_AUTO_TEST_CASE(Byte) {
    auto reg = rand_reg();
    int8_t v = get_v8();
    set_fpu_reg(reg, static_cast<double>(v));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 6 << 10 | reg << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x11);
    BOOST_TEST(static_cast<int8_t>(raw_read8(0x10)) == v);
}

BOOST_AUTO_TEST_CASE(Word) {
    auto reg = rand_reg();
    int16_t v = get_v16();
    set_fpu_reg(reg, static_cast<double>(v));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 4 << 10 | reg << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x12);
    BOOST_TEST(static_cast<int16_t>(raw_read16(0x10)) == v);
}

BOOST_AUTO_TEST_CASE(Long) {
    auto reg = rand_reg();
    int32_t v = get_v16();
    set_fpu_reg(reg, static_cast<double>(v));
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 0 << 10 | reg << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(static_cast<int32_t>(raw_read32(0x10)) == v);
}

BOOST_AUTO_TEST_CASE(Single) {
    auto reg = rand_reg();
    float v = get_rx(-3.0, 3.0);
    set_fpu_reg(reg, v);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 1 << 10 | reg << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x14);
    BOOST_TEST(std::bit_cast<float>(raw_read32(0x10)) == v);
}

BOOST_AUTO_TEST_CASE(Double) {
    auto reg = rand_reg();
    double v = get_rx(-3.0, 3.0);
    set_fpu_reg(reg, v);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 5 << 10 | reg << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x18);
    uint64_t vx =
        static_cast<uint64_t>(raw_read32(0x10)) << 32 | raw_read32(0x14);
    BOOST_TEST(std::bit_cast<double>(vx) == v);
}

BOOST_AUTO_TEST_CASE(Ext) {
    regs.fpu.rnd_mode = MPFR_RNDN;
    auto reg = rand_reg();
    MPFR_DECL_INIT(tmp, 64);
    mpfr_set_d(tmp, 4.0, MPFR_RNDN);
    mpfr_div_d(tmp, tmp, 3.0, MPFR_RNDN);
    mpfr_prec_round(regs.fpu.fp[reg], 64, MPFR_RNDN);
    mpfr_set(regs.fpu.fp[reg], tmp, MPFR_RNDN);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 2 << 10 | reg << 7);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 16383 << 16);
    BOOST_TEST(raw_read32(0x14) == 0xaaaaaaaa);
    BOOST_TEST(raw_read32(0x18) == 0xaaaaaaab);
}

BOOST_AUTO_TEST_SUITE(packed_fixed)
BOOST_AUTO_TEST_CASE(positive_k) {
    regs.fpu.rnd_mode = MPFR_RNDN;
    auto reg = rand_reg();
    set_fpu_reg(reg, 12345.678765);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | reg << 7 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001);
    BOOST_TEST(raw_read32(0x14) == 0x23000000);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_AUTO_TEST_CASE(negative_k) {
    regs.fpu.rnd_mode = MPFR_RNDN;
    auto reg = rand_reg();
    set_fpu_reg(reg, 12345.678765);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | reg << 7 | 0x7D);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001);
    BOOST_TEST(raw_read32(0x14) == 0x23456790);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_AUTO_TEST_CASE(negative_value) {
    regs.fpu.rnd_mode = MPFR_RNDN;
    auto reg = rand_reg();
    set_fpu_reg(reg, -12345.678765);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | reg << 7 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x80040001);
    BOOST_TEST(raw_read32(0x14) == 0x23000000);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_AUTO_TEST_CASE(negative_exp) {
    regs.fpu.rnd_mode = MPFR_RNDN;
    auto reg = rand_reg();
    set_fpu_reg(reg, 0.00012345678765);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | reg << 7 | 3);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x40040001);
    BOOST_TEST(raw_read32(0x14) == 0x23000000);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(packed_dyn)
BOOST_AUTO_TEST_CASE(positive_k) {
    regs.fpu.rnd_mode = MPFR_RNDN;
    auto reg = rand_reg();
    auto dreg = rand_reg();
    set_fpu_reg(reg, 12345.678765);
    regs.a[1] = 0x10;
    regs.d[dreg] = 3;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 7 << 10 | reg << 7 | dreg << 4);
    m68k_do_execute();

    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001);
    BOOST_TEST(raw_read32(0x14) == 0x23000000);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_AUTO_TEST_CASE(negative_k) {
    regs.fpu.rnd_mode = MPFR_RNDN;
    auto reg = rand_reg();
    auto dreg = rand_reg();
    set_fpu_reg(reg, 12345.678765);
    regs.a[1] = 0x10;
    regs.d[dreg] = -3;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 7 << 10 | reg << 7 | dreg << 4);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1c);
    BOOST_TEST(raw_read32(0x10) == 0x00040001);
    BOOST_TEST(raw_read32(0x14) == 0x23456790);
    BOOST_TEST(raw_read32(0x18) == 0x00000000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVE_CR_FROM, InitFix)
BOOST_AUTO_TEST_CASE(fpiar) {
    auto ar = rand_ar();
    regs.a[ar] = 0x100;
    raw_write16(0, 0171010 | ar);
    raw_write16(2, 0xA000 | 1 << 10);
    m68k_do_execute();
    BOOST_TEST(regs.a[ar] == 0);
}

BOOST_AUTO_TEST_CASE(fpsr) {
    auto dr = rand_reg();
    memset(&regs.fpu.FPSR, 0, sizeof(regs.fpu.FPSR));
    regs.fpu.FPSR.n = true;
    raw_write16(0, 0171000 | dr);
    raw_write16(2, 0xA000 | 1 << 11);
    m68k_do_execute();
    BOOST_TEST(regs.d[dr] == 1 << 27);
}

BOOST_AUTO_TEST_CASE(fpcr) {
    auto dr = rand_reg();
    memset(&regs.fpu.FPCR, 0, sizeof(regs.fpu.FPCR));
    regs.fpu.FPCR.bsun = true;
    raw_write16(0, 0171000 | dr);
    raw_write16(2, 0xA000 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.d[dr] == 0x8000);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVE_CR_TO, InitFix)
BOOST_AUTO_TEST_CASE(fpiar) {
    auto ar = rand_ar();
    regs.a[ar] = 0x100;
    raw_write16(0, 0171010 | ar);
    raw_write16(2, 0x8000 | 1 << 10);
    m68k_do_execute();
    BOOST_TEST(regs.fpu.fpiar == 0x100);
}

BOOST_AUTO_TEST_CASE(fpsr) {
    auto dr = rand_reg();
    memset(&regs.fpu.FPSR, 0, sizeof(regs.fpu.FPSR));
    regs.d[dr] = 1 << 27;
    raw_write16(0, 0171000 | dr);
    raw_write16(2, 0x8000 | 1 << 11);
    m68k_do_execute();
    BOOST_TEST(regs.fpu.FPSR.n);
}

BOOST_AUTO_TEST_CASE(fpcr) {
    auto dr = rand_reg();
    memset(&regs.fpu.FPCR, 0, sizeof(regs.fpu.FPCR));
    regs.d[dr] = 0x8000;
    raw_write16(0, 0171000 | dr);
    raw_write16(2, 0x8000 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.fpu.FPCR.bsun);
}
BOOST_AUTO_TEST_SUITE_END()
