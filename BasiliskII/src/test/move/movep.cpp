#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(MOVEP, InitFix)
BOOST_AUTO_TEST_CASE(word_from) {
    regs.a[2] = 0x1000;
    raw_write8(0x1000 + 100, 0x12);
    raw_write8(0x1000 + 100 + 1, 0x13);
    raw_write8(0x1000 + 100 + 2, 0x14);
    raw_write8(0x1000 + 100 + 3, 0x15);
    raw_write16(0, 0003412);
    raw_write16(2, 100);
    m68k_do_execute();
    BOOST_TEST(regs.d[3] == 0x1214);
}

BOOST_AUTO_TEST_CASE(word_to) {

    regs.a[3] = 0x1000;
    regs.d[2] = 0x1234;
    raw_write16(0, 0002613);
    raw_write16(2, 200);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x1000 + 200) == 0x12);
    BOOST_TEST(raw_read8(0x1000 + 200 + 2) == 0x34);
}

BOOST_AUTO_TEST_CASE(long_from) {
    regs.a[2] = 0x1000;
    raw_write8(0x1000 + 60 + 0, 0x12);
    raw_write8(0x1000 + 60 + 1, 0x13);
    raw_write8(0x1000 + 60 + 2, 0x14);
    raw_write8(0x1000 + 60 + 3, 0x15);
    raw_write8(0x1000 + 60 + 4, 0x16);
    raw_write8(0x1000 + 60 + 5, 0x17);
    raw_write8(0x1000 + 60 + 6, 0x18);
    raw_write8(0x1000 + 60 + 7, 0x19);
    raw_write16(0, 0004512 );
    raw_write16(2, 60);
    m68k_do_execute();
    BOOST_TEST(regs.d[4] == 0x12141618);
}

BOOST_AUTO_TEST_CASE(long_to) {

    regs.a[2] = 0x1000;
    regs.d[3] = 0x12345678;
    raw_write16(0, 0003712);
    raw_write16(2, 50);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x1000 + 50 + 0) == 0x12);
    BOOST_TEST(raw_read8(0x1000 + 50 + 2) == 0x34);
    BOOST_TEST(raw_read8(0x1000 + 50 + 4) == 0x56);
    BOOST_TEST(raw_read8(0x1000 + 50 + 6) == 0x78);
}
BOOST_AUTO_TEST_SUITE_END()