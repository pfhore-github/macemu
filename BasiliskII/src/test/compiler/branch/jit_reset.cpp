#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
extern bool reset;
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(RESET)

BOOST_AUTO_TEST_CASE(unpriv) {
    regs.S = false;
    raw_write16(0, 0047160);
    jit_compile(0, 2);
    jit_exception_check(8);
}

BOOST_AUTO_TEST_CASE(run) {
    reset = true;
    raw_write16(0, 0047160);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(reset);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()