#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
double load_ext(uint32_t addr);
void store_ext(uint32_t addr, double v);
BOOST_FIXTURE_TEST_SUITE(FMOVEM_CR_FROM, InitFix)
BOOST_AUTO_TEST_CASE(fpiar_fpsr) {
    regs.a[3] = 0x10;
    memset(&regs.FPSR, 0, sizeof(regs.FPSR));
    regs.FPSR.n = true;
    asm_m68k("fmovem.l %FPSR/%FPIAR, (%A3)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x18);
    BOOST_TEST(raw_read32(0x10) == 1 << 27);
    BOOST_TEST(raw_read32(0x14) == 0);
}

BOOST_AUTO_TEST_CASE(fpiar_fpcr) {
    regs.a[3] = 0x10;
    memset(&regs.FPCR, 0, sizeof(regs.FPCR));
    regs.FPCR.bsun = true;
    asm_m68k("fmovem.l %FPCR/%FPIAR, (%A3)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x18);
    BOOST_TEST(raw_read32(0x10) == 0x8000);
    BOOST_TEST(raw_read32(0x14) == 0);
}

BOOST_AUTO_TEST_CASE(fpcr_fpsr) {
    regs.a[3] = 0x10;
    memset(&regs.FPCR, 0, sizeof(regs.FPCR));
    regs.FPCR.bsun = true;
    asm_m68k("fmovem.l %FPCR/%FPSR, (%A3)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x18);
    BOOST_TEST(raw_read32(0x10) == 0x8000);
    BOOST_TEST(raw_read32(0x14) == 1 << 27);
}

BOOST_AUTO_TEST_CASE(all) {
    regs.a[3] = 0x10;
    memset(&regs.FPSR, 0, sizeof(regs.FPSR));
    regs.FPSR.n = true;
    memset(&regs.FPCR, 0, sizeof(regs.FPCR));
    regs.FPCR.bsun = true;
    asm_m68k("fmovem.l %FPCR/%FPSR/%FPIAR, (%A3)+");
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x1C);
    BOOST_TEST(raw_read32(0x10) == 0x8000);
    BOOST_TEST(raw_read32(0x14) == 1 << 27);
    BOOST_TEST(raw_read32(0x18) == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FMOVEM_CR_TO, InitFix)
BOOST_AUTO_TEST_CASE(fpiar_fpsr) {
    regs.a[3] = 0x10;
    raw_write32(0x10, 1 << 27);
    raw_write32(0x14, 0x100);
    asm_m68k("fmovem.l (%A3)+, %FPSR/%FPIAR");
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x18);
    BOOST_TEST(regs.FPSR.n);
    BOOST_TEST(regs.fpiar == 0x100);
}

BOOST_AUTO_TEST_CASE(fpiar_fpcr) {
    regs.a[3] = 0x10;
    raw_write32(0x10, 0x8000);
    raw_write32(0x14, 0x100);
    asm_m68k("fmovem.l (%A3)+, %FPCR/%FPIAR");
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x18);
    BOOST_TEST(regs.FPCR.bsun);
    BOOST_TEST(regs.fpiar == 0x100);
}

BOOST_AUTO_TEST_CASE(fpcr_fpsr) {
    regs.a[3] = 0x10;
    raw_write32(0x10, 0x8000);
    raw_write32(0x14, 1 << 27);
    asm_m68k("fmovem.l (%A3)+, %FPCR/%FPSR");
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x18);
    BOOST_TEST(regs.FPSR.n);
    BOOST_TEST(regs.FPCR.bsun);
}

BOOST_AUTO_TEST_CASE(all) {
    regs.a[3] = 0x10;
    raw_write32(0x10, 0x8000);
    raw_write32(0x14, 1 << 27);
    raw_write32(0x18, 0x100);
    asm_m68k("fmovem.l (%A3)+, %FPCR/%FPSR/%FPIAR");
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x1C);
    BOOST_TEST(regs.FPSR.n);
    BOOST_TEST(regs.FPCR.bsun);
    BOOST_TEST(regs.fpiar == 0x100);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FPR_TO, InitFix)
BOOST_AUTO_TEST_CASE(addr) {
    regs.fp[1] = 1.0;
    regs.fp[2] = 2.0;
    regs.fp[3] = 3.0;
    regs.a[2] = 0x100;
    asm_m68k("fmovem.x %FP1-%FP3, (%A2)");
    m68k_do_execute();
    BOOST_TEST(load_ext(0x100) == 1.0);
    BOOST_TEST(load_ext(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext(0x100 + 12 * 2) == 3.0);
}

BOOST_AUTO_TEST_CASE(addr_d) {
    regs.fp[1] = 1.0;
    regs.fp[2] = 2.0;
    regs.fp[3] = 3.0;
    regs.d[3] = 0x70;
    regs.a[2] = 0x100;
    asm_m68k("fmovem.x %D3, (%A2)");
    m68k_do_execute();
    BOOST_TEST(load_ext(0x100) == 1.0);
    BOOST_TEST(load_ext(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext(0x100 + 12 * 2) == 3.0);
}

BOOST_AUTO_TEST_CASE(decr) {
    regs.fp[1] = 1.0;
    regs.fp[2] = 2.0;
    regs.fp[3] = 3.0;
    regs.a[2] = 0x100 + 12 * 3;
    asm_m68k("fmovem.x %FP1-%FP3, -(%A2)");
    m68k_do_execute();
    BOOST_TEST(load_ext(0x100) == 1.0);
    BOOST_TEST(load_ext(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext(0x100 + 12 * 2) == 3.0);
    BOOST_TEST(regs.a[2] == 0x100);
}
BOOST_AUTO_TEST_CASE(decr_d) {
    regs.fp[1] = 1.0;
    regs.fp[2] = 2.0;
    regs.fp[3] = 3.0;
    regs.d[3] = 0xE;
    regs.a[2] = 0x100 + 12 * 3;
    asm_m68k("fmovem.x %D3, -(%A2)");
    m68k_do_execute();
    BOOST_TEST(load_ext(0x100) == 1.0);
    BOOST_TEST(load_ext(0x100 + 12) == 2.0);
    BOOST_TEST(load_ext(0x100 + 12 * 2) == 3.0);
    BOOST_TEST(regs.a[2] == 0x100);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(FPR_FROM, InitFix)
BOOST_AUTO_TEST_CASE(addr) {
    regs.a[2] = 0x100;
    store_ext(0x100, 1.0);
    store_ext(0x100 + 12, 2.0);
    store_ext(0x100 + 12 * 2, 3.0);

    asm_m68k("fmovem.x (%A2), %FP1-%FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[1] == 1.0);
    BOOST_TEST(regs.fp[2] == 2.0);
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(addr_d) {
    regs.a[2] = 0x100;
    store_ext(0x100, 1.0);
    store_ext(0x100 + 12, 2.0);
    store_ext(0x100 + 12 * 2, 3.0);
    regs.d[3] = 0x70;
    asm_m68k("fmovem.x (%A2), %D3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[1] == 1.0);
    BOOST_TEST(regs.fp[2] == 2.0);
    BOOST_TEST(regs.fp[3] == 3.0);
}

BOOST_AUTO_TEST_CASE(incr) {
    regs.a[2] = 0x100;
    store_ext(0x100, 1.0);
    store_ext(0x100 + 12, 2.0);
    store_ext(0x100 + 12 * 2, 3.0);
    asm_m68k("fmovem.x (%A2)+, %FP1-%FP3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[1] == 1.0);
    BOOST_TEST(regs.fp[2] == 2.0);
    BOOST_TEST(regs.fp[3] == 3.0);
    BOOST_TEST(regs.a[2] == 0x100 + 12 * 3);
}

BOOST_AUTO_TEST_CASE(incr_d) {
    regs.a[2] = 0x100;
    store_ext(0x100, 1.0);
    store_ext(0x100 + 12, 2.0);
    store_ext(0x100 + 12 * 2, 3.0);
    regs.d[3] = 0x70;
    asm_m68k("fmovem.x (%A2)+, %D3");
    m68k_do_execute();
    BOOST_TEST(regs.fp[1] == 1.0);
    BOOST_TEST(regs.fp[2] == 2.0);
    BOOST_TEST(regs.fp[3] == 3.0);
    BOOST_TEST(regs.a[2] == 0x100 + 12 * 3);
}

BOOST_AUTO_TEST_SUITE_END()
