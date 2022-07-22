#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(JMP)
BOOST_AUTO_TEST_CASE(run) {
    regs.a[2] = 0x10;
    raw_write16(0, 0047322);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.pc == 0x10);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()