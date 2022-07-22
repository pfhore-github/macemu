#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(BTST)
BOOST_AUTO_TEST_SUITE(Imm)
BOOST_DATA_TEST_CASE(Byte, BIT, z) {
    regs.a[2] = 0x10;
    raw_write8(0x10, (!z) << 4);
    raw_write16(0, 0004022);
    raw_write16(2, 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z == z);
}

BOOST_DATA_TEST_CASE(Long, BIT, z) {
    regs.d[2] = (!z) << 16;
    raw_write16(0, 0004002);
    raw_write16(2, 16);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z == z);
}

BOOST_AUTO_TEST_SUITE_END()
#if 0
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_DATA_TEST_CASE(Byte, BIT, z) {
    regs.a[2] = 0x10;
    regs.d[3] = 4;
    raw_write16(0, 0003422);
    raw_write8(0x10, (!z) << 4);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z == z);
}

BOOST_DATA_TEST_CASE(Long, BIT, z) {
    regs.d[2] = (!z) << 16;
    regs.d[3] = 16;
    raw_write16(0, 0003402);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z == z);
}
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()