#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"


BOOST_AUTO_TEST_SUITE( Word )
BOOST_AUTO_TEST_CASE( ne1 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 2000;
    regs.d[2] = 3000;
    regs.d[3] = 4000;
    regs.d[4] = 5000;
    regs.a[1] = 0x10;
    regs.a[2] = 0x12;
    raw_write16(0x10, 6000);
    raw_write16(0x12, 7000);
    asm_m68k("cas2w %D1:%D2, %D3:%D4, (%A1):(%A2)");    
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 6000 );
    BOOST_TEST( raw_read16(0x12) == 7000 );
    BOOST_TEST( regs.d[1] == 6000);
    BOOST_TEST( regs.d[2] == 7000);
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_CASE( ne2 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 2000;
    regs.d[2] = 3000;
    regs.d[3] = 4000;
    regs.d[4] = 5000;
    regs.a[1] = 0x10;
    regs.a[2] = 0x12;
    raw_write16(0x10, 2000);
    raw_write16(0x12, 7000);
    asm_m68k("cas2w %D1:%D2, %D3:%D4, (%A1):(%A2)");    
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 2000 );
    BOOST_TEST( raw_read16(0x12) == 7000 );
    BOOST_TEST( regs.d[1] == 2000);
    BOOST_TEST( regs.d[2] == 7000);
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_CASE( eq )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 2000;
    regs.d[2] = 3000;
    regs.d[3] = 4000;
    regs.d[4] = 5000;
    regs.a[1] = 0x10;
    regs.a[2] = 0x12;
    raw_write16(0x10, 2000);
    raw_write16(0x12, 3000);
    asm_m68k("cas2w %D1:%D2, %D3:%D4, (%A1):(%A2)");    
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 4000 );
    BOOST_TEST( raw_read16(0x12) == 5000 );
    BOOST_TEST( regs.d[1] == 2000);
    BOOST_TEST( regs.d[2] == 3000);
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( Long )
BOOST_AUTO_TEST_CASE( ne1 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 200000;
    regs.d[2] = 300000;
    regs.d[3] = 400000;
    regs.d[4] = 500000;
    regs.a[1] = 0x10;
    regs.a[2] = 0x14;
    raw_write32(0x10, 600000);
    raw_write32(0x14, 700000);
    asm_m68k("cas2l %D1:%D2, %D3:%D4, (%A1):(%A2)");    
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == 600000 );
    BOOST_TEST( raw_read32(0x14) == 700000 );
    BOOST_TEST( regs.d[1] == 600000);
    BOOST_TEST( regs.d[2] == 700000);
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_CASE( ne2 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 200000;
    regs.d[2] = 300000;
    regs.d[3] = 400000;
    regs.d[4] = 500000;
    regs.a[1] = 0x10;
    regs.a[2] = 0x14;
    raw_write32(0x10, 200000);
    raw_write32(0x14, 700000);
    asm_m68k("cas2l %D1:%D2, %D3:%D4, (%A1):(%A2)");    
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == 200000 );
    BOOST_TEST( raw_read32(0x14) == 700000 );
    BOOST_TEST( regs.d[1] == 200000);
    BOOST_TEST( regs.d[2] == 700000);
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_CASE( eq )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 200000;
    regs.d[2] = 300000;
    regs.d[3] = 400000;
    regs.d[4] = 500000;
    regs.a[1] = 0x10;
    regs.a[2] = 0x14;
    raw_write32(0x10, 200000);
    raw_write32(0x14, 300000);
    asm_m68k("cas2l %D1:%D2, %D3:%D4, (%A1):(%A2)");    
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == 400000 );
    BOOST_TEST( raw_read32(0x14) == 500000 );
    BOOST_TEST( regs.d[1] == 200000);
    BOOST_TEST( regs.d[2] == 300000);
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_SUITE_END()