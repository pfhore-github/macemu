#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( JSR, InitFix )
{    
    regs.a[7] = 0x204;
    regs.a[3] = 0x1000;
    asm_m68k("jsr (%A3)");
    m68k_do_execute();
    BOOST_TEST( raw_read32( 0x200) == 2);
    BOOST_TEST( regs.pc = 0x1000);
}

BOOST_FIXTURE_TEST_CASE( JMP, InitFix )
{    
    regs.a[3] = 0x1000;
    asm_m68k("jmp (%A3)");
    m68k_do_execute();
    BOOST_TEST( regs.pc = 0x1000);
}

BOOST_FIXTURE_TEST_CASE( BRA, InitFix )
{    
    asm_m68k("bra NEXT\nNOP\nNEXT:\nNOP");
    m68k_do_execute();
    BOOST_TEST( regs.pc = 4);
}

BOOST_FIXTURE_TEST_CASE( BRA_W, InitFix )
{    
    raw_write16(0, 0x6000);
    raw_write16(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST( regs.pc = 0x1002);
}

BOOST_FIXTURE_TEST_CASE( BRA_L, InitFix )
{    
    raw_write16(0, 0x60ff);
    raw_write32(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST( regs.pc = 0x1002);
}

BOOST_FIXTURE_TEST_CASE( BSR, InitFix )
{    
    regs.a[7] = 0x104;
    asm_m68k("bsr NEXT\nNOP\nNEXT:\nNOP");
    m68k_do_execute();
    BOOST_TEST( regs.a[7] == 0x100);
    BOOST_TEST( raw_read32( 0x100) == 2);
    BOOST_TEST( regs.pc = 4);
}

BOOST_FIXTURE_TEST_CASE( BSR_W, InitFix )
{    
    regs.a[7] = 0x104;
    raw_write16(0, 0x6100);
    raw_write16(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST( regs.a[7] == 0x100);
    BOOST_TEST( raw_read32( 0x100) == 2);
    BOOST_TEST( regs.pc = 0x1002);
}

BOOST_FIXTURE_TEST_CASE( BSR_L, InitFix )
{    
    regs.a[7] = 0x104;
    raw_write16(0, 0x61ff);
    raw_write32(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST( regs.a[7] == 0x100);
    BOOST_TEST( raw_read32( 0x100) == 2);
    BOOST_TEST( regs.pc = 0x1002);
}

BOOST_FIXTURE_TEST_CASE( BCC_t, InitFix )
{    
    regs.z = true;
    asm_m68k("beq NEXT\nNOP\nNEXT:\nNOP");
    m68k_do_execute();
    BOOST_TEST( regs.pc = 4);
}

BOOST_FIXTURE_TEST_CASE( BCC_f, InitFix )
{    
    regs.z = false;
    asm_m68k("beq NEXT\nNOP\nNEXT:\nNOP");
    m68k_do_execute();
    BOOST_TEST( regs.pc = 2);
}

BOOST_FIXTURE_TEST_CASE( BCC_W, InitFix )
{    
    regs.z = false;
    raw_write16(0, 0x6700);
    raw_write16(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST( regs.pc = 0x1002);
}

BOOST_FIXTURE_TEST_CASE( BCC_L, InitFix )
{    
    regs.z = false;
    raw_write16(0, 0x67ff);
    raw_write32(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST( regs.pc = 0x1002);
}