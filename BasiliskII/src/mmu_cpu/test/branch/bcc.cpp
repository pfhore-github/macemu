#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(BCC, InitFix)
BOOST_AUTO_TEST_CASE(Byte) {
    raw_write16(0, 0x6610);
    regs.z = false;
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(Word) {
    raw_write16(0, 0x6600);
    raw_write16(2, 0x1000);
    regs.z = false;
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x1002);
}

BOOST_AUTO_TEST_CASE(Long) {
    raw_write16(0, 0x66ff);
    raw_write32(2, 0x100000);
    regs.z = false;
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x100002);
}

BOOST_AUTO_TEST_SUITE(hi)
BOOST_AUTO_TEST_CASE(t) {
    regs.c = false;
    regs.z = false;
    raw_write16(0, 0x6210);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.c = true;
    regs.z = false;
    raw_write16(0, 0x6210);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.c = false;
    regs.z = true;
    raw_write16(0, 0x6210);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ls)
BOOST_AUTO_TEST_CASE(t1) {
    regs.c = true;
    regs.z = false;
    raw_write16(0, 0x6310);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.c = false;
    regs.z = true;
    raw_write16(0, 0x6310);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.c = false;
    regs.z = false;
    raw_write16(0, 0x6310);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x02);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(cc)
BOOST_AUTO_TEST_CASE(t) {
    regs.c = false;
    raw_write16(0, 0x6410);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.c = true;
    raw_write16(0, 0x6410);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(cs)
BOOST_AUTO_TEST_CASE(t) {
    regs.c = true;
    raw_write16(0, 0x6510);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.c = false;
    raw_write16(0, 0x6510);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ne)
BOOST_AUTO_TEST_CASE(t) {
    regs.z = false;
    raw_write16(0, 0x6610);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.z = true;
    raw_write16(0, 0x6610);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(eq)
BOOST_AUTO_TEST_CASE(t) {
    regs.z = true;
    raw_write16(0, 0x6710);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.z = false;
    raw_write16(0, 0x6710);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(vc)
BOOST_AUTO_TEST_CASE(t) {
    regs.v = false;
    raw_write16(0, 0x6810);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.v = true;
    raw_write16(0, 0x6810);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(vs)
BOOST_AUTO_TEST_CASE(t) {
    regs.v = true;
    raw_write16(0, 0x6910);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.v = false;
    raw_write16(0, 0x6910);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(pl)
BOOST_AUTO_TEST_CASE(t) {
    regs.n = false;
    raw_write16(0, 0x6A10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}
BOOST_AUTO_TEST_CASE(f) {
    regs.n = true;
    raw_write16(0, 0x6A10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(mi)
BOOST_AUTO_TEST_CASE(t) {
    regs.n = true;
    raw_write16(0, 0x6B10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f) {
    regs.n = false;
    raw_write16(0, 0x6B10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ge)
BOOST_AUTO_TEST_CASE(t1) {
    regs.n = true;
    regs.v = true;
    raw_write16(0, 0x6C10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.n = false;
    regs.v = false;
    raw_write16(0, 0x6C10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.n = true;
    regs.v = false;
    raw_write16(0, 0x6C10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.n = false;
    regs.v = true;
    raw_write16(0, 0x6C10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(lt)
BOOST_AUTO_TEST_CASE(t1) {
    regs.n = true;
    regs.v = false;
    raw_write16(0, 0x6D10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.n = false;
    regs.v = true;
    raw_write16(0, 0x6D10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.n = true;
    regs.v = true;
    raw_write16(0, 0x6D10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.n = false;
    regs.v = false;
    raw_write16(0, 0x6D10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(gt)
BOOST_AUTO_TEST_CASE(t1) {
    regs.z = false;
    regs.n = true;
    regs.v = true;
    raw_write16(0, 0x6E10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(t2) {
    regs.z = false;
    regs.n = false;
    regs.v = false;
    raw_write16(0, 0x6E10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.z = false;
    regs.n = true;
    regs.v = false;
    raw_write16(0, 0x6E10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.z = false;
    regs.n = false;
    regs.v = true;
    raw_write16(0, 0x6E10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_CASE(f3) {
    regs.z = true;
    raw_write16(0, 0x6E10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(le)
BOOST_AUTO_TEST_CASE(t1) {
    regs.z = true;
    raw_write16(0, 0x6F10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}
BOOST_AUTO_TEST_CASE(t2) {
    regs.z = false;
    regs.n = true;
    regs.v = false;
    raw_write16(0, 0x6F10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(t3) {
    regs.z = false;
    regs.n = false;
    regs.v = true;
    raw_write16(0, 0x6F10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x12);
}

BOOST_AUTO_TEST_CASE(f1) {
    regs.z = false;
    regs.n = true;
    regs.v = true;
    raw_write16(0, 0x6F10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_CASE(f2) {
    regs.z = false;
    regs.n = false;
    regs.v = false;
    raw_write16(0, 0x6F10);
    m68k_do_execute();
    BOOST_TEST(regs.pc == 0x2);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()