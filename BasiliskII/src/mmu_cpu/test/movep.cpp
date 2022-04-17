#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( word_from, InitFix )
{    
    regs.a[1] = 0x10;
    raw_write8(0x14, 0x12);
    raw_write8(0x15, 0x13);
    raw_write8(0x16, 0x14);
    raw_write8(0x17, 0x15);
    asm_m68k("movep.w (4, %A1), %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0x1214 );
}

BOOST_FIXTURE_TEST_CASE( word_to , InitFix )
{    
    regs.a[1] = 0x10;
    regs.d[2] = 0x1234;
    asm_m68k("movep.w %D2, (4, %A1)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x14) == 0x12);
    BOOST_TEST( raw_read8(0x16) == 0x34);
}

BOOST_FIXTURE_TEST_CASE( long_from , InitFix)
{    
    regs.a[1] = 0x10;
    raw_write8(0x14, 0x12);
    raw_write8(0x15, 0x13);
    raw_write8(0x16, 0x14);
    raw_write8(0x17, 0x15);
    raw_write8(0x18, 0x16);
    raw_write8(0x19, 0x17);
    raw_write8(0x1a, 0x18);
    raw_write8(0x1b, 0x19);
    asm_m68k("movep.l (4, %A1), %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0x12141618 );
}

BOOST_FIXTURE_TEST_CASE( long_to, InitFix )
{    
    regs.a[1] = 0x10;
    regs.d[2] = 0x12345678;
    asm_m68k("movep.l %D2, (4, %A1)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x14) == 0x12);
    BOOST_TEST( raw_read8(0x16) == 0x34);
    BOOST_TEST( raw_read8(0x18) == 0x56);
    BOOST_TEST( raw_read8(0x1a) == 0x78);
}