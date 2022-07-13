#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(EXT, InitFix)
BOOST_AUTO_TEST_SUITE(Byte_to_word)
BOOST_AUTO_TEST_CASE(positive) {
    regs.d[2] = 0x3f;
    raw_write16(0, 0044202);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x3f);
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
    regs.d[1] = 0xc0;
    raw_write16(0, 0044201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffc0);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word_to_Long)
BOOST_AUTO_TEST_CASE(positive) {
    regs.d[1] = 0x1000;
    raw_write16(0, 0044301);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x1000);
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
    uint16_t v1 = 0xc000;
    regs.d[1] = v1;
    raw_write16(0, 0044301);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffc000);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()