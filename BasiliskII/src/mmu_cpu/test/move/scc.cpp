#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(SCC, InitFix)
BOOST_AUTO_TEST_CASE(t) {
    auto ea = rand_reg();
    raw_write16(0, 0x50C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    auto ea = rand_reg();
    raw_write16(0, 0x51C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE(hi)
BOOST_AUTO_TEST_CASE(t) {
    regs.c = false;
    regs.z = false;
    auto ea = rand_reg();
    raw_write16(0, 0x52C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.c = true;
    regs.z = false;
    auto ea = rand_reg();
    raw_write16(0, 0x52C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.c = false;
    regs.z = true;
    auto ea = rand_reg();
    raw_write16(0, 0x52C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ls)
BOOST_AUTO_TEST_CASE(t1) {
    regs.c = true;
    regs.z = false;
    auto ea = rand_reg();
    raw_write16(0, 0x53C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.c = false;
    regs.z = true;
    auto ea = rand_reg();
    raw_write16(0, 0x53C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}
BOOST_AUTO_TEST_CASE(f) {
    regs.c = false;
    regs.z = false;
    auto ea = rand_reg();
    raw_write16(0, 0x53C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(cc)
BOOST_AUTO_TEST_CASE(t) {
    regs.c = false;
    auto ea = rand_reg();
    raw_write16(0, 0x54C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.c = true;
    auto ea = rand_reg();
    raw_write16(0, 0x54C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(cs)
BOOST_AUTO_TEST_CASE(t) {
    regs.c = true;
    auto ea = rand_reg();
    raw_write16(0, 0x55C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.c = false;
    auto ea = rand_reg();
    raw_write16(0, 0x55C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ne)
BOOST_AUTO_TEST_CASE(t) {
    regs.z = false;
    auto ea = rand_reg();
    raw_write16(0, 0x56C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.z = true;
    auto ea = rand_reg();
    raw_write16(0, 0x56C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(eq)
BOOST_AUTO_TEST_CASE(t) {
    regs.z = true;
    auto ea = rand_reg();
    raw_write16(0, 0x57C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.z = false;
    auto ea = rand_reg();
    raw_write16(0, 0x57C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(vc)
BOOST_AUTO_TEST_CASE(t) {
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x58C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x58C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(vs)
BOOST_AUTO_TEST_CASE(t) {
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x59C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x59C0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(pl)
BOOST_AUTO_TEST_CASE(t) {
    regs.n = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5AC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.n = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5AC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(mi)
BOOST_AUTO_TEST_CASE(t) {
    regs.n = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5BC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.n = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5BC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ge)
BOOST_AUTO_TEST_CASE(t1) {
    regs.n = true;
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5CC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.n = false;
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5CC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.n = true;
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5CC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.n = false;
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5CC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(lt)
BOOST_AUTO_TEST_CASE(t1) {
    regs.n = true;
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5DC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.n = false;
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5DC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.n = true;
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5DC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.n = false;
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5DC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(gt)
BOOST_AUTO_TEST_CASE(t1) {
    regs.z = false;
    regs.n = true;
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5EC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.z = false;
    regs.n = false;
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5EC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.z = false;
    regs.n = true;
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5EC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.z = false;
    regs.n = false;
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5EC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_CASE(f3) {
    regs.z = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5EC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(le)
BOOST_AUTO_TEST_CASE(t1) {
    regs.z = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5FC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}
BOOST_AUTO_TEST_CASE(t2) {
    regs.z = false;
    regs.n = true;
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5FC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(t3) {
    regs.z = false;
    regs.n = false;
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5FC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0xff);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.z = false;
    regs.n = true;
    regs.v = true;
    auto ea = rand_reg();
    raw_write16(0, 0x5FC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.z = false;
    regs.n = false;
    regs.v = false;
    auto ea = rand_reg();
    raw_write16(0, 0x5FC0 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()