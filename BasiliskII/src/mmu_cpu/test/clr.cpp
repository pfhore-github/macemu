#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( Byte, InitFix  )
{    
    regs.d[1] = 44;
    regs.x = regs.c = regs.v = regs.n = true;
    asm_m68k("clrb %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.x );
    BOOST_TEST( ! regs.v );
    BOOST_TEST( ! regs.n );
    BOOST_TEST( regs.z );
}

BOOST_FIXTURE_TEST_CASE( Word, InitFix  )
{    
    regs.d[1] = 4400;
    regs.x = regs.c = regs.v = regs.n = true;
    asm_m68k("clrw %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.x );
    BOOST_TEST( ! regs.v );
    BOOST_TEST( ! regs.n );
    BOOST_TEST( regs.z );
}

BOOST_FIXTURE_TEST_CASE( Long, InitFix  )
{    
    regs.d[1] = 4400000;
    regs.x = regs.c = regs.v = regs.n = true;
    asm_m68k("clrl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.x );
    BOOST_TEST( ! regs.v );
    BOOST_TEST( ! regs.n );
    BOOST_TEST( regs.z );
}