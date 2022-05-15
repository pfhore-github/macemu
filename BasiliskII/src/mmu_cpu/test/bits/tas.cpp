#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(TAS, InitFix)
BOOST_AUTO_TEST_CASE(Byte) {
    auto ea = rand_reg();
    uint8_t v = get_v8();
    regs.d[ea] = v;
    raw_write16(0, 0045300 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] & 0x80);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x80;
    raw_write16(0, 0045301);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0x00;
    raw_write16(0, 0045301);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()