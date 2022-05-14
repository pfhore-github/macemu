#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
BOOST_FIXTURE_TEST_SUITE(CLR, InitFix)
BOOST_AUTO_TEST_CASE(Byte) {
    auto ea = rand_reg();
    regs.d[ea] = 44;
    regs.x = regs.c = regs.v = regs.n = true;
    raw_write16(0, 0041000 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.n);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(Word) {
    auto ea = rand_reg();
    regs.d[ea] = 4400;
    regs.x = regs.c = regs.v = regs.n = true;
    raw_write16(0, 0041100 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.n);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(Long) {
    auto ea = rand_reg();
    regs.d[ea] = 4400000;
    regs.x = regs.c = regs.v = regs.n = true;
    raw_write16(0, 0041200 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.n);
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_SUITE_END()