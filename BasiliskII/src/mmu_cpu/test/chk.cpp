#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_AUTO_TEST_SUITE( Word )

BOOST_AUTO_TEST_CASE( in_range )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x4000;    
    regs.d[1] = 0x6000;
    asm_m68k("chkw %D1, %D2");
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.pc == 2);
}


BOOST_AUTO_TEST_CASE( under )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0xC000;
     regs.d[1] = 0x1000;
    asm_m68k("chkw %D1, %D2");

    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.n );
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_CASE( over )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x5555;
    regs.d[1] = 0x3000;
    asm_m68k("chkw %D1, %D2");

    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.n );
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Long )

BOOST_AUTO_TEST_CASE( in_range )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x400000;    
    regs.d[1] = 0x600000;
    asm_m68k("chkl %D1, %D2");
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.pc == 2);
}


BOOST_AUTO_TEST_CASE( under )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0xC0000000;
     regs.d[1] = 0x10000000;
    asm_m68k("chkl %D1, %D2");

    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.n );
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_CASE( over )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x70000000;
    regs.d[1] = 0x30000000;
    asm_m68k("chkl %D1, %D2");

    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.n );
    BOOST_TEST( regs.pc == 0x12345678);
}



BOOST_AUTO_TEST_SUITE_END()
