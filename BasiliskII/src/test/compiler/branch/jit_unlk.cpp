#define BOOST_TEST_DYN_LINK
#include "newcpu.h"
#include "memory.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(UNLK)
BOOST_AUTO_TEST_CASE( run )
{    
    regs.a[2] = 0x200;
    raw_write32( 0x200, 0x1000 );
    raw_write16( 0, 0047132);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST( regs.a[2] == 0x1000 );
    BOOST_TEST( regs.a[7] == 0x204 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()