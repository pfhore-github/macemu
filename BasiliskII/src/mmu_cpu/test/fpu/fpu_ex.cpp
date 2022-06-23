#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(fpu_exception, InitFix)

BOOST_AUTO_TEST_CASE(uncnd) {
    fpu.FPCR.bsun = true;
    fpu.FPSR.nan = true;
    raw_write16(0, 0171200 | 16);
    raw_write16(2, 0x100);
    exception_check(48);
}

BOOST_AUTO_TEST_CASE(inex) {
    fpu.FPCR.inex2 = true;
    set_fpu_reg(1, 3.0);
    set_fpu_reg(2, 1.0);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x20);
    exception_check(49);
}

BOOST_AUTO_TEST_CASE(div0) {
    fpu.FPCR.dz = true;
    set_fpu_reg(1, 0.0);
    set_fpu_reg(2, 1.0);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x20);
    exception_check(50);
}

BOOST_AUTO_TEST_CASE(ovfl) {
    fpu.FPCR.ovfl = true;
    xval xm{false, 1, 0x3fe};
    set_fpu_reg(1, xm);
    set_fpu_reg(2, xm);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x67);
    exception_check(53);
}

BOOST_AUTO_TEST_CASE(unfl) {
    fpu.FPCR.unfl = true;
    xval xm{false, 1, -0x3fe};
    set_fpu_reg(1, xm);
    set_fpu_reg(2, xm);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x67);
    exception_check(51);
}

BOOST_AUTO_TEST_CASE(operr) {
    fpu.FPCR.operr = true;
    fpu.fp[1] = -1.0;
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x45);
    exception_check(52);
}

BOOST_AUTO_TEST_CASE(snan) {
    fpu.FPCR.snan = true;
    fpu.fp[1].set_nan(0x3F00000000000000LLU);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x18);
    exception_check(54);
}

BOOST_AUTO_TEST_SUITE_END()