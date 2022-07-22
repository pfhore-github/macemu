#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"


BOOST_FIXTURE_TEST_SUITE(ADDA, InitFix)

BOOST_AUTO_TEST_CASE(Word) {
    regs.a[3] = 0xa0000000;
    regs.d[2] = 0xc000;
    raw_write16(0, 0153302);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x9fffc000);
}

BOOST_AUTO_TEST_CASE(Long) {
    regs.a[3] = 0x10000000;
    regs.d[2] = 0x20000000;
    raw_write16(0, 0153702);
    m68k_do_execute();
    BOOST_TEST(regs.a[3] == 0x30000000);
}

BOOST_AUTO_TEST_SUITE_END()

