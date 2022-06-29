#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include "fpu/fpu_mpfr.h"
void load_ext(uint32_t addr, fpvalue &dst);
void store_ext(uint32_t addr, const fpvalue &v);
double load_ext_v(uint32_t addr) {
    fpvalue tmp;
    load_ext(addr, tmp);
    return static_cast<double>(tmp);
}
void store_ext_v(uint32_t addr, double v) {
    fpvalue tmp = v;
    store_ext(addr, tmp);
}
BOOST_FIXTURE_TEST_SUITE(FMOVEM_CR_FROM, InitFix)
BOOST_AUTO_TEST_CASE(fpiar_fpsr) {
    regs.a[2] = 0x10;
    memset(&fpu.FPSR, 0, sizeof(fpu.FPSR));
    fpu.FPSR.n = true;
    raw_write16(0, 0171032);
    raw_write16(2, 0xA000 | 1 << 10 | 1 << 11);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x18);
    BOOST_TEST(raw_read32(0x10) == 1 << 27);
    BOOST_TEST(raw_read32(0x14) == 0);
}

BOOST_AUTO_TEST_CASE(fpiar_fpcr) {
    regs.a[2] = 0x10;
    memset(&fpu.FPCR, 0, sizeof(fpu.FPCR));
    fpu.FPCR.bsun = true;
    raw_write16(0, 0171032);
    raw_write16(2, 0xA000 | 1 << 10 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x18);
    BOOST_TEST(raw_read32(0x10) == 0x8000);
    BOOST_TEST(raw_read32(0x14) == 0);
}

BOOST_AUTO_TEST_CASE(fpcr_fpsr) {
    regs.a[2] = 0x10;
    memset(&fpu.FPCR, 0, sizeof(fpu.FPCR));
    fpu.FPCR.bsun = true;
    memset(&fpu.FPSR, 0, sizeof(fpu.FPSR));
    fpu.FPSR.n = true;
    raw_write16(0, 0171032);
    raw_write16(2, 0xA000 | 1 << 11 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x18);
    BOOST_TEST(raw_read32(0x10) == 0x8000);
    BOOST_TEST(raw_read32(0x14) == 1 << 27);
}

BOOST_AUTO_TEST_CASE(all) {
    regs.a[2] = 0x10;
    memset(&fpu.FPSR, 0, sizeof(fpu.FPSR));
    fpu.FPSR.n = true;
    memset(&fpu.FPCR, 0, sizeof(fpu.FPCR));
    fpu.FPCR.bsun = true;
    raw_write16(0, 0171032);
    raw_write16(2, 0xA000 | 1 << 10 | 1 << 11 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x1C);
    BOOST_TEST(raw_read32(0x10) == 0x8000);
    BOOST_TEST(raw_read32(0x14) == 1 << 27);
    BOOST_TEST(raw_read32(0x18) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(FMOVEM_CR_TO, InitFix)
BOOST_AUTO_TEST_CASE(fpiar_fpsr) {
    regs.a[2] = 0x18;
    raw_write32(0x10, 1 << 27);
    raw_write32(0x14, 0x100);
    raw_write16(0, 0171042);
    raw_write16(2, 0x8000 | 1 << 10 | 1 << 11);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(fpu.FPSR.n);
    BOOST_TEST(fpu.fpiar == 0x100);
}

BOOST_AUTO_TEST_CASE(fpiar_fpcr) {
    regs.a[2] = 0x18;
    raw_write32(0x10, 0x8000);
    raw_write32(0x14, 0x100);
    raw_write16(0, 0171042);
    raw_write16(2, 0x8000 | 1 << 10 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(fpu.FPCR.bsun);
    BOOST_TEST(fpu.fpiar == 0x100);
}

BOOST_AUTO_TEST_CASE(fpcr_fpsr) {
    regs.a[2] = 0x18;
    raw_write32(0x10, 0x8000);
    raw_write32(0x14, 1 << 27);
    raw_write16(0, 0171042);
    raw_write16(2, 0x8000 | 1 << 11 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(fpu.FPSR.n);
    BOOST_TEST(fpu.FPCR.bsun);
}

BOOST_AUTO_TEST_CASE(all) {
    regs.a[2] = 0x1C;
    raw_write32(0x10, 0x8000);
    raw_write32(0x14, 1 << 27);
    raw_write32(0x18, 0x100);
    raw_write16(0, 0171042);
    raw_write16(2, 0x8000 | 1 << 10 | 1 << 11 | 1 << 12);
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(fpu.FPSR.n);
    BOOST_TEST(fpu.FPCR.bsun);
    BOOST_TEST(fpu.fpiar == 0x100);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVEM_FPR_TO, InitFix)
BOOST_AUTO_TEST_CASE(addr) {
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 2.0);
    set_fpu_reg(3, 3.0);
    regs.a[2] = 0x100;
    raw_write16(0, 0171022);
    raw_write16(2, 0xF070);
    m68k_do_execute();
    BOOST_TEST(load_ext_v(0x100) == 1.0);
    BOOST_TEST(load_ext_v(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext_v(0x100 + 12 * 2) == 3.0);
}

BOOST_AUTO_TEST_CASE(addr_d) {
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 2.0);
    set_fpu_reg(3, 3.0);
    regs.d[3] = 0x70;
    regs.a[2] = 0x100;
    raw_write16(0, 0171022);
    raw_write16(2, 0xF800 | 3 << 4);
    m68k_do_execute();
    BOOST_TEST(load_ext_v(0x100) == 1.0);
    BOOST_TEST(load_ext_v(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext_v(0x100 + 12 * 2) == 3.0);
}
BOOST_AUTO_TEST_CASE(decr) {
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 2.0);
    set_fpu_reg(3, 3.0);
    raw_write16(0, 0171042);
    raw_write16(2, 0xE00E);
    regs.a[2] = 0x100 + 12 * 3;
    m68k_do_execute();
    BOOST_TEST(load_ext_v(0x100) == 1.0);
    BOOST_TEST(load_ext_v(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext_v(0x100 + 12 * 2) == 3.0);
    BOOST_TEST(regs.a[2] == 0x100);
}
BOOST_AUTO_TEST_CASE(decr_d) {
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 2.0);
    set_fpu_reg(3, 3.0);

    regs.d[3] = 0xE;
    regs.a[2] = 0x100 + 12 * 3;
    raw_write16(0, 0171042);
    raw_write16(2, 0xE800 | 3 << 4);
    m68k_do_execute();
    BOOST_TEST(load_ext_v(0x100) == 1.0);
    BOOST_TEST(load_ext_v(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext_v(0x100 + 12 * 2) == 3.0);
    BOOST_TEST(regs.a[2] == 0x100);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVEM_FPR_FROM, InitFix)
BOOST_AUTO_TEST_CASE(addr) {
    regs.a[2] = 0x100;
    store_ext_v(0x100, 1.0);
    store_ext_v(0x100 + 12, 2.0);
    store_ext_v(0x100 + 12 * 2, 3.0);
    raw_write16(0, 0171022);
    raw_write16(2, 0xD070);
    m68k_do_execute();
    BOOST_TEST(static_cast<double>(fpu.fp[1]) == 1.0);
    BOOST_TEST(static_cast<double>(fpu.fp[2]) == 2.0);
    BOOST_TEST(static_cast<double>(fpu.fp[3]) == 3.0);
}

BOOST_AUTO_TEST_CASE(addr_d) {
    regs.a[2] = 0x100;
    store_ext_v(0x100, 1.0);
    store_ext_v(0x100 + 12, 2.0);
    store_ext_v(0x100 + 12 * 2, 3.0);
    regs.d[3] = 0x70;
    raw_write16(0, 0171022);
    raw_write16(2, 0xD800 | 3 << 4);
    m68k_do_execute();
    BOOST_TEST(static_cast<double>(fpu.fp[1]) == 1.0);
    BOOST_TEST(static_cast<double>(fpu.fp[2]) == 2.0);
    BOOST_TEST(static_cast<double>(fpu.fp[3]) == 3.0);
}

BOOST_AUTO_TEST_CASE(incr) {
    regs.a[2] = 0x100;
    store_ext_v(0x100, 1.0);
    store_ext_v(0x100 + 12, 2.0);
    store_ext_v(0x100 + 12 * 2, 3.0);
    raw_write16(0, 0171032);
    raw_write16(2, 0xD070);
    m68k_do_execute();
    BOOST_TEST(static_cast<double>(fpu.fp[1]) == 1.0);
    BOOST_TEST(static_cast<double>(fpu.fp[2]) == 2.0);
    BOOST_TEST(static_cast<double>(fpu.fp[3]) == 3.0);
    BOOST_TEST(regs.a[2] == 0x100 + 12 * 3);
}

BOOST_AUTO_TEST_CASE(incr_d) {
    regs.a[2] = 0x100;
    store_ext_v(0x100, 1.0);
    store_ext_v(0x100 + 12, 2.0);
    store_ext_v(0x100 + 12 * 2, 3.0);
    regs.d[3] = 0x70;
    raw_write16(0, 0171032);
    raw_write16(2, 0xD800 | 3 << 4);
    m68k_do_execute();
    BOOST_TEST(static_cast<double>(fpu.fp[1]) == 1.0);
    BOOST_TEST(static_cast<double>(fpu.fp[2]) == 2.0);
    BOOST_TEST(static_cast<double>(fpu.fp[3]) == 3.0);
    BOOST_TEST(regs.a[2] == 0x100 + 12 * 3);
}

BOOST_AUTO_TEST_SUITE_END()
