#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE( EXTB, InitFix  )
BOOST_AUTO_TEST_CASE(  pos)
{    
    regs.d[1] = 44;
    asm_m68k("extb.l %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 44 );
    BOOST_TEST( ! regs.z );
    BOOST_TEST( ! regs.n );
}

BOOST_AUTO_TEST_CASE( zero )
{    
    regs.d[1] = 0;
    asm_m68k("extb.l %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.n );
}
BOOST_AUTO_TEST_CASE( neg )
{    
    regs.d[1] = -22 & 0xff;
    asm_m68k("extb.l %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == -22 );
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.n );
}
BOOST_AUTO_TEST_SUITE_END()