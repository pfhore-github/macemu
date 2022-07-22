#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(ILLEGAL)

BOOST_AUTO_TEST_CASE(run) {
    raw_write16(0, 0045374);
    jit_compile(0, 2);
    jit_exception_check(4);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()