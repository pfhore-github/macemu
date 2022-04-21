#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"


BOOST_FIXTURE_TEST_CASE( Unlk, InitFix )
{    
    regs.a[3] = 0x200;
    raw_write32( 0x200, 0x1000 );
    asm_m68k("unlk %A3");
    m68k_do_execute();
    BOOST_TEST( regs.a[3] == 0x1000 );
    BOOST_TEST( regs.a[7] == 0x204 );
}
