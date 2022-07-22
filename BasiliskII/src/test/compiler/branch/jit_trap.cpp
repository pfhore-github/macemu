#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(TRAP)

BOOST_DATA_TEST_CASE(run, bdata::xrange(16), vn) {
    raw_write16(0, 0047100 | vn);
    jit_compile(0, 2);
    jit_exception_check(32 + vn);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()