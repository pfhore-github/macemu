#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE( OR, InitFix )
BOOST_AUTO_TEST_SUITE( Byte )

BOOST_AUTO_TEST_CASE( RegtoReg )
{    
    regs.d[1] = 0x12345678;
    regs.d[2] = 0x2a;
    asm_m68k("orb %D2, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12345678 | 0x2a) );
}

BOOST_AUTO_TEST_CASE( MemToReg )
{    
    regs.d[1] = 0x12345678;
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x2a);
    asm_m68k("orb (%A1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12345678 | 0x2a) );
}

BOOST_AUTO_TEST_CASE( RegToMem )
{    
    regs.d[1] = 0x12345678;
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x2a);
    asm_m68k("orb %D1, (%A1)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == (0x78 | 0x2a) );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( Word )
BOOST_AUTO_TEST_CASE( RegtoReg )
{    
    regs.d[1] = 0x12345678;
    regs.d[2] = 0xdead;
    asm_m68k("orw %D2, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12345678 | 0xdead) );
}

BOOST_AUTO_TEST_CASE( MemToReg )
{    
    regs.d[1] = 0x12345678;
    regs.a[1] = 0x10;
    raw_write16(0x10, 0xdead);
    asm_m68k("orw (%A1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12345678 | 0xdead) );
}

BOOST_AUTO_TEST_CASE( RegToMem )
{    
    regs.d[1] = 0x12345678;
    regs.a[1] = 0x10;
    raw_write16(0x10, 0x2a2b);
    asm_m68k("orw %D1, (%A1)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == (0x5678 | 0x2a2b) );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( Long )
BOOST_AUTO_TEST_CASE( RegtoReg )
{    
    regs.d[1] = 0x12345678;
    regs.d[2] = 0xdeadbeaf;
    asm_m68k("orl %D2, %D1");

    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12345678 | 0xdeadbeaf) );
}

BOOST_AUTO_TEST_CASE( MemToReg )
{    
    regs.d[1] = 0x12345678;
    regs.a[1] = 0x10;
    raw_write32(0x10, 0xdeadbeaf);
    asm_m68k("orl (%A1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12345678 | 0xdeadbeaf) );
}

BOOST_AUTO_TEST_CASE( RegToMem )
{    
    regs.d[1] = 0x12345678;
    regs.a[1] = 0x10;
    raw_write32(0x10, 0x2a2b2c2d);
    asm_m68k("orl %D1, (%A1)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == (0x12345678 | 0x2a2b2c2d) );
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE( ORI, InitFix )
BOOST_AUTO_TEST_SUITE(Byte)
BOOST_AUTO_TEST_CASE(value) {
    regs.v = regs.c = regs.x = regs.z = true;
    regs.n = false;
    regs.d[1] = 0x12345698;
    asm_m68k("orib #0x2a, %D1");
    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345698 | 0x2a));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.a[1] = 0x10;
    raw_write8(0x10, 0x12);
    asm_m68k("orib #0x2a, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read8(0x10) == (0x12 | 0x2a));
}

BOOST_AUTO_TEST_CASE(ccr) {
    regs.pc = 0;
    regs.vbr = 0;
    regs.x = regs.v = regs.c = regs.n = regs.z = false;

    asm_m68k("orib #0xff, %CCR");

    m68k_do_execute();
    BOOST_TEST(regs.x == true);
    BOOST_TEST(regs.v == true);
    BOOST_TEST(regs.c == true);
    BOOST_TEST(regs.n == true);
    BOOST_TEST(regs.z == true);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Word)
BOOST_AUTO_TEST_CASE(value) {
    regs.v = regs.c = regs.x = regs.z = true;
    regs.n = false;
    regs.d[1] = 0x12345678;

    asm_m68k("oriw #0xdead, %D1");

    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345678 | 0xdead));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.a[1] = 0x10;
    raw_write16(0x10, 0x1234);
    asm_m68k("oriw #0x2abc, (%A1)");
    m68k_do_execute();
    BOOST_TEST(raw_read16(0x10) == (0x1234 | 0x2abc));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Long)
BOOST_AUTO_TEST_CASE(value) {
    regs.d[1] = 0x12345678;
    regs.v = regs.c = regs.x = regs.z = true;
    regs.n = false;
    asm_m68k("oril #0xdeadbeaf, %D1");

    m68k_do_execute();
    BOOST_TEST(regs.d[1] == (0x12345678 | 0xdeadbeaf));
    BOOST_TEST(!regs.v);
    BOOST_TEST(!regs.c);
    BOOST_TEST(regs.x);
    BOOST_TEST(regs.n);
    BOOST_TEST(!regs.z);
}

BOOST_AUTO_TEST_CASE(value_m) {
    regs.a[1] = 0x10;

    asm_m68k("oril #0x2abc5412, (%A1)");
    raw_write32(0x10, 0x1234dead);
    m68k_do_execute();
    BOOST_TEST(raw_read32(0x10) == (0x2abc5412 | 0x1234dead));
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
