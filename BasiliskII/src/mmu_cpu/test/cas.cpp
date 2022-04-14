#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"


BOOST_AUTO_TEST_SUITE( Byte )
BOOST_AUTO_TEST_CASE( eq )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 20;
    regs.d[2] = 30;
    regs.a[1] = 0x10;
    raw_write8(0x10, 20);
    asm_m68k("casb %D1, %D2, (%A1)");    
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == 30 );
    BOOST_TEST( regs.d[2] == 30);
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE( ne )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 20;
    regs.d[2] = 30;
    regs.a[1] = 0x10;
    raw_write8(0x10, 25);
    asm_m68k("casb %D1, %D2, (%A1)");    
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == 25 );
    BOOST_TEST( regs.d[1] == 25);
    BOOST_TEST( ! regs.z );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Word )
BOOST_AUTO_TEST_CASE( eq )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 2000;
    regs.d[2] = 3000;
    regs.a[1] = 0x10;
    raw_write16(0x10, 2000);
    asm_m68k("casw %D1, %D2, (%A1)");    
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 3000 );
    BOOST_TEST( regs.d[2] == 3000);
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE( ne )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 2000;
    regs.d[2] = 3000;
    regs.a[1] = 0x10;
    raw_write16(0x10, 2500);
    asm_m68k("casw %D1, %D2, (%A1)");    
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 2500 );
    BOOST_TEST( regs.d[1] == 2500);
    BOOST_TEST( ! regs.z );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Long )
BOOST_AUTO_TEST_CASE( eq )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 200000;
    regs.d[2] = 300000;
    regs.a[1] = 0x10;
    raw_write32(0x10, 200000);
    asm_m68k("casl %D1, %D2, (%A1)");    
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == 300000 );
    BOOST_TEST( regs.d[2] == 300000);
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE( ne )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 200000;
    regs.d[2] = 300000;
    regs.a[1] = 0x10;
    raw_write32(0x10, 250000);
    asm_m68k("casl %D1, %D2, (%A1)");    
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == 250000 );
    BOOST_TEST( regs.d[1] == 250000);
    BOOST_TEST( ! regs.z );
}
BOOST_AUTO_TEST_SUITE_END()