#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(FBCC, InitFix)
BOOST_AUTO_TEST_CASE(Word) {
    regs.fpu.FPSR.z = true;
    raw_write16(0, 0171201);
    raw_write16(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x1002);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.fpu.FPSR.z = true;
    raw_write16(0, 0171301);
    raw_write32(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x1002);
}
BOOST_DATA_TEST_CASE(eq, BIT, z) {
    regs.fpu.FPSR.z = z;
    raw_write16(0, 0171200 | 1);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (z ? 0x102 : 4));
}

BOOST_DATA_TEST_CASE(ogt, BIT *BIT *BIT, nan, z, n) {
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171200 | 2);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (!(nan | z | n) ? 0x102 : 4));
}

BOOST_DATA_TEST_CASE(oge, BIT *BIT *BIT, nan, z, n) {
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171200 | 3);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == ((z | !(nan | n)) ? 0x102 : 4));
}

BOOST_DATA_TEST_CASE(olt, BIT *BIT *BIT, nan, z, n) {
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171200 | 4);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == ((n & !(nan | z)) ? 0x102 : 4));
}

BOOST_DATA_TEST_CASE(ole, BIT *BIT *BIT, nan, z, n) {
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171200 | 5);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == ((z | (n & !nan)) ? 0x102 : 4));
}

BOOST_DATA_TEST_CASE(ogl, BIT *BIT, nan, z) {
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    raw_write16(0, 0171200 | 6);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (!(nan | z) ? 0x102 : 4));
}

BOOST_DATA_TEST_CASE(or_, BIT, nan) {
    regs.fpu.FPSR.nan = nan;
    raw_write16(0, 0171200 | 7);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == ((!nan) ? 0x102 : 4));
}

BOOST_DATA_TEST_CASE(un, BIT, nan) {
    regs.fpu.FPSR.nan = nan;
    raw_write16(0, 0171200 | 8);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == ((nan) ? 0x102 : 4));
}

BOOST_DATA_TEST_CASE( ueq, BIT * BIT, nan, z )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    raw_write16(0, 0171200 | 9);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|z)) ? 0x102 : 4));;
}



BOOST_DATA_TEST_CASE( ugt, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171200 | 10);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|!(n|z))) ? 0x102 : 4));;
}


BOOST_DATA_TEST_CASE( uge, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171200 | 11);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|!n|z)) ? 0x102 : 4));;
}


BOOST_DATA_TEST_CASE( ult, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171200 | 12);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|(n&!z))) ? 0x102 : 4));;
}


BOOST_DATA_TEST_CASE( ule, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    raw_write16(0, 0171200 | 13);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|n|z)) ? 0x102 : 4));;
}

BOOST_DATA_TEST_CASE( ne, BIT , z )
{    
    regs.fpu.FPSR.z = z;
    raw_write16(0, 0171200 | 14);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == ((!z) ? 0x102 : 4));;
}

BOOST_AUTO_TEST_CASE( t )
{    
    raw_write16(0, 0171200 | 15);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x102);
}

BOOST_AUTO_TEST_CASE( sf )
{    
    regs.fpu.FPSR.nan = true;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 16);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 4);
    BOOST_TEST( regs.fpu.FPSR.bsun);
}
BOOST_DATA_TEST_CASE( seq, BIT, z )
{    
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.bsun = false;
    regs.fpu.FPSR.nan = true;
    raw_write16(0, 0171200 | 17);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (z ? 0x102 : 4));
    BOOST_TEST( regs.fpu.FPSR.bsun);
}

BOOST_DATA_TEST_CASE( gt, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 18);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((!(nan|z|n))) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( ge, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 19);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((z|!(nan|n))) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( lt, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 20);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((n&!(nan|z))) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( le, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 21);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((z|(n&!nan))) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( gl, BIT * BIT, nan, z )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 22);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == ((!(nan|z)) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( gle, BIT, nan)
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 23);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((!nan)) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( ngle, BIT, nan)
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 24);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan)) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( ngl, BIT * BIT , nan, z )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 25);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|z)) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( nle, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 26);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|!(n|z))) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( nlt, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 27);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|!n|z)) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( nge, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 28);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|(n&!z))) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( ngt, BIT * BIT * BIT, nan, z, n )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.n = n;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 29);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == (((nan|n|z)) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( sne, BIT*BIT, nan, z )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.z = z;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 30);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST(regs.pc == ((!z) ? 0x102 : 4));;
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( st, BIT, nan )
{    
    regs.fpu.FPSR.nan = nan;
    regs.fpu.FPSR.bsun = false;
    raw_write16(0, 0171200 | 31);
    raw_write16(2, 0x100);
    m68k_do_execute();
    BOOST_TEST( regs.pc == 0x102 );
    BOOST_TEST( regs.fpu.FPSR.bsun == nan);
}

BOOST_AUTO_TEST_SUITE_END()