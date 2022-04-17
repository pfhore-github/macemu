#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_SUITE( Byte, InitFix  )
BOOST_AUTO_TEST_CASE(  cv)
{    
    regs.d[1] = 44;
    asm_m68k("notb %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (uint8_t)(~44) );
    BOOST_TEST( !regs.c );
    BOOST_TEST( !regs.v );
}

BOOST_AUTO_TEST_CASE( n )
{    
    regs.d[1] = 1;
    asm_m68k("notb %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n );
}

BOOST_AUTO_TEST_CASE( z )
{    
    regs.d[1] = 0xff;
    asm_m68k("notb %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( Word, InitFix  )
BOOST_AUTO_TEST_CASE( cv )
{    
    regs.d[1] = 440;
    asm_m68k("notw %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (uint16_t)(~440) );
    BOOST_TEST( !regs.c );
    BOOST_TEST( !regs.v );
}

BOOST_AUTO_TEST_CASE( n )
{    
    regs.d[1] = 1;
    asm_m68k("notw %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n );
}

BOOST_AUTO_TEST_CASE( z )
{    
    regs.d[1] = 0xffff;
    asm_m68k("notw %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( Long, InitFix  )
BOOST_AUTO_TEST_CASE( cv ) {
    regs.d[1] = 440000;
    asm_m68k("notl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == ~440000);
    BOOST_TEST( !regs.c );
    BOOST_TEST( !regs.v );
}

BOOST_AUTO_TEST_CASE( n )
{    
    regs.d[1] = 1;
    asm_m68k("notl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n );
}

BOOST_AUTO_TEST_CASE( z )
{    
    regs.d[1] = 0xffffffff;
    asm_m68k("notl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z );
}

BOOST_AUTO_TEST_SUITE_END()