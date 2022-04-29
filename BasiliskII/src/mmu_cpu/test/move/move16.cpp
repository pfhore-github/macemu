#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(MOVE16, InitFix)
BOOST_AUTO_TEST_CASE(incr_to_imm) {
    regs.a[2] = 0x100;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x100 + i, 0x20 + i);
    }
    asm_m68k("move16 (%A2)+, (0x200).L ");
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x200 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[2] == 0x110);
}

BOOST_AUTO_TEST_CASE(imm_to_incr) {
    regs.a[2] = 0x100;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x200 + i, 0x20 + i);
    }
    asm_m68k("move16 (0x200).L, (%A2)+");
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x100 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[2] == 0x110);
}

BOOST_AUTO_TEST_CASE(reg_to_imm) {
    regs.a[2] = 0x100;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x100 + i, 0x20 + i);
    }
    asm_m68k("move16 (%A2), (0x200).L ");
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x200 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(imm_to_reg) {
    regs.a[2] = 0x100;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x200 + i, 0x20 + i);
    }
    asm_m68k("move16 (0x200).L, (%A2)");
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x100 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[2] == 0x100);
}

BOOST_AUTO_TEST_CASE(incr_to_incr) {
    regs.a[2] = 0x100;
    regs.a[3] = 0x200;
    for(int i = 0; i < 16; ++i) {
        raw_write8(0x100 + i, 0x20 + i);
    }
    asm_m68k("move16 (%A2)+, (%A3)+ ");
    m68k_do_execute();
    for(int i = 0; i < 16; ++i) {
        BOOST_TEST(raw_read8(0x200 + i) == 0x20 + i);
    }

    BOOST_TEST(regs.a[2] == 0x110);
    BOOST_TEST(regs.a[3] == 0x210);
}

BOOST_AUTO_TEST_SUITE_END()