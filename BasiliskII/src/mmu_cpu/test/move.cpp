#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE( Byte, InitFix  )

BOOST_DATA_TEST_CASE( 
    n, 
boost::unit_test::data::xrange(1), value )
{    
    regs.d[1] = value << 7;
    asm_m68k("move.b %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.n == value );
}

BOOST_DATA_TEST_CASE( 
    z, 
boost::unit_test::data::xrange(1), value )
{    
    regs.d[1] = !value;
    asm_m68k("move.b %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.z == value);
}
BOOST_AUTO_TEST_SUITE( to )
BOOST_AUTO_TEST_CASE( reg)
{    
    regs.d[1] = 0x30;
    regs.d[2] = 0x12345678;
    asm_m68k("move.b %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0x12345630 );
}


BOOST_AUTO_TEST_CASE( address )
{    
    regs.d[1] = 0xff;
    regs.a[2] = 0x10;
    asm_m68k("move.b %D1, (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == 0xff );
    BOOST_TEST( regs.n );
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_CASE( inc )
{    
    regs.d[1] = 0xff;
    regs.a[2] = 0x10;
    asm_m68k("move.b %D1, (%A2)+");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == 0xff );
    BOOST_TEST( regs.a[2] == 0x11 );
}

BOOST_AUTO_TEST_CASE( dec )
{    
    regs.d[1] = 0xff;
    regs.a[2] = 0x11;
    asm_m68k("move.b %D1, -(%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == 0xff );
    BOOST_TEST( regs.a[2] == 0x10 );
}

BOOST_AUTO_TEST_CASE( d16 )
{    
    regs.d[1] = 0xff;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (160, %A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+160) == 0xff );
}
BOOST_AUTO_TEST_SUITE( xn )
BOOST_AUTO_TEST_CASE( w_1 )
{    
    regs.d[1] = 0xff;
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (20, %A2, %D3.W*1)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+20-4) == 0xff );
}

BOOST_AUTO_TEST_CASE( w_2)
{    
    regs.d[1] = 0xff;
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (20, %A2, %D3.W*2)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+20-4*2) == 0xff );
}

BOOST_AUTO_TEST_CASE( w_4 )
{    
    regs.d[1] = 0xff;
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (20, %A2, %D3.W*4)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+20-4*4) == 0xff );
}

BOOST_AUTO_TEST_CASE( w_8 )
{    
    regs.d[1] = 0xff;
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (20, %A2, %D3.W*8)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+20-4*8) == 0xff );
}

BOOST_AUTO_TEST_CASE( l_1 )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (20, %A2, %D3.L*1)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+20+4) == 0xff );
}

BOOST_AUTO_TEST_CASE( l_2 )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (20, %A2, %D3.L*2)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+20+4*2) == 0xff );
}

BOOST_AUTO_TEST_CASE( l_4 )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (20, %A2, %D3.L*4)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+20+4*4) == 0xff );
}

BOOST_AUTO_TEST_CASE( l_8 )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (20, %A2, %D3.L*8)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+20+4*8) == 0xff );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE( bdw )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("move.b %D1, (300, %A2, %D3*1)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x1000+300+4) == 0xff );
}

BOOST_AUTO_TEST_CASE( bdl )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x10000;
    asm_m68k("move.b %D1, (70000, %A2, %D3*1)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10000+70000+4) == 0xff );
}

BOOST_AUTO_TEST_CASE( post_od_none )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    asm_m68k("move.b %D1, ([30, %A2], %D3*1)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x2004) == 0xff );
}

BOOST_AUTO_TEST_CASE( post_odw )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    asm_m68k("move.b %D1, ([30, %A2], %D3*1, 40)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x2004+40) == 0xff );
}

BOOST_AUTO_TEST_CASE( post_odl )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    asm_m68k("move.b %D1, ([30, %A2], %D3*1, 0x10000)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x2004+0x10000) == 0xff );
}

BOOST_AUTO_TEST_CASE( pre_od_none )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30+4, 0x2000);
    asm_m68k("move.b %D1, ([30, %A2, %D3*1])");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x2000) == 0xff );
}

BOOST_AUTO_TEST_CASE( pre_odw )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30+4, 0x2000);
    asm_m68k("move.b %D1, ([30, %A2, %D3*1], 40)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x2000+40) == 0xff );
}

BOOST_AUTO_TEST_CASE( pre_odl )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+34, 0x2000);
    asm_m68k("move.b %D1, ([30, %A2, %D3*1], 0x10000)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x2000+0x10000) == 0xff );
}

BOOST_AUTO_TEST_CASE( immw )
{    
    regs.d[1] = 0xff;
    asm_m68k("move.b %D1, (160).W");
    m68k_do_execute();
    BOOST_TEST( raw_read8(160) == 0xff );
}

BOOST_AUTO_TEST_CASE( imml )
{    
    regs.d[1] = 0xff;
    asm_m68k("move.b %D1, (0x10000).L");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10000) == 0xff );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( from )
BOOST_AUTO_TEST_CASE( address )
{    
    regs.a[2] = 0x10;
    raw_write8(0x10, 0xff);
    asm_m68k("move.b (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( inc )
{    
    raw_write8(0x10, 0xff);
    regs.a[2] = 0x10;
    asm_m68k("move.b (%A2)+, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0x11 );
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( dec )
{    
    raw_write8(0x10, 0xff);
    regs.a[2] = 0x11;
    asm_m68k("move.b -(%A2), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0x10 );
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( d16 )
{    
    regs.a[2] = 0x1000;
    raw_write8(0x1000+160, 0xff);
    asm_m68k("move.b (160, %A2), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( xn )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write8(0x1000+20+4, 0xff);
    asm_m68k("move.b (20, %A2, %D3.W*1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( bdw )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write8(0x1000+300+4, 0xff);
    asm_m68k("move.b (300, %A2, %D3*1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}


BOOST_AUTO_TEST_CASE( bdl )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x10000;
    raw_write8(0x10000+70000+4, 0xff);
    asm_m68k("move.b (70000, %A2, %D3*1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( post_od_none )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    raw_write8(0x2004, 0xff);
    asm_m68k("move.b ([30, %A2], %D3*1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}


BOOST_AUTO_TEST_CASE( post_odw )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    raw_write8(0x2004+40, 0xff);
    asm_m68k("move.b ([30, %A2], %D3*1, 40), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( post_odl )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    raw_write8(0x2004+0x10000, 0xff);
    asm_m68k("move.b ([30, %A2], %D3*1, 0x10000), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pre_od_none )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30+4, 0x2000);
    raw_write8(0x2000, 0xff);
    asm_m68k("move.b ([30, %A2, %D3*1]), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pre_odw )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30+4, 0x2000);
    raw_write8(0x2000+40, 0xff);
    asm_m68k("move.b ([30, %A2, %D3*1], 40), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pre_odl )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+34, 0x2000);
    raw_write8(0x2000+0x10000, 0xff );
    asm_m68k("move.b ([30, %A2, %D3*1], 0x10000), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_d16 )
{    
    raw_write8(2+160, 0xff);
    asm_m68k("move.b (160, %PC), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_xn )
{    
    regs.d[1] = 0xff;
    regs.d[3] = 4;
    raw_write8(2+20+4, 0xff);
    asm_m68k("move.b (20, %PC, %D3.W*1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_bdw )
{    
    regs.d[3] = 4;
    raw_write8(2+300+4, 0xff);
    asm_m68k("move.b (300, %PC, %D3*1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_bdl )
{    
    regs.d[3] = 4;
    raw_write8(2+70000+4, 0xff);
    asm_m68k("move.b (70000, %PC, %D3*1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_post_od_none )
{    
    regs.d[3] = 4;
    raw_write32(2+30, 0x2000);
    raw_write8(0x2004, 0xff);
    asm_m68k("move.b ([30, %PC], %D3*1), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_post_odw )
{    
    regs.d[3] = 4;
    raw_write32(2+30, 0x2000);
    raw_write8(0x2004+40, 0xff);
    asm_m68k("move.b ([30, %PC], %D3*1, 40), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_post_odl )
{    
    regs.d[3] = 4;
    raw_write32(2+30, 0x2000);
    raw_write8(0x2004+0x10000, 0xff);
    asm_m68k("move.b ([30, %PC], %D3*1, 0x10000), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_pre_od_none )
{    
    regs.d[3] = 4;
    raw_write32(2+30+4, 0x2000);
    raw_write8(0x2000, 0xff);
    asm_m68k("move.b ([30, %PC, %D3*1]), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_pre_odw )
{    
    regs.d[3] = 4;
    raw_write32(2+30+4, 0x2000);
    raw_write8(0x2000+40, 0xff);
    asm_m68k("move.b ([30, %PC, %D3*1], 40), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( pc_pre_odl )
{    
    regs.d[3] = 4;
    raw_write32(2+34, 0x2000);
    raw_write8(0x2000+0x10000, 0xff );
    asm_m68k("move.b ([30, %PC, %D3*1], 0x10000), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( immw )
{    
    raw_write8(160, 0xff);
    asm_m68k("move.b (160).W, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( imml )
{    
    raw_write8(0x10000, 0xff);
    asm_m68k("move.b (0x10000).L, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( imm )
{    
    asm_m68k("move.b #200, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 200 );
}



BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE( Word, InitFix  )

BOOST_DATA_TEST_CASE( 
    n, 
boost::unit_test::data::xrange(1), value )
{    
    regs.d[1] = value << 15;
    asm_m68k("move.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.n == value );
}

BOOST_DATA_TEST_CASE( 
    z, 
boost::unit_test::data::xrange(1), value )
{    
    regs.d[1] = !value;
    asm_m68k("move.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.z == value);
}
BOOST_AUTO_TEST_SUITE( to )
BOOST_AUTO_TEST_CASE( reg)
{    
    regs.d[1] = 0x3040;
    regs.d[2] = 0x12345678;
    asm_m68k("move.w %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0x12343040 );
}


BOOST_AUTO_TEST_CASE( address )
{    
    regs.d[1] = 0xffff;
    regs.a[2] = 0x10;
    asm_m68k("move.w %D1, (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 0xffff );
    BOOST_TEST( regs.n );
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_CASE( inc )
{    
    regs.d[1] = 0xffff;
    regs.a[2] = 0x10;
    asm_m68k("move.w %D1, (%A2)+");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 0xffff );
    BOOST_TEST( regs.a[2] == 0x12 );
}

BOOST_AUTO_TEST_CASE( dec )
{    
    regs.d[1] = 0xffff;
    regs.a[2] = 0x12;
    asm_m68k("move.w %D1, -(%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x10) == 0xffff );
    BOOST_TEST( regs.a[2] == 0x10 );
}

BOOST_AUTO_TEST_CASE( ccr )
{    
    regs.d[1] = 0xff;
    asm_m68k("move.w %D1, %CCR");
    m68k_do_execute();
    BOOST_TEST( regs.v );
    BOOST_TEST( regs.n );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.z );
    BOOST_TEST( regs.x );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( from )
BOOST_AUTO_TEST_CASE( address )
{    
    regs.a[2] = 0x10;
    raw_write16(0x10, 0xffff);
    asm_m68k("move.w (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xffff );
}

BOOST_AUTO_TEST_CASE( inc )
{    
    raw_write16(0x10, 0xffff);
    regs.a[2] = 0x10;
    asm_m68k("move.w (%A2)+, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0x12 );
    BOOST_TEST( regs.d[1] == 0xffff );
}

BOOST_AUTO_TEST_CASE( dec )
{    
    raw_write16(0x10, 0xffff);
    regs.a[2] = 0x12;
    asm_m68k("move.w -(%A2), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0x10 );
    BOOST_TEST( regs.d[1] == 0xffff );
}

BOOST_AUTO_TEST_CASE( imm )
{    
    asm_m68k("move.w #2000, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 2000 );
}

BOOST_AUTO_TEST_CASE( ccr )
{    
    regs.v = regs.x = regs.c = regs.z = regs.n = true;
    asm_m68k("move.w %CCR, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] = 0x1f );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_FIXTURE_TEST_SUITE( Long, InitFix  )

BOOST_DATA_TEST_CASE( 
    n, 
boost::unit_test::data::xrange(1), value )
{    
    regs.d[1] = value << 31;
    asm_m68k("move.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.n == value );
}

BOOST_DATA_TEST_CASE( 
    z, 
boost::unit_test::data::xrange(1), value )
{    
    regs.d[1] = !value;
    asm_m68k("move.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.z == value);
}
BOOST_AUTO_TEST_SUITE( to )
BOOST_AUTO_TEST_CASE( reg)
{    
    regs.d[1] = 0x12345678;
    asm_m68k("move.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0x12345678 );
}


BOOST_AUTO_TEST_CASE( address )
{    
    regs.d[1] = 0xffffffff;
    regs.a[2] = 0x10;
    asm_m68k("move.l %D1, (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == 0xffffffff );
    BOOST_TEST( regs.n );
    BOOST_TEST( ! regs.z );
}

BOOST_AUTO_TEST_CASE( inc )
{    
    regs.d[1] = 0xffffffff;
    regs.a[2] = 0x10;
    asm_m68k("move.l %D1, (%A2)+");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == 0xffffffff );
    BOOST_TEST( regs.a[2] == 0x14 );
}

BOOST_AUTO_TEST_CASE( dec )
{    
    regs.d[1] = 0xffffffff;
    regs.a[2] = 0x14;
    asm_m68k("move.l %D1, -(%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x10) == 0xffffffff );
    BOOST_TEST( regs.a[2] == 0x10 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( from )
BOOST_AUTO_TEST_CASE( address )
{    
    regs.a[2] = 0x10;
    raw_write32(0x10, 0xffffffff);
    asm_m68k("move.l (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xffffffff );
}

BOOST_AUTO_TEST_CASE( inc )
{    
    raw_write32(0x10, 0xffffffff);
    regs.a[2] = 0x10;
    asm_m68k("move.l (%A2)+, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0x14 );
    BOOST_TEST( regs.d[1] == 0xffffffff );
}

BOOST_AUTO_TEST_CASE( dec )
{    
    raw_write32(0x10, 0xffffffff);
    regs.a[2] = 0x14;
    asm_m68k("move.l -(%A2), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.a[2] == 0x10 );
    BOOST_TEST( regs.d[1] == 0xffffffff );
}

BOOST_AUTO_TEST_CASE( imm )
{    
    asm_m68k("move.l #0x10000, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0x10000 );
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
