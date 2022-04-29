#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(EXT, InitFix)
BOOST_DATA_TEST_CASE(opc, REG(), dn) {
    BOOST_TEST(opc_map[0044200 | dn] == opc_map[0044200]);
    BOOST_TEST(opc_map[0044300 | dn] == opc_map[0044300]);
}

BOOST_AUTO_TEST_SUITE(Byte_to_word)
BOOST_AUTO_TEST_CASE(positive) {
    regs.d[1] = 44;
    raw_write16(0, 0044201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 44);
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
    regs.d[1] = -22 & 0xff;
    raw_write16(0, 0044201);
    raw_write16(0, 0044201);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (-22 & 0xffff));
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word_to_Long)
BOOST_AUTO_TEST_CASE(positive) {
    regs.d[1] = 4400;
    raw_write16(0, 0044301);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 4400);
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
    regs.d[1] = -2222 & 0xffff;
    raw_write16(0, 0044301);
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == -2222);
    BOOST_TEST(!regs.z);
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()