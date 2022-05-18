#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
void load_ext(uint32_t addr, mpfr_ptr dst);
void store_ext(uint32_t addr, mpfr_ptr v);
double load_ext_v(uint32_t addr) {
    MPFR_DECL_INIT(tmp, 64);
    load_ext(addr, tmp);
    return mpfr_get_d(tmp, MPFR_RNDN);
}
void store_ext_v(uint32_t addr, double v) {
    MPFR_DECL_INIT(tmp, 64);
    mpfr_set_d(tmp, v, MPFR_RNDN);
    store_ext(addr, tmp);
}
BOOST_FIXTURE_TEST_SUITE(FMOVEM_CR_FROM, InitFix)
BOOST_AUTO_TEST_CASE(fpiar_fpsr) {
    auto ar = rand_ar();
    regs.a[ar] = 0x10;
    memset(&regs.fpu.FPSR, 0, sizeof(regs.fpu.FPSR));
    regs.fpu.FPSR.n = true;
    raw_write16(0, 0171030 | ar);
    raw_write16(2, 0xA000 | 1 << 10 | 1 << 11);
    m68k_do_execute();
    BOOST_TEST(regs.a[ar] == 0x18);
    BOOST_TEST(raw_read32(0x10) == 1 << 27);
    BOOST_TEST(raw_read32(0x14) == 0);
}

BOOST_AUTO_TEST_CASE(fpiar_fpcr) {
    auto ar = rand_ar();
    regs.a[ar] = 0x10;
    memset(&regs.fpu.FPCR, 0, sizeof(regs.fpu.FPCR));
    regs.fpu.FPCR.bsun = true;
    raw_write16(0, 0171030 | ar);
    raw_write16(2, 0xA000 | 1 << 10 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[ar] == 0x18);
    BOOST_TEST(raw_read32(0x10) == 0x8000);
    BOOST_TEST(raw_read32(0x14) == 0);
}

BOOST_AUTO_TEST_CASE(fpcr_fpsr) {
    auto ar = rand_ar();
    regs.a[ar] = 0x10;
    memset(&regs.fpu.FPCR, 0, sizeof(regs.fpu.FPCR));
    regs.fpu.FPCR.bsun = true;
    memset(&regs.fpu.FPSR, 0, sizeof(regs.fpu.FPSR));
    regs.fpu.FPSR.n = true;
    raw_write16(0, 0171030 | ar);
    raw_write16(2, 0xA000 | 1 << 11 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[ar] == 0x18);
    BOOST_TEST(raw_read32(0x10) == 0x8000);
    BOOST_TEST(raw_read32(0x14) == 1 << 27);
}

BOOST_AUTO_TEST_CASE(all) {
    auto ar = rand_ar();
    regs.a[ar] = 0x10;
    memset(&regs.fpu.FPSR, 0, sizeof(regs.fpu.FPSR));
    regs.fpu.FPSR.n = true;
    memset(&regs.fpu.FPCR, 0, sizeof(regs.fpu.FPCR));
    regs.fpu.FPCR.bsun = true;
    raw_write16(0, 0171030 | ar);
    raw_write16(2, 0xA000 | 1 << 10 | 1 << 11 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[ar] == 0x1C);
    BOOST_TEST(raw_read32(0x10) == 0x8000);
    BOOST_TEST(raw_read32(0x14) == 1 << 27);
    BOOST_TEST(raw_read32(0x18) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(FMOVEM_CR_TO, InitFix)
BOOST_AUTO_TEST_CASE(fpiar_fpsr) {
    auto ar = rand_ar();
    regs.a[ar] = 0x18;
    raw_write32(0x10, 1 << 27);
    raw_write32(0x14, 0x100);
    raw_write16(0, 0171040 | ar);
    raw_write16(2, 0x8000 | 1 << 10 | 1 << 11);
    m68k_do_execute();
    BOOST_TEST(regs.a[ar] == 0x10);
    BOOST_TEST(regs.fpu.FPSR.n);
    BOOST_TEST(regs.fpu.fpiar == 0x100);
}

BOOST_AUTO_TEST_CASE(fpiar_fpcr) {
    auto ar = rand_ar();
    regs.a[ar] = 0x18;
    raw_write32(0x10, 0x8000);
    raw_write32(0x14, 0x100);
    raw_write16(0, 0171040 | ar);
    raw_write16(2, 0x8000 | 1 << 10 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[ar] == 0x10);
    BOOST_TEST(regs.fpu.FPCR.bsun);
    BOOST_TEST(regs.fpu.fpiar == 0x100);
}

BOOST_AUTO_TEST_CASE(fpcr_fpsr) {
    auto ar = rand_ar();
    regs.a[ar] = 0x18;
    raw_write32(0x10, 0x8000);
    raw_write32(0x14, 1 << 27);
    raw_write16(0, 0171040 | ar);
    raw_write16(2, 0x8000 | 1 << 11 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[ar] == 0x10);
    BOOST_TEST(regs.fpu.FPSR.n);
    BOOST_TEST(regs.fpu.FPCR.bsun);
}

BOOST_AUTO_TEST_CASE(all) {
    auto ar = rand_ar();
    regs.a[ar] = 0x1C;
    raw_write32(0x10, 0x8000);
    raw_write32(0x14, 1 << 27);
    raw_write32(0x18, 0x100);
    raw_write16(0, 0171040 | ar);
    raw_write16(2, 0x8000 | 1 << 10 | 1 << 11 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[ar] == 0x10);
    BOOST_TEST(regs.fpu.FPSR.n);
    BOOST_TEST(regs.fpu.FPCR.bsun);
    BOOST_TEST(regs.fpu.fpiar == 0x100);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVEM_FPR_TO, InitFix)
BOOST_AUTO_TEST_CASE(addr) {
    auto ar = rand_ar();
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 2.0);
    set_fpu_reg(3, 3.0);
    regs.a[ar] = 0x100;
    raw_write16(0, 0171020 | ar);
    raw_write16(2, 0xF070);
    m68k_do_execute();
    BOOST_TEST(load_ext_v(0x100) == 1.0);
    BOOST_TEST(load_ext_v(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext_v(0x100 + 12 * 2) == 3.0);
}

BOOST_AUTO_TEST_CASE(addr_d) {
    auto ar = rand_ar();
    auto dr = rand_reg();
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 2.0);
    set_fpu_reg(3, 3.0);
    regs.d[dr] = 0x70;
    regs.a[ar] = 0x100;
    raw_write16(0, 0171020 | ar);
    raw_write16(2, 0xF800 | dr << 4);
    m68k_do_execute();
    BOOST_TEST(load_ext_v(0x100) == 1.0);
    BOOST_TEST(load_ext_v(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext_v(0x100 + 12 * 2) == 3.0);
}
BOOST_AUTO_TEST_CASE(decr) {
    auto ar = rand_ar();
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 2.0);
    set_fpu_reg(3, 3.0);
    raw_write16(0, 0171040 | ar);
    raw_write16(2, 0xE00E);
    regs.a[ar] = 0x100 + 12 * 3;
    m68k_do_execute();
    BOOST_TEST(load_ext_v(0x100) == 1.0);
    BOOST_TEST(load_ext_v(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext_v(0x100 + 12 * 2) == 3.0);
    BOOST_TEST(regs.a[ar] == 0x100);
}
BOOST_AUTO_TEST_CASE(decr_d) {
    auto ar = rand_ar();
    auto dr = rand_reg();
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 2.0);
    set_fpu_reg(3, 3.0);

    regs.d[dr] = 0xE;
    regs.a[ar] = 0x100 + 12 * 3;
    raw_write16(0, 0171040 | ar);
    raw_write16(2, 0xE800 | dr << 4);
    m68k_do_execute();
    BOOST_TEST(load_ext_v(0x100) == 1.0);
    BOOST_TEST(load_ext_v(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext_v(0x100 + 12 * 2) == 3.0);
    BOOST_TEST(regs.a[ar] == 0x100);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVEM_FPR_FROM, InitFix)
BOOST_AUTO_TEST_CASE(addr) {
    auto ar = rand_ar();
    regs.a[ar] = 0x100;
    store_ext_v(0x100, 1.0);
    store_ext_v(0x100 + 12, 2.0);
    store_ext_v(0x100 + 12 * 2, 3.0);
    raw_write16(0, 0171020 | ar);
    raw_write16(2, 0xD070);
    m68k_do_execute();
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[1], MPFR_RNDN) == 1.0);
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[2], MPFR_RNDN) == 2.0);
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[3], MPFR_RNDN) == 3.0);
}

BOOST_AUTO_TEST_CASE(addr_d) {
    auto ar = rand_ar();
    auto dr = rand_reg();
    regs.a[ar] = 0x100;
    store_ext_v(0x100, 1.0);
    store_ext_v(0x100 + 12, 2.0);
    store_ext_v(0x100 + 12 * 2, 3.0);
    regs.d[dr] = 0x70;
    raw_write16(0, 0171020 | ar);
    raw_write16(2, 0xD800 | dr << 4);
    m68k_do_execute();
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[1], MPFR_RNDN) == 1.0);
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[2], MPFR_RNDN) == 2.0);
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[3], MPFR_RNDN) == 3.0);
}

BOOST_AUTO_TEST_CASE(incr) {
    auto ar = rand_ar();
    regs.a[ar] = 0x100;
    store_ext_v(0x100, 1.0);
    store_ext_v(0x100 + 12, 2.0);
    store_ext_v(0x100 + 12 * 2, 3.0);
    raw_write16(0, 0171030 | ar);
    raw_write16(2, 0xD070);
    m68k_do_execute();
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[1], MPFR_RNDN) == 1.0);
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[2], MPFR_RNDN) == 2.0);
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[3], MPFR_RNDN) == 3.0);
    BOOST_TEST(regs.a[ar] == 0x100 + 12 * 3);
}

BOOST_AUTO_TEST_CASE(incr_d) {
    auto ar = rand_ar();
    auto dr = rand_reg();
    regs.a[ar] = 0x100;
    store_ext_v(0x100, 1.0);
    store_ext_v(0x100 + 12, 2.0);
    store_ext_v(0x100 + 12 * 2, 3.0);
    regs.d[dr] = 0x70;
    raw_write16(0, 0171030 | ar);
    raw_write16(2, 0xD800 | dr << 4);
    m68k_do_execute();
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[1], MPFR_RNDN) == 1.0);
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[2], MPFR_RNDN) == 2.0);
    BOOST_TEST(mpfr_get_d(regs.fpu.fp[3], MPFR_RNDN) == 3.0);
    BOOST_TEST(regs.a[ar] == 0x100 + 12 * 3);
}

BOOST_AUTO_TEST_SUITE_END()
