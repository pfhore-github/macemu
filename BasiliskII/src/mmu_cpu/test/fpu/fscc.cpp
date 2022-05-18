#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(FSCC, InitFix)

BOOST_DATA_TEST_CASE(eq, BIT, z) {
    auto reg = rand_reg();
    regs.fpu.FPSR.z = z;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == 0xff * z);
}

BOOST_DATA_TEST_CASE(ogt, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 2);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((!(nan | z | n)) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(oge, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 3);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == (((z | !(nan | n))) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(olt, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 4);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == (((n & !(nan | z))) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(ole, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 5);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == (((z | (n & !nan))) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(ogl, BIT *BIT, nan, z) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 6);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((!(nan | z)) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(or_, BIT, nan) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 7);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == (((!nan)) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(un, BIT, nan) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 8);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == (((nan)) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(ueq, BIT *BIT, nan, z) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | z))) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(ugt, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 10);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | !(n | z)))) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(uge, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 11);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | !n | z))) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(ult, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 12);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | (n & !z)))) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(ule, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 13);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | n | z))) ? 0xff : 0));
}

BOOST_DATA_TEST_CASE(ne, BIT, z) {
    auto reg = rand_reg();
    regs.fpu.FPSR.z = z;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 14);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == (((!z)) ? 0xff : 0));
}

BOOST_AUTO_TEST_CASE(t) {
    auto reg = rand_reg();
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 15);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == 0xff);
}

BOOST_AUTO_TEST_CASE(sf) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = true;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 16);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == 0);
    BOOST_TEST(regs.fpu.FPSR.bsun);
}
BOOST_DATA_TEST_CASE(seq, BIT, z) {
    auto reg = rand_reg();
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.bsun = false;
    regs.fpu.FPSR.nan = true;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 17);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((z) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun);
}

BOOST_DATA_TEST_CASE(gt, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 18);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((!(nan | z | n)))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(ge, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 19);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((z | !(nan | n)))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(lt, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 20);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((n & !(nan | z)))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(le, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 21);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((z | (n & !nan)))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(gl, BIT *BIT, nan, z) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 22);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == (((!(nan | z))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(gle, BIT, nan) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 23);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((!nan))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(ngle, BIT, nan) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 24);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(ngl, BIT *BIT, nan, z) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 25);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | z))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(nle, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 26);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | !(n | z)))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(nlt, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 27);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | !n | z))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(nge, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 28);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | (n & !z)))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(ngt, BIT *BIT *BIT, nan, z, n) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 29);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == ((((nan | n | z))) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(sne, BIT *BIT, nan, z) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 30);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == (((!z)) ? 0xff : 0));
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE(st, BIT, nan) {
    auto reg = rand_reg();
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171100 | reg);
    raw_write16(2, 31);
    m68k_do_execute();
    BOOST_TEST(regs.d[reg] == 0xff);
    BOOST_TEST(regs.fpu.FPSR.bsun == nan);
}

BOOST_AUTO_TEST_SUITE_END()