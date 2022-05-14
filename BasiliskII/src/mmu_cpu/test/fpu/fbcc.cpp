#define BOOST_TEST_DYN_LINK
#include "fpu/fpu_mpfr.h"
#include "memory.h"
#include "newcpu.h"
#include "test/test_common.h"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(FBCC, InitFix)
BOOST_AUTO_TEST_CASE( t )
{    
    regs.FPSR.z = true;
    asm_m68k("fbeq NEXT\nNOP\nNEXT:\nNOP");
    m68k_do_execute();
    BOOST_TEST( regs.pc = 6);
}

BOOST_AUTO_TEST_CASE( f )
{    
    regs.FPSR.z = false;
    asm_m68k("fbeq NEXT\nNOP\nNEXT:\nNOP");
    m68k_do_execute();
    BOOST_TEST( regs.pc = 2);
}

BOOST_AUTO_TEST_CASE( t_w )
{    
    regs.FPSR.z = false;
    raw_write16(0, 0x724F);
    raw_write16(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST( regs.pc = 0x1002);
}

BOOST_AUTO_TEST_CASE( t_l )
{    
    regs.z = false;
    raw_write16(0, 0x726F);
    raw_write32(2, 0x1000);
    m68k_do_execute();
    BOOST_TEST( regs.pc = 0x1002);
}

BOOST_AUTO_TEST_SUITE_END()