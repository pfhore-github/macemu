#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( Long, InitFix )
{    
    regs.a[7] = 0x204;
    regs.a[3] = 0x200000;
    asm_m68k("link.l %A3, #100000");
    m68k_do_execute();
    BOOST_TEST( raw_read32( 0x200) == 0x200000);
    BOOST_TEST( regs.a[3] == 0x200 );
    BOOST_TEST( regs.a[7] == 0x200 + 100000);
}

BOOST_FIXTURE_TEST_CASE( Word, InitFix )
{    
    regs.a[7] = 0x204;
    regs.a[3] = 0x2000;
    asm_m68k("link.w %A3, #1000");
    m68k_do_execute();
    BOOST_TEST( raw_read32( 0x200) == 0x2000);
    BOOST_TEST( regs.a[3] == 0x200 );
    BOOST_TEST( regs.a[7] == 0x200 + 1000);
}
