#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(TST, InitFix)
BOOST_AUTO_TEST_CASE(Byte) {
    auto ea = rand_reg();
    auto v1 = get_v8();
    regs.d[ea] = v1;
    raw_write16(0, 0045000 | ea);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.n == !!(v1 & 0x80));
    BOOST_TEST(regs.z == (v1 == 0));
}

BOOST_AUTO_TEST_CASE(Word) {
    auto ea = rand_reg();
    auto v1 = get_v16();
    regs.d[ea] = v1;
    raw_write16(0, 0045100 | ea);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.n == !!(v1 & 0x8000));
    BOOST_TEST(regs.z == (v1 == 0));
}

BOOST_AUTO_TEST_CASE(Long) {
    auto ea = rand_reg();
    auto v1 = get_v32();
    regs.d[ea] = v1;
    raw_write16(0, 0045200 | ea);
    regs.x = regs.v = regs.c = true;
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
    BOOST_TEST(regs.n == !!(v1 & 0x80000000));
    BOOST_TEST(regs.z == (v1 == 0));
}

BOOST_AUTO_TEST_SUITE_END()