#define BOOST_TEST_DYN_LINK
#include "newcpu.h"
#include "memory.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(TRAP, InitFix)


BOOST_DATA_TEST_CASE( run , bdata::xrange(16), vn)
{    
    raw_write16(0, 0047100 | vn);
    exception_check(32+vn);
}

BOOST_AUTO_TEST_SUITE_END()