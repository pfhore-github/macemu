#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE( Byte_to_word, InitFix  )
BOOST_AUTO_TEST_CASE(  pos)
{    
    regs.d[1] = 44;
    asm_m68k("ext.w %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 44 );
    BOOST_TEST( ! regs.z );
    BOOST_TEST( ! regs.n );
}

BOOST_AUTO_TEST_CASE( zero )
{    
    regs.d[1] = 0;
    asm_m68k("ext.w %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.n );
}
BOOST_AUTO_TEST_CASE( neg )
{    
    regs.d[1] = -22 & 0xff;
    asm_m68k("ext.w %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == ( -22 & 0xffff ) );
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.n );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( Word_to_Long, InitFix  )
BOOST_AUTO_TEST_CASE(  pos)
{    
    regs.d[1] = 4400;
    asm_m68k("ext.l %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 4400 );
    BOOST_TEST( ! regs.z );
    BOOST_TEST( ! regs.n );
}

BOOST_AUTO_TEST_CASE( zero )
{    
    regs.d[1] = 0;
    asm_m68k("ext.l %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.n );
}
BOOST_AUTO_TEST_CASE( neg )
{    
    regs.d[1] = -2222 & 0xffff;
    asm_m68k("ext.l %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == -2222 );
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.n );
}
BOOST_AUTO_TEST_SUITE_END()