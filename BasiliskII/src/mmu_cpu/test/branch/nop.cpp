#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(NOP, InitFix)

BOOST_AUTO_TEST_CASE(run) {
    raw_write16(0, 0047161);
    exception_check(0);
}

BOOST_AUTO_TEST_SUITE_END()