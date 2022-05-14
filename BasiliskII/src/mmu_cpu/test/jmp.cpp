#define BOOST_TEST_DYN_LINK
#include "newcpu.h"
#include "memory.h"
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
