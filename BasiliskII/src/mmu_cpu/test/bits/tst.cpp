#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(TST, InitFix)
BOOST_AUTO_TEST_SUITE(Byte) 

BOOST_DATA_TEST_CASE(n, BIT, n) {
    regs.x = regs.v = regs.c = true;
    regs.d[1] = n << 7;
    raw_write16(0, 0045001);
    m68k_do_execute();
    BOOST_TEST(regs.n == n);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_DATA_TEST_CASE(z, BIT, z) {
    regs.d[1] = z ? 0 : 1;
    raw_write16(0, 0045001);
    m68k_do_execute();
    BOOST_TEST(regs.z == z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word) 
BOOST_DATA_TEST_CASE(n, BIT, n) {
    regs.x = regs.v = regs.c = true;
    regs.d[1] = n << 15;
    raw_write16(0, 0045101);
    m68k_do_execute();
    BOOST_TEST(regs.n == n);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_DATA_TEST_CASE(z, BIT, z) {
    regs.d[1] = z ? 0 : 1;
    raw_write16(0, 0045101);
    m68k_do_execute();
    BOOST_TEST(regs.z == z);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(Long) 
BOOST_DATA_TEST_CASE(n, BIT, n) {
    regs.x = regs.v = regs.c = true;
    regs.d[1] = n << 31;
    raw_write16(0, 0045201);
    m68k_do_execute();
    BOOST_TEST(regs.n == n);
    BOOST_TEST(regs.x);
    BOOST_TEST(!regs.c);
    BOOST_TEST(!regs.v);
}

BOOST_DATA_TEST_CASE(z, BIT, z) {
    regs.d[1] = z ? 0 : 1;
    raw_write16(0, 0045201);
    m68k_do_execute();
    BOOST_TEST(regs.z == z);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()