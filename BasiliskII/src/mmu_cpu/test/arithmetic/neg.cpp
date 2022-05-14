#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(NEG, InitFix)

BOOST_AUTO_TEST_CASE(Byte) {
    regs.d[1] = 44;
    raw_write16(0, 0042001);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 256 - 44);
}

BOOST_AUTO_TEST_CASE(Word) {
    regs.d[1] = 440;
    raw_write16(0, 0042101);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 65536 - 440);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.d[1] = 440000;
    raw_write16(0, 0042201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -440000);
}

BOOST_AUTO_TEST_SUITE_END()