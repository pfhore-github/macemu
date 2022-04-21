#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( DR2DR, InitFix  )
{    
    regs.d[1] = 123456;
    regs.d[3] = 333333;
    asm_m68k("exg %D1, %D3");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 333333 );
    BOOST_TEST( regs.d[3] == 123456 );
}

BOOST_FIXTURE_TEST_CASE( AR2AR, InitFix  )
{    
    regs.a[1] = 123456;
    regs.a[3] = 333333;
    asm_m68k("exg %A1, %A3");
    m68k_do_execute();
    BOOST_TEST( regs.a[1] == 333333 );
    BOOST_TEST( regs.a[3] == 123456 );
}

BOOST_FIXTURE_TEST_CASE( AR2DR, InitFix  )
{    
    regs.d[1] = 123456;
    regs.a[3] = 333333;
    asm_m68k("exg %D1, %A3");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 333333 );
    BOOST_TEST( regs.a[3] == 123456 );
}
