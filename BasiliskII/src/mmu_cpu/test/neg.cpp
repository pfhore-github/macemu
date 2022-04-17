#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE( Byte, InitFix  )
BOOST_AUTO_TEST_CASE(  cx)
{    
    regs.d[1] = 44;
    asm_m68k("negb %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 256 - 44 );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.x );
}

BOOST_AUTO_TEST_CASE( v )
{    
    regs.d[1] = 0x80;
    asm_m68k("negb %D1");
    m68k_do_execute();
    BOOST_TEST( regs.v );
}

BOOST_AUTO_TEST_CASE( z )
{    
    regs.d[1] = 0;
    asm_m68k("negb %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_CASE( n )
{    
    regs.d[1] = 3;
    asm_m68k("negb %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( Word, InitFix  )
BOOST_AUTO_TEST_CASE( cx )
{    
    regs.d[1] = 440;
    asm_m68k("negw %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 65536 - 440  );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.x );
}

BOOST_AUTO_TEST_CASE( v )
{    
    regs.d[1] = 0x8000;
    asm_m68k("negw %D1");
    m68k_do_execute();
    BOOST_TEST( regs.v );
}

BOOST_AUTO_TEST_CASE( z )
{    
    regs.d[1] = 10;
    asm_m68k("negw %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_CASE( n )
{    
    regs.d[1] = 3;
    asm_m68k("negw %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( Long, InitFix  )
BOOST_AUTO_TEST_CASE( cx ) {
    regs.d[1] = 440000;
    asm_m68k("negl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xffffffff - 440000 + 1);
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.x );
}

BOOST_AUTO_TEST_CASE( v )
{    
    regs.d[1] = 0x80000000;
    asm_m68k("negl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.v );
}

BOOST_AUTO_TEST_CASE( z )
{    
    regs.d[1] = 10;
    asm_m68k("negl %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_CASE( n )
{    
    regs.d[1] = 3;
    asm_m68k("negl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n );
}

BOOST_AUTO_TEST_SUITE_END()