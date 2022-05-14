#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(MOVEP, InitFix)
BOOST_AUTO_TEST_CASE(word_from) {
    auto xr = rand_reg();
    auto yr = rand_ar();
    uint16_t disp = get_v16() & 0x7fff;
    regs.a[yr] = 0x1000;
    raw_write8(0x1000 + disp, 0x12);
    raw_write8(0x1000 + disp + 1, 0x13);
    raw_write8(0x1000 + disp + 2, 0x14);
    raw_write8(0x1000 + disp + 3, 0x15);
    raw_write16(0, 0000410 | xr << 9 | yr);
    raw_write16(2, disp);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == 0x1214);
}

BOOST_AUTO_TEST_CASE(word_to) {
    auto xr = rand_reg();
    auto yr = rand_ar();
    uint16_t disp = get_v16() & 0x7fff;

    regs.a[yr] = 0x1000;
    regs.d[xr] = 0x1234;
    raw_write16(0, 0000610 | xr << 9 | yr);
    raw_write16(2, disp);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x1000 + disp) == 0x12);
    BOOST_TEST(raw_read8(0x1000 + disp + 2) == 0x34);
}

BOOST_AUTO_TEST_CASE(long_from) {
    auto xr = rand_reg();
    auto yr = rand_ar();
    uint16_t disp = get_v16() & 0x7fff;
    regs.a[yr] = 0x1000;
    raw_write8(0x1000 + disp + 0, 0x12);
    raw_write8(0x1000 + disp + 1, 0x13);
    raw_write8(0x1000 + disp + 2, 0x14);
    raw_write8(0x1000 + disp + 3, 0x15);
    raw_write8(0x1000 + disp + 4, 0x16);
    raw_write8(0x1000 + disp + 5, 0x17);
    raw_write8(0x1000 + disp + 6, 0x18);
    raw_write8(0x1000 + disp + 7, 0x19);
    raw_write16(0, 0000510 | xr << 9 | yr);
    raw_write16(2, disp);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == 0x12141618);
}

BOOST_AUTO_TEST_CASE(long_to) {
    auto xr = rand_reg();
    auto yr = rand_ar();
    uint16_t disp = get_v16() & 0x7fff;

    regs.a[yr] = 0x1000;
    regs.d[xr] = 0x12345678;
    raw_write16(0, 0000710 | xr << 9 | yr);
    raw_write16(2, disp);
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x1000 + disp + 0) == 0x12);
    BOOST_TEST(raw_read8(0x1000 + disp + 2) == 0x34);
    BOOST_TEST(raw_read8(0x1000 + disp + 4) == 0x56);
    BOOST_TEST(raw_read8(0x1000 + disp + 6) == 0x78);
}
BOOST_AUTO_TEST_SUITE_END()