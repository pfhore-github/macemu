
#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(SUBQ, InitFix)

BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(operand, REG(), im) {
    auto ea = rand_reg();
    auto v1 = get_v8();
    regs.d[ea] = v1;
    raw_write16(0, 0050400 | ea | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == ((v1 - (im ? im : 8)) & 0xff));
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
BOOST_DATA_TEST_CASE(operand, REG(), im) {
    auto ea = rand_reg();
    auto v1 = get_v16();
    regs.d[ea] = v1;
    raw_write16(0, 0050500 | ea | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == ((v1 - (im ? im : 8)) & 0xffff));
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

BOOST_DATA_TEST_CASE(AWord, REG(), im) {
    regs.a[1] = 0x1234;
    raw_write16(0, 0050511 | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x1234 - (im ? im : 8));
}


BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(operand, REG(), im) {
    auto ea = rand_reg();
    auto v1 = get_v32();
    regs.d[ea] = v1;
    raw_write16(0, 0050600 | ea | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (v1 - (im ? im : 8)));
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
BOOST_DATA_TEST_CASE(ALong, REG(), im) {
    regs.a[1] = 0x20000002;
    raw_write16(0, 0050611 | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.a[1] == 0x20000002- (im ? im : 8));
}

BOOST_AUTO_TEST_SUITE_END()
