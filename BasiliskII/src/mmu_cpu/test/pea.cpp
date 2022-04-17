#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( PEA, InitFix )
{    
    regs.a[7] = 0x104;
    regs.a[2] = 0x10;
    asm_m68k("pea (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x10 );
    BOOST_TEST( regs.a[7] == 0x100 );
}
