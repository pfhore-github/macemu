#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(NOT, InitFix)
BOOST_AUTO_TEST_SUITE(Byte) 
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    uint8_t v = get_v8();
    regs.d[ea] = v;
    raw_write16(0, 0043000 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (0xff ^ v));
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0043001);
    m68k_do_execute();
    BOOST_TEST( regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xff;
    raw_write16(0, 0043001);
    m68k_do_execute();
    BOOST_TEST( regs.z);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word) 
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    uint16_t v = get_v16();
    regs.d[ea] = v;
    raw_write16(0, 0043100 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (0xffff ^ v));
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0043101);
    m68k_do_execute();
    BOOST_TEST( regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0043101);
    m68k_do_execute();
    BOOST_TEST( regs.z);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long) 
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    uint32_t v = get_v32();
    regs.d[ea] = v;
    raw_write16(0, 0043200 | ea);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == ~ v);
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 20;
    raw_write16(0, 0043201);
    m68k_do_execute();
    BOOST_TEST( regs.n);
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0043201);
    m68k_do_execute();
    BOOST_TEST( regs.z);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()