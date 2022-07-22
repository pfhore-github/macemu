#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(BCLR)
BOOST_AUTO_TEST_SUITE(Imm)
BOOST_AUTO_TEST_CASE(Byte) {
    regs.a[2] = 0x10;
    raw_write8(0x10, 4);
    raw_write16(0, 0004222);
    raw_write16(2, 2);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(raw_read8(0x10) == 0);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[2] = 0x10000;
    raw_write16(0, 0004202);
    raw_write16(2, 16);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()
#if 0
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(Byte) {
    regs.a[3] = 0x10;
    regs.d[5] = 2;
    raw_write16(0, 0005623);
    raw_write8(0x10, 4);
    m68k_do_execute();
    BOOST_TEST(!regs.z );
    BOOST_TEST(raw_read8(0x10) == 0);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[2] = 0x10000;
    regs.d[3] = 16;
    raw_write16(0, 0003602);
    m68k_do_execute();
    BOOST_TEST(!regs.z );
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
