#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"


BOOST_FIXTURE_TEST_SUITE( rte, InitFix )
BOOST_AUTO_TEST_CASE( format0 )
{    
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    PUSH16( 0 );
    PUSH32( 0x1000) ;
    PUSH16(1);
    asm_m68k("rte");
    m68k_do_execute();
    BOOST_TEST( regs.a[7] == 0x2000 );
    BOOST_TEST( ! regs.S );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.pc == 0x1000 );
}

BOOST_AUTO_TEST_CASE( format1 )
{    
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    PUSH16( 0 );
    PUSH32( 0x1200) ;
    PUSH16(0);
    PUSH16( 0x1000 );
    PUSH32( 0x1000) ;
    PUSH16(1 << 13);
    asm_m68k("rte");
    m68k_do_execute();
     BOOST_TEST( ! regs.S );
    BOOST_TEST( regs.a[7] == 0x2000 );
    BOOST_TEST( regs.pc == 0x1200 );
}

BOOST_AUTO_TEST_CASE( format2 )
{    
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    PUSH16( 0 );
    PUSH32( 0x1200) ;
    PUSH16(0);
    PUSH16( 0x1000 );
    PUSH32( 0x1000) ;
    PUSH16(1 << 13);
    asm_m68k("rte");
    m68k_do_execute();
    BOOST_TEST( ! regs.S );
    BOOST_TEST( regs.a[7] == 0x2000 );
    BOOST_TEST( regs.pc == 0x1200 );
}

BOOST_AUTO_TEST_CASE( format3 )
{    
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    PUSH32( 0xffff ) ;
    PUSH16( 0x3fff );
    PUSH32( 0x1200) ;
    PUSH16(0);
    asm_m68k("rte");
    m68k_do_execute();
    BOOST_TEST( ! regs.S );
    BOOST_TEST( regs.a[7] == 0x2000 );
    BOOST_TEST( regs.pc == 0x1200 );
}

BOOST_AUTO_TEST_CASE( format7_normal )
{    
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    regs.a[7] -= 2 * 23;
    PUSH16( 0);
    PUSH32( 0xffff ) ;
    PUSH16( 0x7fff );
    PUSH32( 0x1200) ;
    PUSH16(0);
    asm_m68k("rte");
    m68k_do_execute();
    BOOST_TEST( ! regs.S );
    BOOST_TEST( regs.a[7] == 0x2000 );
    BOOST_TEST( regs.pc == 0x1200 );
}

BOOST_AUTO_TEST_CASE( format7_movem)
{    
    regs.S = true;
    regs.usp = 0x2000;
    regs.a[7] = 0x200;
    regs.a[7] -= 2 * 23;
    PUSH16( 1 << 12);
    PUSH32( 0xffff ) ;
    PUSH16( 0x7fff );
    PUSH32( 0x1200) ;
    PUSH16(0);
    asm_m68k("rte");
    m68k_do_execute();
    BOOST_TEST( ! regs.S );
    BOOST_TEST( regs.i_eav == 0xffff );
    BOOST_TEST( regs.a[7] == 0x2000 );
    BOOST_TEST( regs.pc == 0x1200 );
}

BOOST_AUTO_TEST_CASE( formatx)
{    
    regs.S = true;
    regs.usp = 0x2000;
    regs.vbr = 0x3000;
    raw_write32( 0x3000 + 4 * 14, 0x4000);
    regs.a[7] = 0x200;
    PUSH16( 0x5fff );
    PUSH32( 0x1200) ;
    PUSH16(0);
    asm_m68k("rte");
    m68k_do_execute();
    BOOST_TEST( regs.S );
    BOOST_TEST( regs.pc == 0x4000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_CASE( rtd, InitFix )
{    
    regs.a[7] = 0x200;
    raw_write32( 0x200, 0x1000 );
    asm_m68k("rtd #20");
    m68k_do_execute();
    BOOST_TEST( regs.a[7] == 0x200 + 24 );
    BOOST_TEST( regs.pc == 0x1000 );
}

BOOST_FIXTURE_TEST_CASE( rts, InitFix )
{    
    regs.a[7] = 0x200;
    raw_write32( 0x200, 0x1000 );
    asm_m68k("rts");
    m68k_do_execute();
    BOOST_TEST( regs.pc == 0x1000 );
}

BOOST_FIXTURE_TEST_SUITE( trapv, InitFix )
BOOST_AUTO_TEST_CASE( success )
{
    regs.msp = regs.a[7] = 0x1000;
    regs.vbr = 0x1000;
    regs.S = false;
    raw_write32( 0x1000 + 7 * 4, 0x2000);
    regs.v = false;
    asm_m68k("trapv");
    m68k_do_execute();
    BOOST_TEST( ! regs.S );
    BOOST_TEST( regs.pc == 2);
}

BOOST_AUTO_TEST_CASE( failure )
{
    regs.msp = regs.a[7] = 0x1000;
    regs.vbr = 0x1000;
    regs.S = false;
    raw_write32( 0x1000 + 7 * 4, 0x2000);
    regs.v = true;
    asm_m68k("trapv");
    m68k_do_execute();
    BOOST_TEST( regs.S );
    BOOST_TEST( regs.pc == 0x2000);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_CASE( rtr, InitFix )
{    
    regs.a[7] = 0x200;
    raw_write16( 0x200, 0x1 );
    raw_write32( 0x202, 0x1000 );
    asm_m68k("rtr");
    m68k_do_execute();
    BOOST_TEST( regs.c == 1 );
    BOOST_TEST( regs.pc == 0x1000 );
}