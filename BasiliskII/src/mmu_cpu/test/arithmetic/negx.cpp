#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(NEGX, InitFix)

BOOST_AUTO_TEST_SUITE(Byte)
BOOST_DATA_TEST_CASE(operand, BIT, value) {
    auto ea = rand_reg();
    uint8_t v = get_v8();
    regs.d[ea] = v;
    regs.x = value;
    raw_write16(0, 0040000 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == ((0x100 - v - value)&0xff));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    regs.x = false;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.x = false;
    raw_write16(0, 0040001);
    m68k_do_execute();
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 3;
    raw_write16(0, 0040001);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_DATA_TEST_CASE(operand, BIT, value) {
    auto ea = rand_reg();
    uint16_t v = get_v16();
    regs.d[ea] = v;
    regs.x = value;
    raw_write16(0, 0040100 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == ((0x10000 - v - value)&0xffff));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    regs.x = false;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.x = false;
    raw_write16(0, 0040101);
    m68k_do_execute();
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 3;
    raw_write16(0, 0040101);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_DATA_TEST_CASE(operand, BIT, value) {
    auto ea = rand_reg();
    uint32_t v = get_v32();
    regs.d[ea] = v;
    regs.x = value;
    raw_write16(0, 0040200 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == - v - value);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    regs.x = false;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(regs.v);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    regs.x = false;
    raw_write16(0, 0040201);
    m68k_do_execute();
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 3;
    raw_write16(0, 0040201);
    m68k_do_execute();
    BOOST_TEST(regs.n);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()