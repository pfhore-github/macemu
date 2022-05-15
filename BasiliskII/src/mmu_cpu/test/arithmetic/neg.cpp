#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(NEG, InitFix)
BOOST_AUTO_TEST_SUITE(Byte) 
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    uint8_t v = get_v8();
    regs.d[ea] = v;
    raw_write16(0, 0042000 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0x100 - v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0042001);
    m68k_do_execute();
    BOOST_TEST( regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0042001);
    m68k_do_execute();
    BOOST_TEST( regs.z);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0042001);
    m68k_do_execute();
    BOOST_TEST( regs.c);
    BOOST_TEST( regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80;
    raw_write16(0, 0042001);
    m68k_do_execute();
    BOOST_TEST( regs.v);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word) 
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    uint16_t v = get_v16();
    regs.d[ea] = v;
    raw_write16(0, 0042100 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == 0x10000 - v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0042101);
    m68k_do_execute();
    BOOST_TEST( regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0042101);
    m68k_do_execute();
    BOOST_TEST( regs.z);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0042101);
    m68k_do_execute();
    BOOST_TEST( regs.c);
    BOOST_TEST( regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x8000;
    raw_write16(0, 0042101);
    m68k_do_execute();
    BOOST_TEST( regs.v);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long) 
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    uint32_t v = get_v32();
    regs.d[ea] = v;
    raw_write16(0, 0042200 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == - v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0042201);
    m68k_do_execute();
    BOOST_TEST( regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0;
    raw_write16(0, 0042201);
    m68k_do_execute();
    BOOST_TEST( regs.z);
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 1;
    raw_write16(0, 0042201);
    m68k_do_execute();
    BOOST_TEST( regs.c);
    BOOST_TEST( regs.x);
}

BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x80000000;
    raw_write16(0, 0042201);
    m68k_do_execute();
    BOOST_TEST( regs.v);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()