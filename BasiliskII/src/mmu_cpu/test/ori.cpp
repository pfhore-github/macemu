#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
void m68k_do_execute ();
extern std::vector<uint8_t> RAM;

BOOST_AUTO_TEST_SUITE( Byte )
BOOST_AUTO_TEST_CASE( value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12345678;
    
    asm_m68k("orib #0x2a, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12345678 | 0x2a) );
}

BOOST_AUTO_TEST_CASE( value_m )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    
    asm_m68k("orib #0x2a, (%A1)");
    raw_write8(0x10, 0x12);
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == (0x12 | 0x2a) );
}

BOOST_AUTO_TEST_CASE( vcx )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.v = regs.c = regs.x = true;
    
    asm_m68k("orib #0x2a, %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.v );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.x );
}

BOOST_DATA_TEST_CASE( 
    n, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12345600 |  value << 7;
    
    asm_m68k("orib #0, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n == value );
}

BOOST_DATA_TEST_CASE( 
    z, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12345600 | !value;
    
    asm_m68k("orib #0, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z == value );
}

BOOST_AUTO_TEST_CASE( ccr )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.x = regs.v = regs.c = regs.n = regs.z = false;
    
    asm_m68k("orib #0xff, %CCR");

    m68k_do_execute();
    BOOST_TEST( regs.x == true );
    BOOST_TEST( regs.v == true );
    BOOST_TEST( regs.c == true );
    BOOST_TEST( regs.n == true );
    BOOST_TEST( regs.z == true );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Word )
BOOST_AUTO_TEST_CASE( value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12345678;
    
    asm_m68k("oriw #0xdead, %D1");

    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12345678 | 0xdead) );
}

BOOST_AUTO_TEST_CASE( value_m )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    
    asm_m68k("oriw #0x2abc, (%A1)");
    raw_write16(0x10, 0x1234);
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == (0x1234 | 0x2abc) );
}
BOOST_AUTO_TEST_CASE( vcx )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.v = regs.c = regs.x = true;
    
    asm_m68k("oriw #0x2abc, (%A1)");
    m68k_do_execute();
    BOOST_TEST( ! regs.v );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.x );
}

BOOST_DATA_TEST_CASE( 
    n, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12340000 | value << 15;
    
    asm_m68k("oriw #0, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n == value );
}

BOOST_DATA_TEST_CASE( 
    z, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12340000 | ! value;
    
    asm_m68k("oriw #0, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z == value );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Long )
BOOST_AUTO_TEST_CASE( value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12345678;
    
    asm_m68k("oril #0xdeadbeaf, %D1");

    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12345678 | 0xdeadbeaf) );
}

BOOST_AUTO_TEST_CASE( value_m )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    
    asm_m68k("oril #0x2abc5412, (%A1)");
    raw_write32(0x10, 0x1234dead);
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == (0x2abc5412 | 0x1234dead) );
}
BOOST_AUTO_TEST_CASE( vcx )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.v = regs.c = regs.x = true;
    
    asm_m68k("oril #0xdeadbeaf, %D1");
    m68k_do_execute();
    BOOST_TEST( ! regs.v );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.x );
}

BOOST_DATA_TEST_CASE( 
    n, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = value << 31;
    
    asm_m68k("oril #0, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n == value );
}

BOOST_DATA_TEST_CASE( 
    z, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = ! value;
    
    asm_m68k("oril #0, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z == value );
}
BOOST_AUTO_TEST_SUITE_END()

