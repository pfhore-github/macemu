#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(SCC, InitFix)
BOOST_AUTO_TEST_CASE(t) {
    raw_write16(0, 0x50C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    raw_write16(0, 0x51C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE(hi)
BOOST_AUTO_TEST_CASE(t) {
    regs.c = false;
    regs.z = false;
    raw_write16(0, 0x52C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.c = true;
    regs.z = false;
    raw_write16(0, 0x52C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.c = false;
    regs.z = true;
    raw_write16(0, 0x52C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ls)
BOOST_AUTO_TEST_CASE(t1) {
    regs.c = true;
    regs.z = false;
    raw_write16(0, 0x53C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.c = false;
    regs.z = true;
    raw_write16(0, 0x53C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}
BOOST_AUTO_TEST_CASE(f) {
    regs.c = false;
    regs.z = false;
    raw_write16(0, 0x53C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(cc)
BOOST_AUTO_TEST_CASE(t) {
    regs.c = false;
    raw_write16(0, 0x54C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.c = true;
    raw_write16(0, 0x54C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(cs)
BOOST_AUTO_TEST_CASE(t) {
    regs.c = true;
    raw_write16(0, 0x55C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.c = false;
    raw_write16(0, 0x55C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ne)
BOOST_AUTO_TEST_CASE(t) {
    regs.z = false;
    raw_write16(0, 0x56C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.z = true;
    raw_write16(0, 0x56C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(eq)
BOOST_AUTO_TEST_CASE(t) {
    regs.z = true;
    raw_write16(0, 0x57C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.z = false;
    raw_write16(0, 0x57C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(vc)
BOOST_AUTO_TEST_CASE(t) {
    regs.v = false;
    raw_write16(0, 0x58C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.v = true;
    raw_write16(0, 0x58C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(vs)
BOOST_AUTO_TEST_CASE(t) {
    regs.v = true;
    raw_write16(0, 0x59C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.v = false;
    raw_write16(0, 0x59C2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(pl)
BOOST_AUTO_TEST_CASE(t) {
    regs.n = false;
    raw_write16(0, 0x5AC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.n = true;
    raw_write16(0, 0x5AC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(mi)
BOOST_AUTO_TEST_CASE(t) {
    regs.n = true;
    raw_write16(0, 0x5BC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.n = false;
    raw_write16(0, 0x5BC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ge)
BOOST_AUTO_TEST_CASE(t1) {
    regs.n = true;
    regs.v = true;
    raw_write16(0, 0x5CC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.n = false;
    regs.v = false;
    raw_write16(0, 0x5CC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.n = true;
    regs.v = false;
    raw_write16(0, 0x5CC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.n = false;
    regs.v = true;
    raw_write16(0, 0x5CC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(lt)
BOOST_AUTO_TEST_CASE(t1) {
    regs.n = true;
    regs.v = false;
    raw_write16(0, 0x5DC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.n = false;
    regs.v = true;
    raw_write16(0, 0x5DC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.n = true;
    regs.v = true;
    raw_write16(0, 0x5DC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.n = false;
    regs.v = false;
    raw_write16(0, 0x5DC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(gt)
BOOST_AUTO_TEST_CASE(t1) {
    regs.z = false;
    regs.n = true;
    regs.v = true;
    raw_write16(0, 0x5EC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.z = false;
    regs.n = false;
    regs.v = false;
    raw_write16(0, 0x5EC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.z = false;
    regs.n = true;
    regs.v = false;
    raw_write16(0, 0x5EC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.z = false;
    regs.n = false;
    regs.v = true;
    raw_write16(0, 0x5EC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(f3) {
    regs.z = true;
    raw_write16(0, 0x5EC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(le)
BOOST_AUTO_TEST_CASE(t1) {
    regs.z = true;
    raw_write16(0, 0x5FC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}
BOOST_AUTO_TEST_CASE(t2) {
    regs.z = false;
    regs.n = true;
    regs.v = false;
    raw_write16(0, 0x5FC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(t3) {
    regs.z = false;
    regs.n = false;
    regs.v = true;
    raw_write16(0, 0x5FC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.z = false;
    regs.n = true;
    regs.v = true;
    raw_write16(0, 0x5FC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.z = false;
    regs.n = false;
    regs.v = false;
    raw_write16(0, 0x5FC2);
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()