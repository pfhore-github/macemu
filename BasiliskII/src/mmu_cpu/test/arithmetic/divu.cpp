#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE( DIVU , InitFix)
BOOST_AUTO_TEST_SUITE( Long )
BOOST_AUTO_TEST_CASE( in_range ) {
    regs.d[1] = 2;
    regs.d[2] = 10000000;
    asm_m68k("divu.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 5000000);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( ! regs.z);
}

BOOST_AUTO_TEST_CASE( zero ) {
    regs.d[1] = 12345;
    regs.d[2] = 0;
    asm_m68k("divu.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( regs.z);
}

BOOST_AUTO_TEST_CASE( overflow ) {
    regs.d[1] = 2;
    regs.d[2] = 2;
    regs.d[3] = 0;
    asm_m68k("divu.l %D1, %D2:%D3");
    m68k_do_execute();
    BOOST_TEST( regs.v);
}

BOOST_AUTO_TEST_CASE( div0 )
{    
    regs.vbr = 0x100;
    regs.d[2] = 1;
    regs.d[1] = 0;
    
    regs.a[1] = 0x10;
    asm_m68k("divu.l %D1, %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    raw_write32(0x100+5*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_CASE( mod ) {
    regs.d[1] = 3;
    regs.d[2] = 10000;
    regs.d[3] = 0;
    asm_m68k("divu.l %D1, %D3:%D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[3] == 1);
    BOOST_TEST( regs.d[2] == 3333);
}
BOOST_AUTO_TEST_CASE( divul ) {
    regs.d[1] = 3;
    regs.d[2] = 10000;
    regs.d[3] = 1;
    asm_m68k("divul.l %D1, %D3:%D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[3] == 1);
    BOOST_TEST( regs.d[2] == 3333);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Word )
BOOST_AUTO_TEST_CASE( in_range ) {
    regs.d[1] = 3;
    regs.d[2] = 1000;
    asm_m68k("divu.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == (333 | 1 << 16) );
    BOOST_TEST( ! regs.v);
    BOOST_TEST( ! regs.z);
}

BOOST_AUTO_TEST_CASE( zero ) {
    regs.d[1] = 100;
    regs.d[2] = 0;
    asm_m68k("divu.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0);
    BOOST_TEST( ! regs.v);
    BOOST_TEST( regs.z);
}

BOOST_AUTO_TEST_CASE( overflow ) {
    regs.d[1] = 2;
    regs.d[2] = 1000000;
    asm_m68k("divu.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.v);
}

BOOST_AUTO_TEST_CASE( div0 )
{    
    regs.vbr = 0x100;
    regs.d[2] = 1;
    regs.d[1] = 0;
    
    regs.a[1] = 0x10;
    asm_m68k("divu.w %D1, %D2");
    raw_write32(0x100+5*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
