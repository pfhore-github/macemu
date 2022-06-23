#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FNOP, InitFix)
BOOST_AUTO_TEST_CASE(run) {
    raw_write16(0, 0171200);
    raw_write16(2, 0);
    exception_check(0);
}
BOOST_AUTO_TEST_SUITE_END()