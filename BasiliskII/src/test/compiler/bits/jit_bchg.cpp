#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(BCHG)
BOOST_AUTO_TEST_SUITE(Imm)
BOOST_AUTO_TEST_CASE(Byte) {
    regs.a[2] = 0x10;
    raw_write8(0x10, 0xfb);
    raw_write16(0, 0004122);
    raw_write16(2, 2);
     jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z );
    BOOST_TEST(raw_read8(0x10) == 0xff);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[2] = 0xfffeffff;
    raw_write16(0, 0004100 | 2);
    raw_write16(2, 16);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(regs.z );
    BOOST_TEST(regs.d[2] == 0xffffffff);
}

BOOST_AUTO_TEST_SUITE_END()
#if 0
BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(Byte) {
    regs.a[4] = 0x10;
    regs.d[3] = 2;
    raw_write16(0, 0003524 );
    raw_write8(0x10, 0xfb);
    m68k_do_execute();
    BOOST_TEST(regs.z );
    BOOST_TEST(raw_read8(0x10) ==0xff);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[2] = 0xfffeffff;
    regs.d[3] = 16;
    raw_write16(0, 0003502 );
    m68k_do_execute();
    BOOST_TEST(regs.z );
    BOOST_TEST(regs.d[2] == 0xffffffff);
}
BOOST_AUTO_TEST_SUITE_END()
#endif
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
