#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_AUTO_TEST_SUITE( Byte )
BOOST_AUTO_TEST_CASE( value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12345600 | 33;
    
    asm_m68k("addib #25, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0x12345600 | (33+25) );
}

BOOST_AUTO_TEST_CASE( value_m )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    
    asm_m68k("addib #25, (%A1)");
    raw_write8(0x10, 66);
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == 66+25 );
}


BOOST_AUTO_TEST_CASE( v1 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 127;
    regs.v = false;
    
    asm_m68k("addib #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.v  );
}

BOOST_AUTO_TEST_CASE( v2 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = -128;
    regs.v = false;
    
    asm_m68k("addib #-1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.v  );
}

BOOST_AUTO_TEST_CASE( c )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0xff;
    regs.c = false;
    
    asm_m68k("addib #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.c );
}

BOOST_DATA_TEST_CASE( 
    n, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12345640 | value << 7;
    
    asm_m68k("addib #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n == value );
}

BOOST_DATA_TEST_CASE( 
    z, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x123456fe | value;
    
    asm_m68k("addib #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z == value );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Word )

BOOST_AUTO_TEST_CASE( value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12340000 | 3456;
    
    asm_m68k("addiw #1000, %D1");

    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (0x12340000 | 4456));
}

BOOST_AUTO_TEST_CASE( value_m )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    
    asm_m68k("addiw #1000, (%A1)");
    raw_write16(0x10, 3456);
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 4456 );
}


BOOST_AUTO_TEST_CASE( v1 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x7fff;
    regs.v = false;
    
    asm_m68k("addiw #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.v  );
}

BOOST_AUTO_TEST_CASE( v2 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x8000;
    regs.v = false;
    
    asm_m68k("addiw #-1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.v  );
}

BOOST_AUTO_TEST_CASE( c )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0xffff;
    regs.c = false;
    
    asm_m68k("addiw #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.c );
}



BOOST_DATA_TEST_CASE( 
    n, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x12340400 | value << 15;
    
    asm_m68k("addiw #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.n == value );
}

BOOST_DATA_TEST_CASE( 
    z, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x1234fffe | value;
    
    asm_m68k("addiw #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z == value );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Long )

BOOST_AUTO_TEST_CASE( value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 123456789;
    
    asm_m68k("addil #100000000, %D1");

    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 223456789 );
}

BOOST_AUTO_TEST_CASE( value_m )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    
    asm_m68k("addil #100000000, (%A1)");
    raw_write32(0x10, 123456789);
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == 223456789 );
}


BOOST_AUTO_TEST_CASE( v1 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x7fffffff;
    regs.v = false;
    
    asm_m68k("addil #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.v  );
}

BOOST_AUTO_TEST_CASE( v2 )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0x80000000;
    regs.v = false;
    
    asm_m68k("addil #-1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.v  );
}

BOOST_AUTO_TEST_CASE( c )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0xffffffff;
    regs.c = false;
    
    asm_m68k("addil #1, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.c );
}


BOOST_DATA_TEST_CASE( 
    n, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = value << 31 | 0x04000000;
    asm_m68k("addil #1, %D1");    
    m68k_do_execute();
    BOOST_TEST( regs.n == value );
}

BOOST_DATA_TEST_CASE( 
    z, 
boost::unit_test::data::xrange(1), value )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = 0xfffffffe | value;
    asm_m68k("addil #1, %D1");    
    m68k_do_execute();
    BOOST_TEST( regs.z == value );
}

BOOST_AUTO_TEST_SUITE_END()
