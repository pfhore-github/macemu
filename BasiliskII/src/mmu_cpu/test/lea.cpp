#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( an, InitFix )
{    
    regs.a[7] = 0x104;
    regs.a[2] = 0x10;
    asm_m68k("pea (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x10 );
    BOOST_TEST( regs.a[7] == 0x100 );
}


BOOST_FIXTURE_TEST_CASE( d16, InitFix )
{    
    regs.a[7] = 0x104;
    regs.a[2] = 0x1000;
    asm_m68k("pea (160, %A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+160 );
}
BOOST_FIXTURE_TEST_SUITE( xn , InitFix )
BOOST_AUTO_TEST_CASE( w_1 )
{    
   regs.a[7] = 0x104;
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("pea (20, %A2, %D3.W*1)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+20-4 );
}

BOOST_AUTO_TEST_CASE( w_2)
{    
   regs.a[7] = 0x104;
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("pea (20, %A2, %D3.W*2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+20-4*2 );
}

BOOST_AUTO_TEST_CASE( w_4 )
{    
   regs.a[7] = 0x104;
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("pea (20, %A2, %D3.W*4)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+20-4*4 );
}

BOOST_AUTO_TEST_CASE( w_8 )
{    
   regs.a[7] = 0x104;
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("pea (20, %A2, %D3.W*8)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+20-4*8 );
}

BOOST_AUTO_TEST_CASE( l_1 )
{    
   regs.a[7] = 0x104;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("pea (20, %A2, %D3.L*1)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+20+4 );
}

BOOST_AUTO_TEST_CASE( l_2 )
{    
   regs.a[7] = 0x104;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("pea (20, %A2, %D3.L*2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+20+4*2 );
}

BOOST_AUTO_TEST_CASE( l_4 )
{    
   regs.a[7] = 0x104;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("pea (20, %A2, %D3.L*4)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+20+4*4 );
}

BOOST_AUTO_TEST_CASE( l_8 )
{    
   regs.a[7] = 0x104;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("pea (20, %A2, %D3.L*8)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+20+4*8) );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE( bdw )
{    
   regs.a[7] = 0x104;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("pea (300, %A2, %D3*1)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x1000+300+4 );
}

BOOST_AUTO_TEST_CASE( bdl )
{    
    regs.a[7] = 0x104;
    regs.d[3] = 4;
    regs.a[2] = 0x10000;
    asm_m68k("pea (70000, %A2, %D3*1)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x100) == 0x10000+70000+4 );
}

BOOST_AUTO_TEST_CASE( post_od_none )
{    
    regs.a[7] = 0x104;
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    asm_m68k("pea ([30, %A2], %D3*1)");
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

BOOST_AUTO_TEST_SUITE_END()

