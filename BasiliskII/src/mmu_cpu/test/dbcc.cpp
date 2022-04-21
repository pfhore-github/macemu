#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( t, InitFix  )
{    
    asm_m68k("dbt %D1, DONE\nnop\nDONE:\nnop");
    m68k_do_execute();
    BOOST_TEST( regs.pc == 4 );
}

BOOST_FIXTURE_TEST_CASE( f1, InitFix  )
{    
    regs.d[1] = 2;
    asm_m68k("dbf %D1, DONE\nnop\nDONE:\nnop");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 1 );
    BOOST_TEST( regs.pc == 6 );
}

BOOST_FIXTURE_TEST_CASE( f2, InitFix  )
{    
    regs.d[1] = 0;
    asm_m68k("dbf %D1, DONE\nnop\nDONE:\nnop");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xffff );
    BOOST_TEST( regs.pc == 4 );
}

