#define BOOST_TEST_DYN_LINK
#include "newcpu.h"
#include "memory.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(UNLK, InitFix)
BOOST_AUTO_TEST_CASE( run )
{    
    auto an = rand_ar();
    auto addr = get_v32();
    regs.a[an] = 0x200;
    raw_write32( 0x200, addr );
    raw_write16( 0, 0047130 | an );
    m68k_do_execute();
    BOOST_TEST( regs.a[an] == addr );
    BOOST_TEST( regs.a[7] == 0x204 );
}
BOOST_AUTO_TEST_SUITE_END()