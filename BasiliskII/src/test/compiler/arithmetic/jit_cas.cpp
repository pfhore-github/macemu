#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(JIT, InitFix)
BOOST_AUTO_TEST_SUITE(CAS)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(eq) {
    regs.d[4] = 2;
    regs.d[3] = 1;
    regs.a[2] = 0x10;
    raw_write16(0, 0005322);
    raw_write16(2, 4 << 6 | 3);
    raw_write8(0x10, 1);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 2);
    BOOST_TEST(regs.d[3] == 1);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ne) {
    regs.d[4] = 5;
    regs.d[3] = 11;
    regs.a[2] = 0x10;
    raw_write8(0x10, 1);
    raw_write16(0, 0005322);
    raw_write16(2, 4 << 6 | 3);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read8(0x10) == 1);
    BOOST_TEST(regs.d[3] == 1);
    BOOST_TEST(!regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(eq) {
    regs.d[4] = 1000;
    regs.d[3] = 500;
    regs.a[2] = 0x10;
    raw_write16(0, 0006322);
    raw_write16(2, 4 << 6 | 3);
    raw_write16(0x10, 500);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read16(0x10) == 1000);
    BOOST_TEST(regs.d[3] == 500);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ne) {
    regs.d[4] = 500;
    regs.d[3] = 600;
    regs.a[2] = 0x10;
    raw_write16(0x10, 700);
    raw_write16(0, 0006322);
    raw_write16(2, 4 << 6 | 3);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read16(0x10) == 700);
    BOOST_TEST(regs.d[3] == 700);
    BOOST_TEST(!regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(eq) {
    regs.d[4] = 20000;
    regs.d[3] = 10000;
    regs.a[2] = 0x10;
    raw_write16(0, 0007322);
    raw_write16(2, 4 << 6 | 3);
    raw_write32(0x10, 10000);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read32(0x10) == 20000);
    BOOST_TEST(regs.d[3] == 10000);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(ne) {
    regs.d[4] = 50000;
    regs.d[3] = 60000;
    regs.a[2] = 0x10;
    raw_write32(0x10, 70000);
    raw_write16(0, 0007322);
    raw_write16(2, 4 << 6 | 3);
    jit_compile(0, 2);
    jit_jump(0);
    BOOST_TEST(raw_read32(0x10) == 70000);
    BOOST_TEST(regs.d[3] == 70000);
    BOOST_TEST(!regs.z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()