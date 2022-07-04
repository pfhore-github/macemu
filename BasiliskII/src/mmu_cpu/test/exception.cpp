#define BOOST_TEST_DYN_LINK
#include "exception.h"
#include "memory.h"
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include "test/test_common.h"
void m68k_do_execute();

BOOST_FIXTURE_TEST_SUITE(exception, InitFix)
BOOST_AUTO_TEST_CASE(RESET) {
    raw_write32(0, 0x1000);
    raw_write32(4, 0x2000);
    regs.S = false;
    regs.M = true;
    regs.T = 1;
    regs.IM = 2;
    regs.vbr = 0x1000;
    regs.must_reset.store(true);
    m68k_do_execute();
    BOOST_TEST(regs.S);
    BOOST_TEST(!regs.M);
    BOOST_TEST(regs.T == 0);
    BOOST_TEST(regs.IM == 7);
    BOOST_TEST(regs.vbr == 0);
    BOOST_TEST(regs.a[7] == 0x1000);
    BOOST_TEST(regs.pc == 0x2000);
}
BOOST_AUTO_TEST_SUITE(ACCESS_FAULT)
int szz[] = {3, 1, 0, 2};
BOOST_DATA_TEST_CASE(SIZE, bdata::xrange(1, 4), sz) {
    regs.a[2] = 0x60000000;
    raw_write16(0, 0001022 | sz << 12);
    exception_check(2, 7);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
    BOOST_TEST((raw_read16(regs.a[7] + 0x0c) >> 5 & 3) == szz[sz]);
    BOOST_TEST(raw_read32(regs.a[7] + 0x14) == 0x60000000);
}

BOOST_AUTO_TEST_CASE(READ) {
    mmu.tcr_e = false;
    regs.a[2] = 0x60000000;
    raw_write16(0, 010022);
    exception_check(2, 7);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
    BOOST_TEST(raw_read16(regs.a[7] + 0x0c) & 1 << 8);
    BOOST_TEST(raw_read32(regs.a[7] + 0x14) == 0x60000000);
}

BOOST_AUTO_TEST_CASE(WRITE) {
    mmu.tcr_e = false;
    regs.a[2] = 0x60000000;
    raw_write16(0, 0012200);
    exception_check(2, 7);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
    BOOST_TEST(!(raw_read16(regs.a[7] + 0x0c) & 1 << 8));
    BOOST_TEST(raw_read32(regs.a[7] + 0x14) == 0x60000000);
}

BOOST_AUTO_TEST_CASE(LK) {
    mmu.tcr_e = false;
    regs.a[2] = 0x60000000;
    raw_write16(0, 0051022);
    exception_check(2, 7);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
    BOOST_TEST(raw_read16(regs.a[7] + 0x0c) & 1 << 9);
    BOOST_TEST(raw_read32(regs.a[7] + 0x14) == 0x60000000);
}

BOOST_AUTO_TEST_CASE(ATC) {
    regs.S = false;
    mmu.tcr_e = true;
    mmu.srp = mmu.urp = 0x100000;
    regs.a[2] = 0x1000000;
    mmu_d.TTR[0].S = 2;
    mmu_d.TTR[0].E = true;
    mmu_d.TTR[0].address_base = 0;
    mmu_i.TTR[0].S = 2;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    raw_write16(0, 0001022);
    exception_check(2, 7);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
    BOOST_TEST(raw_read16(regs.a[7] + 0x0c) & 1 << 10);
    BOOST_TEST(raw_read32(regs.a[7] + 0x14) == 0x1000000);
}

BOOST_AUTO_TEST_CASE(MA) {
    regs.S = false;
    mmu.tcr_e = true;
    mmu.srp = mmu.urp = 0x1000;
    regs.a[2] = 0x1FFE;
    mmu_i.TTR[0].S = 2;
    mmu_i.TTR[0].E = true;
    mmu_i.TTR[0].address_base = 0;
    raw_write16(0, 0020022);
    raw_write32(0x1000, 0x2002);
    raw_write32(0x2000, 0x3002);
    raw_write32(0x3000, 0x0003);
    raw_write32(0x3004, 0x1003);
    raw_write32(0x3008, 0);
    raw_write32(0x300C, 0x3003);
    raw_write32(0x3010, 0x4003);
    regs.dfc = 1;
    mmu.pflush();
    exception_check(2, 7);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
    BOOST_TEST(raw_read16(regs.a[7] + 0x0c) & 1 << 11);
    BOOST_TEST(raw_read32(regs.a[7] + 0x14) == 0x1FFE);
}

BOOST_AUTO_TEST_CASE(MOVEM) {
    mmu.tcr_e = false;
    regs.i_ea = 0;
    regs.a[2] = 0x60000000;
    raw_write16(0, 0044222);
    raw_write16(2, 0x0E0E);
    exception_check(2, 7);
    BOOST_TEST(raw_read16(regs.a[7] + 0x0c) & 1 << 12);
}

BOOST_AUTO_TEST_CASE(TRACE) {
    mmu.tcr_e = false;
    regs.i_ea = 0;
    regs.T = 2;
    regs.a[2] = 0x60000000;
    raw_write16(0, 010022);
    exception_check(2, 7);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
    BOOST_TEST(raw_read16(regs.a[7] + 0x0c) & 1 << 13);
    BOOST_TEST(raw_read32(regs.a[7] + 0x14) == 0x60000000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_CASE(ADDRESS_ERROR) {
    raw_write16(0, 0x6023);
    exception_check(3, 2);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0x24);
}

BOOST_AUTO_TEST_CASE(ILLEGAL) {
    raw_write16(0, 0045374);
    exception_check(4, 0);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
}

BOOST_AUTO_TEST_CASE(Div0) {
    regs.d[2] = 1;
    regs.d[1] = 0;
    raw_write16(0, 0046101);
    raw_write16(2, 0x2002);
    exception_check(5, 2);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 4);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
}

BOOST_AUTO_TEST_CASE(CHK) {
    regs.d[1] = 100;
    regs.d[2] = -1;
    raw_write16(0, 0042601);
    exception_check(6, 2);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 2);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
}

BOOST_AUTO_TEST_CASE(TRAPV) {
    regs.v = true;
    raw_write16(0, 0047166);
    exception_check(7, 2);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 2);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
}

BOOST_AUTO_TEST_CASE(PRIV) {
    regs.S = false;
    raw_write16(0, 0040302);
    exception_check(8, 0);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
}
BOOST_AUTO_TEST_SUITE(TRACE)
BOOST_AUTO_TEST_CASE(INST) {
    regs.T = 2;
    raw_write16(0, 0047161);
    exception_check(9, 2);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 2);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
}

BOOST_AUTO_TEST_CASE(B_TAKEN) {
    regs.T = 1;
    raw_write16(0, 0047161);
    exception_check(9, 2);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 2);
    BOOST_TEST(raw_read32(regs.a[7] + 8) == 0);
}

BOOST_AUTO_TEST_CASE(B_NONTAKEN) {
    regs.T = 1;
    raw_write16(0, 0);
    raw_write16(2, 0);
    exception_check(0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(ALine) {
    regs.S = false;
    raw_write16(0, 0xabcd);
    exception_check(10, 0);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
}

BOOST_AUTO_TEST_CASE(FLine) {
    regs.S = false;
    raw_write16(0, 0xffff);
    exception_check(11, 0);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
}
BOOST_AUTO_TEST_CASE(FormatError) {
    regs.S = true;
    regs.isp = regs.msp = regs.a[7] = 0x1000;
    raw_write16(0x1000, 0x1234);
    raw_write32(0x1002, 0x1000);
    raw_write16(0x1006, 0xFFFF);
    raw_write16(0, 0047163);
    exception_check(14, 0);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
}
BOOST_AUTO_TEST_SUITE(IRQ)
BOOST_DATA_TEST_CASE(non_masked, bdata::xrange(1, 8), iq) {
    regs.IM = 0;
    regs.irq = 1 << iq;
    raw_write16(0, 0047161);
    exception_check(25 + iq, 0);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 2);
    BOOST_TEST(regs.IM == iq);
}

BOOST_DATA_TEST_CASE(masked, bdata::xrange(1, 7), iq) {
    regs.IM = 7;
    regs.irq = 1 << iq;
    raw_write16(0, 0047161);
    exception_check(0);
}

BOOST_DATA_TEST_CASE(throw_away, bdata::xrange(1, 8), iq) {
    regs.IM = 0;
    regs.S = true;
    regs.M = true;
    regs.irq = 1 << iq;
    raw_write16(0, 0047161);

    regs.usp = regs.isp = regs.a[7] = 0x1000;
    regs.vbr = 0x3000;
    raw_write32(0x3000 + (25 + iq) * 4, 0x5000);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x5000);
    BOOST_TEST(raw_read16(regs.a[7] + 6) == (1 << 12 | (25 + iq) * 4));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_DATA_TEST_CASE(TRAP, bdata::xrange(0, 16), n) {
    raw_write16(0, 0047100 | n);
    exception_check(32 + n, 0);
    BOOST_TEST(raw_read32(regs.a[7] + 2) == 2);
}

BOOST_DATA_TEST_CASE(BSUN, BIT, enabled) {
    fpu.FPSR.nan = true;
    fpu.FPCR.bsun = enabled;
    raw_write16(0, 0171200 | 16);
    raw_write16(2, 0x100);
    if(enabled) {
        exception_check(48, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
}

BOOST_DATA_TEST_CASE(snan, BIT, enabled) {
    fpu.FPCR.snan = enabled;
    fpu.fp[1].set_nan(1);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7);
    if(enabled) {
        exception_check(54, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
    BOOST_TEST(fpu.fp[2].is_nan());
    BOOST_TEST(fpu.fp[2].get_payload() == (1LLU << 62) | 1);
}
BOOST_AUTO_TEST_SUITE(operr)
BOOST_DATA_TEST_CASE(operation, BIT, enabled) {
    fpu.FPCR.operr = enabled;
    fpu.fp[1] = -1.0;
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 1 << 7 | 4);
    if(enabled) {
        exception_check(52, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
        BOOST_TEST(static_cast<double>(fpu.fp[1]) == -1.0);
    } else {
        exception_check(0);
        BOOST_TEST(fpu.fp[1].is_nan());
        BOOST_TEST(fpu.fp[1].get_payload() == 0xffffffffffffffffLLU);
    }
}

BOOST_DATA_TEST_CASE(to_byte, BIT *SIGN, enabled, sign) {
    fpu.FPCR.operr = enabled;
    fpu.fp[1] = copysign(200.0, sign);
    raw_write16(0, 0171001);
    raw_write16(2, 3 << 13 | 6 << 10 | 1 << 7);
    if(enabled) {
        exception_check(52, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
    BOOST_TEST(static_cast<int8_t>(regs.d[1]) == (sign < 0 ? -128 : 127));
}

BOOST_DATA_TEST_CASE(to_word, BIT *SIGN, enabled, sign) {
    fpu.FPCR.operr = enabled;
    fpu.fp[1] = copysign(100000.0, sign);
    raw_write16(0, 0171001);
    raw_write16(2, 3 << 13 | 4 << 10 | 1 << 7);
    if(enabled) {
        exception_check(52, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
    BOOST_TEST(static_cast<int16_t>(regs.d[1]) == (sign < 0 ? -32768 : 32767));
}

BOOST_DATA_TEST_CASE(to_lword, BIT *SIGN, enabled, sign) {
    fpu.FPCR.operr = enabled;
    fpu.fp[1] = copysign(1e44, sign);
    raw_write16(0, 0171001);
    raw_write16(2, 3 << 13 | 0 << 10 | 1 << 7);
    if(enabled) {
        exception_check(52, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
    BOOST_TEST(regs.d[1] == (sign < 0 ? 0x80000000 : 0x7fffffff));
}

BOOST_DATA_TEST_CASE(to_pakced, BIT, enabled) {
    fpu.FPCR.operr = enabled;
    set_fpu_reg(2, 12345.678765);
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | 2 << 7 | 18);

    if(enabled) {
        exception_check(52, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
}

BOOST_DATA_TEST_CASE(to_pakced2, BIT, enabled) {
    fpu.FPCR.operr = enabled;
    fpu.fp[2] = "1e1001";
    regs.a[1] = 0x10;
    raw_write16(0, 0171031);
    raw_write16(2, 3 << 13 | 3 << 10 | 2 << 7 | 3);

    if(enabled) {
        exception_check(52, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
    BOOST_TEST(raw_read32(0x10) == 0x00011001);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ovfl)
template <class V> void check_ovfl(const V &v1, const V &v2, bool enabled) {
    set_fpu_reg(1, v1);
    set_fpu_reg(2, v2);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x23);

    if(enabled) {
        exception_check(53, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
}
BOOST_AUTO_TEST_SUITE(ext)
BOOST_DATA_TEST_CASE(rn, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_EXT;
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    xval xm1{sign < 0, 1, 0x3fff};
    xval xm2{false, 1, 0x3fff};
    check_ovfl(xm1, xm2, enabled);
    BOOST_TEST(fpu.fp[2].is_inf());
    BOOST_TEST(fpu.fp[2].signbit() == (sign < 0));
}

BOOST_DATA_TEST_CASE(rz, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_EXT;
    mpfr_set_default_rounding_mode(MPFR_RNDZ);
    xval xm1{sign < 0, 1, 0x3fff};
    xval xm2{false, 1, 0x3fff};
    check_ovfl(xm1, xm2, enabled);
    auto [sg, fra, exp] = fpu.fp[2].get_zexp();
    BOOST_TEST(sg == (sign < 0));
    BOOST_TEST(exp == 32765);
    BOOST_TEST(fra == 0xffffffffffffffffLLU);
}

BOOST_DATA_TEST_CASE(rm, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_EXT;
    mpfr_set_default_rounding_mode(MPFR_RNDD);
    xval xm1{sign < 0, 1, 0x3fff};
    xval xm2{false, 1, 0x3fff};
    check_ovfl(xm1, xm2, enabled);
    if(sign > 0) {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(!sg);
        BOOST_TEST(exp == 32765);
        BOOST_TEST(fra == 0xffffffffffffffffLLU);
    } else {
        BOOST_TEST(fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_inf());
    }
}

BOOST_DATA_TEST_CASE(rp, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_EXT;
    mpfr_set_default_rounding_mode(MPFR_RNDU);
    xval xm1{sign < 0, 1, 0x3fff};
    xval xm2{false, 1, 0x3fff};
    check_ovfl(xm1, xm2, enabled);
    if(sign < 0) {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(sg);
        BOOST_TEST(exp == 32765);
        BOOST_TEST(fra == 0xffffffffffffffffLLU);
    } else {
        BOOST_TEST(!fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_inf());
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(dbl)
BOOST_DATA_TEST_CASE(rn, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_DBL;
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    xval xm1{sign < 0, 1, 0x3fe};
    xval xm2{false, 1, 0x3fe};
    check_ovfl(xm1, xm2, enabled);
    BOOST_TEST(fpu.fp[2].is_inf());
    BOOST_TEST(fpu.fp[2].signbit() == (sign < 0));
}

BOOST_DATA_TEST_CASE(rz, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_DBL;
    mpfr_set_default_rounding_mode(MPFR_RNDZ);
    xval xm1{sign < 0, 1, 0x3fe};
    xval xm2{false, 1, 0x3fe};
    check_ovfl(xm1, xm2, enabled);
    auto [sg, fra, exp] = fpu.fp[2].get_zexp();
    BOOST_TEST(sg == (sign < 0));
    BOOST_TEST(exp == 0x3ffe + 1023);
    BOOST_TEST(fra == 0xfffffffffffff800LLU);
}

BOOST_DATA_TEST_CASE(rm, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_DBL;
    mpfr_set_default_rounding_mode(MPFR_RNDD);
    xval xm1{sign < 0, 1, 0x3fe};
    xval xm2{false, 1, 0x3fe};
    check_ovfl(xm1, xm2, enabled);
    if(sign > 0) {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(!sg);
        BOOST_TEST(exp == 0x3ffe + 1023);
        BOOST_TEST(fra == 0xfffffffffffff800LLU);

    } else {
        BOOST_TEST(fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_inf());
    }
}

BOOST_DATA_TEST_CASE(rp, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_DBL;
    mpfr_set_default_rounding_mode(MPFR_RNDU);
    xval xm1{sign < 0, 1, 0x3fe};
    xval xm2{false, 1, 0x3fe};
    check_ovfl(xm1, xm2, enabled);
    if(sign < 0) {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(sg);
        BOOST_TEST(exp == 0x3ffe + 1023);
        BOOST_TEST(fra == 0xfffffffffffff800LLU);
    } else {
        BOOST_TEST(!fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_inf());
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(sgl)
BOOST_DATA_TEST_CASE(rn, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_SGL;
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    xval xm1{sign < 0, 1, 0x7e};
    xval xm2{false, 1, 0x7e};
    check_ovfl(xm1, xm2, enabled);
    BOOST_TEST(fpu.fp[2].is_inf());
    BOOST_TEST(fpu.fp[2].signbit() == (sign < 0));
}

BOOST_DATA_TEST_CASE(rz, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_SGL;
    mpfr_set_default_rounding_mode(MPFR_RNDZ);
    xval xm1{sign < 0, 1, 0x7e};
    xval xm2{false, 1, 0x7e};
    check_ovfl(xm1, xm2, enabled);
    auto [sg, fra, exp] = fpu.fp[2].get_zexp();
    BOOST_TEST(sg == (sign < 0));
    BOOST_TEST(exp == 0x3ffe + 0x7f);
    BOOST_TEST(fra == 0xffffff0000000000LLU);
}

BOOST_DATA_TEST_CASE(rm, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_SGL;
    mpfr_set_default_rounding_mode(MPFR_RNDD);
    xval xm1{sign < 0, 1, 0x7e};
    xval xm2{false, 1, 0x7e};
    check_ovfl(xm1, xm2, enabled);
    if(sign > 0) {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(!sg);
        BOOST_TEST(exp == 0x3ffe + 0x7f);
        BOOST_TEST(fra == 0xffffff0000000000LLU);

    } else {
        BOOST_TEST(fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_inf());
    }
}

BOOST_DATA_TEST_CASE(rp, BIT *SIGN, enabled, sign) {
    fpu.FPCR.ovfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_SGL;
    mpfr_set_default_rounding_mode(MPFR_RNDU);
    xval xm1{sign < 0, 1, 0x7e};
    xval xm2{false, 1, 0x7e};
    check_ovfl(xm1, xm2, enabled);
    if(sign < 0) {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(sg);
        BOOST_TEST(exp == 0x3ffe + 0x7f);
        BOOST_TEST(fra == 0xffffff0000000000LLU);
    } else {
        BOOST_TEST(!fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_inf());
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(unfl)
template <class V> void check_unfl(const V &v1, const V &v2, bool enabled) {
    set_fpu_reg(1, v1);
    set_fpu_reg(2, v2);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x23);

    if(enabled) {
        exception_check(51, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
}
BOOST_AUTO_TEST_SUITE(ext)
BOOST_DATA_TEST_CASE(rn, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_EXT;
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    xval xm1{sign < 0, 1, -0x3fff};
    xval xm2{false, 1, -0x3fff};
    check_unfl(xm1, xm2, enabled);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == (sign < 0));
}

BOOST_DATA_TEST_CASE(rz, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_EXT;
    mpfr_set_default_rounding_mode(MPFR_RNDZ);
    xval xm1{sign < 0, 1, -0x3fff};
    xval xm2{false, 1, -0x3fff};
    check_unfl(xm1, xm2, enabled);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == (sign < 0));
}

BOOST_DATA_TEST_CASE(rm, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_EXT;
    mpfr_set_default_rounding_mode(MPFR_RNDD);
    xval xm1{sign < 0, 1, -0x3fff};
    xval xm2{false, 1, -0x3fff};
    check_unfl(xm1, xm2, enabled);
    if(sign > 0) {
        BOOST_TEST(fpu.fp[2].is_zero());
        BOOST_TEST(!fpu.fp[2].signbit());
    } else {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(sg);
        BOOST_TEST(exp == 0);
        BOOST_TEST(fra == 1);
    }
}

BOOST_DATA_TEST_CASE(rp, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_EXT;
    mpfr_set_default_rounding_mode(MPFR_RNDU);
    xval xm1{sign < 0, 1, -0x3fff};
    xval xm2{false, 1, -0x3fff};
    check_unfl(xm1, xm2, enabled);
    if(sign < 0) {
        BOOST_TEST(fpu.fp[2].is_zero());
        BOOST_TEST(fpu.fp[2].signbit());
    } else {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(!sg);
        BOOST_TEST(exp == 0);
        BOOST_TEST(fra == 1);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(dbl)
BOOST_DATA_TEST_CASE(rn, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_DBL;
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    xval xm1{sign < 0, 1, -0x3fe};
    xval xm2{false, 1, -0x3fe};
    check_unfl(xm1, xm2, enabled);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == (sign < 0));
}

BOOST_DATA_TEST_CASE(rz, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_DBL;
    mpfr_set_default_rounding_mode(MPFR_RNDZ);
    xval xm1{sign < 0, 1, -0x3fe};
    xval xm2{false, 1, -0x3fe};
    check_unfl(xm1, xm2, enabled);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == (sign < 0));
}

BOOST_DATA_TEST_CASE(rm, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_DBL;
    mpfr_set_default_rounding_mode(MPFR_RNDD);
    xval xm1{sign < 0, 1, -0x3fe};
    xval xm2{false, 1, -0x3fe};
    check_unfl(xm1, xm2, enabled);
    if(sign > 0) {
        BOOST_TEST(!fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_zero());
    } else {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(sg);
        BOOST_TEST(exp == 0x3ffe - 1023 - 51);
        BOOST_TEST(fra == 0x8000000000000000LLU);
    }
}

BOOST_DATA_TEST_CASE(rp, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_DBL;
    mpfr_set_default_rounding_mode(MPFR_RNDU);
    xval xm1{sign < 0, 1, -0x3fe};
    xval xm2{false, 1, -0x3fe};
    check_unfl(xm1, xm2, enabled);
    if(sign < 0) {
        BOOST_TEST(fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_zero());
    } else {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(!sg);
        BOOST_TEST(exp == 0x3ffe - 1023 - 51);
        BOOST_TEST(fra == 0x8000000000000000LLU);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(sgl)
BOOST_DATA_TEST_CASE(rn, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_SGL;
    mpfr_set_default_rounding_mode(MPFR_RNDN);
    xval xm1{sign < 0, 1, -0x7e};
    xval xm2{false, 1, -0x7e};
    check_unfl(xm1, xm2, enabled);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == (sign < 0));
}

BOOST_DATA_TEST_CASE(rz, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_SGL;
    mpfr_set_default_rounding_mode(MPFR_RNDZ);
    xval xm1{sign < 0, 1, -0x7e};
    xval xm2{false, 1, -0x7e};
    check_unfl(xm1, xm2, enabled);
    BOOST_TEST(fpu.fp[2].is_zero());
    BOOST_TEST(fpu.fp[2].signbit() == (sign < 0));
}

BOOST_DATA_TEST_CASE(rm, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_SGL;
    mpfr_set_default_rounding_mode(MPFR_RNDD);
    xval xm1{sign < 0, 1, -0x7e};
    xval xm2{false, 1, -0x7e};
    check_unfl(xm1, xm2, enabled);
    if(sign > 0) {
        BOOST_TEST(!fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_zero());
    } else {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(sg);
        BOOST_TEST(exp == 0x3ffe - 0x7f - 22);
        BOOST_TEST(fra == 0x8000000000000000LLU);
    }
}

BOOST_DATA_TEST_CASE(rp, BIT *SIGN, enabled, sign) {
    fpu.FPCR.unfl = enabled;
    fpu.FPCR.prec = RND_PREC::ROUND_SGL;
    mpfr_set_default_rounding_mode(MPFR_RNDU);
    xval xm1{sign < 0, 1, -0x7e};
    xval xm2{false, 1, -0x7e};
    check_unfl(xm1, xm2, enabled);
    if(sign < 0) {
        BOOST_TEST(fpu.fp[2].signbit());
        BOOST_TEST(fpu.fp[2].is_zero());
    } else {
        auto [sg, fra, exp] = fpu.fp[2].get_zexp();
        BOOST_TEST(!sg);
        BOOST_TEST(exp == 0x3ffe - 0x7f - 22);
        BOOST_TEST(fra == 0x8000000000000000LLU);
    }
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_DATA_TEST_CASE(div0, BIT, enabled) {
    fpu.FPCR.dz = enabled;
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 0.0);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x20);

    if(enabled) {
        exception_check(50, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
        BOOST_TEST(fpu.fp[2].is_zero());
    } else {
        exception_check(0);
        BOOST_TEST(fpu.fp[2].is_inf());
    }
}

BOOST_DATA_TEST_CASE(inex1, BIT, enabled) {
    fpu.FPCR.inex1 = enabled;
    raw_write32(0x10, 0x40010001 );
    raw_write32(0x14, 0x00000000);
    raw_write32(0x18, 0);
    regs.a[1] = 0x10;
      raw_write16(0, 0171031);
    raw_write16(2, 1 << 14 | 3 << 10 | 2 << 7);
    if(enabled) {
        exception_check(49, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
    fpu.FPCR.inex1 = false;
}
BOOST_DATA_TEST_CASE(inex2, BIT, enabled) {
    fpu.FPCR.inex2 = enabled;
    set_fpu_reg(1, 1.0);
    set_fpu_reg(2, 3.0);
    raw_write16(0, 0171000);
    raw_write16(2, 1 << 10 | 2 << 7 | 0x20);

    if(enabled) {
        exception_check(49, 0);
        BOOST_TEST(raw_read32(regs.a[7] + 2) == 0);
    } else {
        exception_check(0);
    }
}

BOOST_AUTO_TEST_SUITE_END()