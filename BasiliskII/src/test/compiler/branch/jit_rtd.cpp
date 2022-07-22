#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(RTD)
BOOST_AUTO_TEST_CASE(run) {
    regs.a[7] = 0x200;
    raw_write16(0, 0047164);
    raw_write16(2, -100);
    raw_write32(0x200, 0x1000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.a[7] == 0x200 -100 + 4);
    BOOST_TEST(regs.pc == 0x1000);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()