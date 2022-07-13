#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(BSET, InitFix)
BOOST_AUTO_TEST_SUITE(Imm)
BOOST_AUTO_TEST_CASE(Byte) {
    regs.a[2] = 0x10;
    raw_write8(0x10, 0);
    raw_write16(0, 0004322);
    raw_write16(2, 3);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(raw_read8(0x10) == 0x08);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[2] = 0;
    raw_write16(0, 0004302);
    raw_write16(2, 16);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[2] == 0x10000);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Reg)
BOOST_AUTO_TEST_CASE(Byte) {
    regs.a[2] = 0x10;
    regs.d[4] = 3;
    raw_write16(0, 004722);
    raw_write8(0x10, 0);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(raw_read8(0x10) == 0x08);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[2] = 0;
    regs.d[3] = 16;
    raw_write16(0, 0003702);
    m68k_do_execute();
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.d[2] == 0x10000);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
