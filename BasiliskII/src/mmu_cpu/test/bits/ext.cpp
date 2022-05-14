#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(EXT, InitFix)
BOOST_AUTO_TEST_SUITE(Byte_to_word)
BOOST_AUTO_TEST_CASE(positive) {
    auto dn = rand_reg();
    uint8_t v1 = (get_v8() & 0x7f) | 1;
    regs.d[dn] = v1;
    raw_write16(0, 0044200 | dn);
    m68k_do_execute();
    BOOST_TEST(regs.d[dn] == v1);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 0;
    raw_write16(0, 0044201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.n);
}
BOOST_AUTO_TEST_CASE(negative) {
    uint8_t v1 = get_v8() | 0x80;
    regs.d[1] = v1;
    raw_write16(0, 0044201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0xff00 | v1));
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word_to_Long)
BOOST_AUTO_TEST_CASE(positive) {
    auto dn = rand_reg();
    uint16_t v1 = (get_v16() & 0x7fff) | 1;
    regs.d[dn] = v1;
    raw_write16(0, 0044300 | dn);
    m68k_do_execute();
    BOOST_TEST(regs.d[dn] == v1);
    BOOST_TEST(!regs.z);
    BOOST_TEST(!regs.n);
}

BOOST_AUTO_TEST_CASE(zero) {
    regs.d[1] = 0;
    raw_write16(0, 0044301);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0);
    BOOST_TEST(regs.z);
    BOOST_TEST(!regs.n);
}
BOOST_AUTO_TEST_CASE(negative) {
    uint16_t v1 = get_v16() | 0x8000;
    regs.d[1] = v1;
    raw_write16(0, 0044301);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0xffff0000 | v1));
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()