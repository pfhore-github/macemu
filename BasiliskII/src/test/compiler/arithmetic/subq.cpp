
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(SUBQ, InitFix)

BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[2] = 40;
    raw_write16(0, 0053402);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 37);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.d[2] = 40;
    raw_write16(0, 0050402);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 32);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0052401);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    raw_write16(0, 0052401);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    raw_write16(0, 0051401);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfe;
    raw_write16(0, 0051401);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[2] = 1000;
    raw_write16(0, 0053502 );
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 997);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0052501);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0052501);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    raw_write16(0, 0051501);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffe;
    raw_write16(0, 0051501);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(AWord)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 0x1234;
    raw_write16(0, 0052511 );
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1232);
}
BOOST_AUTO_TEST_CASE(eight) {
    regs.a[1] = 2000;
    raw_write16(0, 0050511 );
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 1992);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.d[2] = 100009;
    raw_write16(0, 0053602);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 100006);
}

BOOST_AUTO_TEST_CASE(eight) {
    regs.d[2] = 100009;
    raw_write16(0, 0050602);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 100001);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0052601);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0052601);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 1;
    raw_write16(0, 0051601);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0xfffffffe;
    raw_write16(0, 0051601);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(ALong)
BOOST_AUTO_TEST_CASE(non_eight) {
    regs.a[1] = 10000004;
    raw_write16(0, 0052611 );
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 10000002);
}
BOOST_AUTO_TEST_CASE(eight) {
    regs.a[1] = 10000008;
    raw_write16(0, 0050611 );
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 10000000);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
