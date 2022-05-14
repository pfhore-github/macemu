#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(NOT, InitFix)

BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[1] = 44;
    raw_write16(0, 0043001);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (uint8_t)(~44));
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[1] = 440;
    raw_write16(0, 0043101);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (uint16_t)(~440));
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[1] = 440000;
    raw_write16(0, 0043201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == ~440000);
}

BOOST_AUTO_TEST_SUITE_END()