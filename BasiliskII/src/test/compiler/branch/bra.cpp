#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(BRA, InitFix)
BOOST_AUTO_TEST_CASE(Byte) {
    raw_write16(0, 0x6010);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(Word) {
    raw_write16(0, 0x6000);
    raw_write16(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x1002);
}

BOOST_AUTO_TEST_CASE(Long) {
    raw_write16(0, 0x60ff);
    raw_write32(2, 0x100000);
    regs.z = true;
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x100002);
}

BOOST_AUTO_TEST_SUITE_END()