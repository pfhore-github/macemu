#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(TRAPV)
BOOST_AUTO_TEST_CASE(success) {
    regs.v = false;
    raw_write16(0, 0047166);
    jit_compile(0, 2);
    jit_exception_check(0);
}

BOOST_AUTO_TEST_CASE(failure) {
    regs.v = true;
    raw_write16(0, 0047166);
    jit_compile(0, 2);
    jit_exception_check(7);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()