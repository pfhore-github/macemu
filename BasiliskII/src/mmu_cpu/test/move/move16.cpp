#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(MOVE16, InitFix)
BOOST_AUTO_TEST_CASE(incr_to_imm) {
    auto src = rand_ar();
    regs.a[src] = 0x100;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x100 + i, 0x20 + i);
    }
    raw_write16(0, 0173000 | src);
    raw_write32(2, 0x200);
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x200 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[src] == 0x110);
}

BOOST_AUTO_TEST_CASE(imm_to_incr) {
    auto dst = rand_ar();
    regs.a[dst] = 0x100;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x200 + i, 0x20 + i);
    }
    raw_write16(0, 0173010 | dst);
    raw_write32(2, 0x200);
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x100 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[dst] == 0x110);
}

BOOST_AUTO_TEST_CASE(reg_to_imm) {
    auto src = rand_ar();
    regs.a[src] = 0x100;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x100 + i, 0x20 + i);
    }
    raw_write16(0, 0173020 | src);
    raw_write32(2, 0x200);
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x200 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[src] == 0x100);
}

BOOST_AUTO_TEST_CASE(imm_to_reg) {
    auto dst = rand_ar();
    regs.a[dst] = 0x100;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x200 + i, 0x20 + i);
    }
    raw_write16(0, 0173030 | dst);
    raw_write32(2, 0x200);
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x100 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[dst] == 0x100);
}

BOOST_AUTO_TEST_CASE(incr_to_incr) {
    regs.a[2] = 0x100;
    regs.a[3] = 0x200;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x100 + i, 0x20 + i);
    }
    raw_write16(0, 0173040 | 2);
    raw_write16(2, 0x8000 | 3 << 12);
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x200 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[2] == 0x110);
    BOOST_TEST(regs.a[3] == 0x210);
}

BOOST_AUTO_TEST_SUITE_END()