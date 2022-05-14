#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ADDQ, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(operand, REG(), im) {
    auto ea = rand_reg();
    auto v1 = get_v8();
    regs.d[ea] = v1;
    raw_write16(0, 0050000 | ea | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == ((v1 + (im ? im : 8)) & 0xff));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xff;
    raw_write16(0, 0051001);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7f;
    raw_write16(0, 0052001);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xff;
    raw_write16(0, 0051001);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7f;
    raw_write16(0, 0051001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(operand, REG(), im) {
    auto ea = rand_reg();
    auto v1 = get_v16();
    regs.d[ea] = v1;
    raw_write16(0, 0050100 | ea | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == ((v1 + (im ? im : 8)) & 0xffff));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0051101);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fff;
    raw_write16(0, 0052101);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0051101);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fff;
    raw_write16(0, 0051101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(AWord)
BOOST_DATA_TEST_CASE(operand, REG(), im) {
    auto ea = rand_reg();
    auto v1 = get_v16();
    regs.a[ea] = v1;
    raw_write16(0, 0050110 | ea | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.a[ea] == ((int16_t)(v1) + (im ? im : 8)) );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(operand, REG(), im) {
    auto ea = rand_reg();
    auto v1 = get_v32();
    regs.d[ea] = v1;
    raw_write16(0, 0050200 | ea | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (v1 + (im ? im : 8)));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0052201);
    m68k_do_execute();
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.c);
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fffffff;
    raw_write16(0, 0052201);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0051201);
    m68k_do_execute();
    BOOST_TEST(regs.z);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fffffff;
    raw_write16(0, 0051201);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ALong)
BOOST_DATA_TEST_CASE(operand, REG(), im) {
    auto ea = rand_reg();
    auto v1 = get_v32();
    regs.a[ea] = v1;
    raw_write16(0, 0050210 | ea | im << 9);
    m68k_do_execute();
    BOOST_TEST(regs.a[ea] == (v1 + (im ? im : 8)));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()