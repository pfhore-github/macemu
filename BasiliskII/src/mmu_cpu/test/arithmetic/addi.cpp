#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE(ADDI, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    auto v1 = get_v8();
    auto v2 = get_v8();
    regs.d[ea] = v1;
    raw_write16(0, 0003000 | ea);
    raw_write16(2, v2);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == ((v1 + v2) & 0xff));
}
BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xff;
    raw_write16(0, 0003001);
    raw_write16(2, 2);
    m68k_do_execute();
    BOOST_TEST( regs.x );
    BOOST_TEST( regs.c );
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7f;
    raw_write16(0, 0003001);
    raw_write16(2, 2);
    m68k_do_execute();
    BOOST_TEST( regs.v );
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xff;
    raw_write16(0, 0003001);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7f;
    raw_write16(0, 0003001);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST( regs.n );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Word)

BOOST_AUTO_TEST_CASE(operand) {
    auto xr = rand_reg();
    auto v1 = get_v16();
    auto v2 = get_v16();
    regs.d[xr] = v1;
    raw_write16(0, 0003100 | xr);
    raw_write16(2, v2);
    m68k_do_execute();
    BOOST_TEST(regs.d[xr] == ((v1 + v2) & 0xffff));
}

BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0003101);
    raw_write16(2, 2);
    m68k_do_execute();
    BOOST_TEST( regs.x );
    BOOST_TEST( regs.c );
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fff;
    raw_write16(0, 0003101);
    raw_write16(2, 2);
    m68k_do_execute();
    BOOST_TEST( regs.v );
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffff;
    raw_write16(0, 0003101);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fff;
    raw_write16(0, 0003101);
    raw_write16(2, 1);
    m68k_do_execute();
    BOOST_TEST( regs.n );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_AUTO_TEST_CASE(operand) {
    auto ea = rand_reg();
    auto v1 = get_v32();
    auto v2 = get_v32();
    regs.d[ea] = v1;
    raw_write16(0, 0003200 | ea);
    raw_write32(2, v2);
    m68k_do_execute();
    BOOST_TEST(regs.d[ea] == (v1+v2));
}


BOOST_AUTO_TEST_CASE(cx) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0003201);
    raw_write32(2, 2);
    m68k_do_execute();
    BOOST_TEST( regs.x );
    BOOST_TEST( regs.c );
}
BOOST_AUTO_TEST_CASE(v) {
    regs.d[1] = 0x7fffffff;
    raw_write16(0, 0003201);
    raw_write32(2, 2);
    m68k_do_execute();
    BOOST_TEST( regs.v );
}

BOOST_AUTO_TEST_CASE(z) {
    regs.d[1] = 0xffffffff;
    raw_write16(0, 0003201);
    raw_write32(2, 1);
    m68k_do_execute();
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE(n) {
    regs.d[1] = 0x7fffffff;
    raw_write16(0, 0003201);
    raw_write32(2, 1);
    m68k_do_execute();
    BOOST_TEST( regs.n );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()