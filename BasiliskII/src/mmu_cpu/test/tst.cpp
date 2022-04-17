#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE( Byte, InitFix  )
BOOST_AUTO_TEST_CASE(  positive )
{    
    regs.d[1] = 44;
    asm_m68k("tstb %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.n );
    BOOST_TEST( ! regs.z );
    BOOST_TEST( !regs.c );
    BOOST_TEST( !regs.v );
}

BOOST_AUTO_TEST_CASE(  zero )
{    
    regs.d[1] = 0;
    asm_m68k("tstb %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.n );
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE( negative )
{    
    regs.d[1] = 0xc0;
    asm_m68k("tstb %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n );
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( Word, InitFix  )
BOOST_AUTO_TEST_CASE(  positive )
{    
    regs.d[1] = 4400;
    asm_m68k("tstw %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.n );
    BOOST_TEST( ! regs.z );
    BOOST_TEST( !regs.c );
    BOOST_TEST( !regs.v );
}

BOOST_AUTO_TEST_CASE(  zero )
{    
    regs.d[1] = 0;
    asm_m68k("tstw %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.n );
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE( negative )
{    
    regs.d[1] = 0xc000;
    asm_m68k("tstw %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n );
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( Long, InitFix  )
BOOST_AUTO_TEST_CASE(  positive )
{    
    regs.d[1] = 0x10000;
    asm_m68k("tstl %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.n );
    BOOST_TEST( ! regs.z );
    BOOST_TEST( !regs.c );
    BOOST_TEST( !regs.v );
}

BOOST_AUTO_TEST_CASE(  zero )
{    
    regs.d[1] = 0;
    asm_m68k("tstl %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.n );
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE( negative )
{    
    regs.d[1] = 0xc0000000;
    asm_m68k("tstl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n );
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_SUITE_END()