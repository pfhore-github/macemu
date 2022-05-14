#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(BKPT, InitFix)

BOOST_AUTO_TEST_CASE(run) {
    raw_write16(0, 0044110);
    exception_check(4);
}

BOOST_AUTO_TEST_SUITE_END()