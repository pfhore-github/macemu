#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_CASE( Word, InitFix)
{    
    regs.d[1] = 0x8040;
    asm_m68k("movea.w %D1, %A2");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0xffff8040 );
}


BOOST_FIXTURE_TEST_CASE( Long, InitFix)
{    
    regs.d[1] = 0x12345678;
    asm_m68k("movea.l %D1, %A2");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0x12345678 );
}
