#define BOOST_TEST_DYN_LINK
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <vector>
BOOST_FIXTURE_TEST_SUITE(MOVE, InitFix)
BOOST_AUTO_TEST_SUITE(Byte)

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    regs.d[1] = value << 7;
    asm_m68k("move.b %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    regs.d[1] = !value;
    asm_m68k("move.b %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x30;
    regs.d[2] = 0x12345678;
    asm_m68k("move.b %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12345630);
}

BOOST_AUTO_TEST_CASE(address) {
    regs.d[1] = 0xff;
    regs.a[2] = 0x10;
    asm_m68k("move.b %D1, (%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0xff);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(inc) {
    regs.d[1] = 0xff;
    regs.a[2] = 0x10;
    asm_m68k("move.b %D1, (%A2)+");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0xff);
    BOOST_TEST(regs.a[2] == 0x11);
}

BOOST_AUTO_TEST_CASE(dec) {
    regs.d[1] = 0xff;
    regs.a[2] = 0x11;
    asm_m68k("move.b %D1, -(%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == 0xff);
    BOOST_TEST(regs.a[2] == 0x10);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(from)
BOOST_AUTO_TEST_CASE(address) {
    regs.a[2] = 0x10;
    raw_write8(0x10, 0xff);
    asm_m68k("move.b (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(inc) {
    raw_write8(0x10, 0xff);
    regs.a[2] = 0x10;
    asm_m68k("move.b (%A2)+, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x11);
    BOOST_TEST(regs.d[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(dec) {
    raw_write8(0x10, 0xff);
    regs.a[2] = 0x11;
    asm_m68k("move.b -(%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(regs.d[1] == 0xff);
}

BOOST_AUTO_TEST_CASE(imm) {
    asm_m68k("move.b #200, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 200);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    regs.d[1] = value << 15;
    asm_m68k("move.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    regs.d[1] = !value;
    asm_m68k("move.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x3040;
    regs.d[2] = 0x12345678;
    asm_m68k("move.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12343040);
}

BOOST_AUTO_TEST_CASE(address) {
    regs.d[1] = 0xffff;
    regs.a[2] = 0x10;
    asm_m68k("move.w %D1, (%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0xffff);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(inc) {
    regs.d[1] = 0xffff;
    regs.a[2] = 0x10;
    asm_m68k("move.w %D1, (%A2)+");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0xffff);
    BOOST_TEST(regs.a[2] == 0x12);
}

BOOST_AUTO_TEST_CASE(dec) {
    regs.d[1] = 0xffff;
    regs.a[2] = 0x12;
    asm_m68k("move.w %D1, -(%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == 0xffff);
    BOOST_TEST(regs.a[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(ccr) {
    regs.d[1] = 0xff;
    asm_m68k("move.w %D1, %CCR");
    m68k_do_execute();
    BOOST_TEST(regs.v);
    BOOST_TEST(regs.n);
    BOOST_TEST(regs.c);
    BOOST_TEST(regs.z);
    BOOST_TEST(regs.x);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(from)
BOOST_AUTO_TEST_CASE(address) {
    regs.a[2] = 0x10;
    raw_write16(0x10, 0xffff);
    asm_m68k("move.w (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffff);
}

BOOST_AUTO_TEST_CASE(inc) {
    raw_write16(0x10, 0xffff);
    regs.a[2] = 0x10;
    asm_m68k("move.w (%A2)+, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x12);
    BOOST_TEST(regs.d[1] == 0xffff);
}

BOOST_AUTO_TEST_CASE(dec) {
    raw_write16(0x10, 0xffff);
    regs.a[2] = 0x12;
    asm_m68k("move.w -(%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(regs.d[1] == 0xffff);
}

BOOST_AUTO_TEST_CASE(imm) {
    asm_m68k("move.w #2000, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 2000);
}

BOOST_AUTO_TEST_CASE(ccr) {
    regs.v = regs.x = regs.c = regs.z = regs.n = true;
    asm_m68k("move.w %CCR, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] = 0x1f);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE(Long)

BOOST_DATA_TEST_CASE(n, boost::unit_test::data::xrange(1), value) {
    regs.d[1] = value << 31;
    asm_m68k("move.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.n == value);
}

BOOST_DATA_TEST_CASE(z, boost::unit_test::data::xrange(1), value) {
    regs.d[1] = !value;
    asm_m68k("move.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.z == value);
}
BOOST_AUTO_TEST_SUITE(to)
BOOST_AUTO_TEST_CASE(reg) {
    regs.d[1] = 0x12345678;
    asm_m68k("move.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST(regs.d[2] == 0x12345678);
}

BOOST_AUTO_TEST_CASE(address) {
    regs.d[1] = 0xffffffff;
    regs.a[2] = 0x10;
    asm_m68k("move.l %D1, (%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0xffffffff);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(inc) {
    regs.d[1] = 0xffffffff;
    regs.a[2] = 0x10;
    asm_m68k("move.l %D1, (%A2)+");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0xffffffff);
    BOOST_TEST(regs.a[2] == 0x14);
}

BOOST_AUTO_TEST_CASE(dec) {
    regs.d[1] = 0xffffffff;
    regs.a[2] = 0x14;
    asm_m68k("move.l %D1, -(%A2)");
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == 0xffffffff);
    BOOST_TEST(regs.a[2] == 0x10);
}

BOOST_AUTO_TEST_CASE(usp) {
    regs.a[2] = 0x14;
    regs.S = true;
    asm_m68k("move.l %A2, %USP");
    m68k_do_execute();
    BOOST_TEST(regs.usp == 0x14);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(from)
BOOST_AUTO_TEST_CASE(address) {
    regs.a[2] = 0x10;
    raw_write32(0x10, 0xffffffff);
    asm_m68k("move.l (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0xffffffff);
}

BOOST_AUTO_TEST_CASE(inc) {
    raw_write32(0x10, 0xffffffff);
    regs.a[2] = 0x10;
    asm_m68k("move.l (%A2)+, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x14);
    BOOST_TEST(regs.d[1] == 0xffffffff);
}

BOOST_AUTO_TEST_CASE(dec) {
    raw_write32(0x10, 0xffffffff);
    regs.a[2] = 0x14;
    asm_m68k("move.l -(%A2), %D1");
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x10);
    BOOST_TEST(regs.d[1] == 0xffffffff);
}

BOOST_AUTO_TEST_CASE(imm) {
    asm_m68k("move.l #0x10000, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == 0x10000);
}

BOOST_AUTO_TEST_CASE(usp) {
    regs.usp = 0x1234;
    regs.S = true;
    asm_m68k("move.l %USP, %A2");
    m68k_do_execute();
    BOOST_TEST(regs.a[2] == 0x1234);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(MOVEA, InitFix)
BOOST_AUTO_TEST_CASE( Word)
{    
    regs.d[1] = 0x8040;
    asm_m68k("movea.w %D1, %A2");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0xffff8040 );
}


BOOST_AUTO_TEST_CASE( Long)
{    
    regs.d[1] = 0x12345678;
    asm_m68k("movea.l %D1, %A2");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0x12345678 );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE(MOVEQ, InitFix)
BOOST_AUTO_TEST_CASE( n  )
{    
    asm_m68k("moveq #-4, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n);
    BOOST_TEST( regs.d[1] == -4);
}

BOOST_AUTO_TEST_CASE( z  )
{    
    asm_m68k("moveq #0, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( regs.d[1] == 0);
}
BOOST_AUTO_TEST_SUITE_END()