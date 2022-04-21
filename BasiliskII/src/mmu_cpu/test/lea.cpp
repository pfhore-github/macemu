#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( an, InitFix )
{    
    regs.a[2] = 0x10;
    asm_m68k("lea (%A2), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x10 );
}


BOOST_FIXTURE_TEST_CASE( d16, InitFix )
{    
    regs.a[2] = 0x1000;
    asm_m68k("lea (160, %A2), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+160 );
}
BOOST_FIXTURE_TEST_SUITE( xn , InitFix )
BOOST_AUTO_TEST_CASE( w_1 )
{    
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("lea (20, %A2, %D3.W*1), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+20-4 );
}

BOOST_AUTO_TEST_CASE( w_2)
{    
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("lea (20, %A2, %D3.W*2), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+20-4*2 );
}

BOOST_AUTO_TEST_CASE( w_4 )
{    
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("lea (20, %A2, %D3.W*4), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+20-4*4 );
}

BOOST_AUTO_TEST_CASE( w_8 )
{    
    regs.d[3] = (-4)&0xffff;
    regs.a[2] = 0x1000;
    asm_m68k("lea (20, %A2, %D3.W*8), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+20-4*8 );
}

BOOST_AUTO_TEST_CASE( l_1 )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("lea (20, %A2, %D3.L*1), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+20+4 );
}

BOOST_AUTO_TEST_CASE( l_2 )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("lea (20, %A2, %D3.L*2), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+20+4*2 );
}

BOOST_AUTO_TEST_CASE( l_4 )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("lea (20, %A2, %D3.L*4), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+20+4*4 );
}

BOOST_AUTO_TEST_CASE( l_8 )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("lea (20, %A2, %D3.L*8), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+20+4*8 );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_CASE( bdw, InitFix  )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    asm_m68k("lea (300, %A2, %D3*1), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x1000+300+4 );
}

BOOST_FIXTURE_TEST_CASE( bdl, InitFix  )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x10000;
    asm_m68k("lea (70000, %A2, %D3*1), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x10000+70000+4 );
}

BOOST_FIXTURE_TEST_CASE( post_od_none , InitFix )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    asm_m68k("lea ([30, %A2], %D3*1), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2004 );
}

BOOST_FIXTURE_TEST_CASE( post_odw , InitFix )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    asm_m68k("lea ([30, %A2], %D3*1, 40), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2004 + 40 );
}

BOOST_FIXTURE_TEST_CASE( post_odl, InitFix  )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30, 0x2000);
    asm_m68k("lea ([30, %A2], %D3*1, 0x10000), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2004+0x10000 );
}

BOOST_FIXTURE_TEST_CASE( pre_od_none, InitFix  )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30+4, 0x2000);
    asm_m68k("lea ([30, %A2, %D3*1]), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2000 );
}

BOOST_FIXTURE_TEST_CASE( pre_odw, InitFix  )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+30+4, 0x2000);
    asm_m68k("lea ([30, %A2, %D3*1], 40), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2000+40 );
}

BOOST_FIXTURE_TEST_CASE( pre_odl, InitFix  )
{    
    regs.d[3] = 4;
    regs.a[2] = 0x1000;
    raw_write32(0x1000+34, 0x2000);
    asm_m68k("lea ([30, %A2, %D3*1], 0x10000), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2000+0x10000 );
}

BOOST_FIXTURE_TEST_CASE( immw, InitFix  )
{    
    asm_m68k("lea (160).W, %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 160 );
}

BOOST_FIXTURE_TEST_CASE( imml, InitFix  )
{    
    asm_m68k("lea (0x10000).L, %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] = 0x10000  );
}


BOOST_FIXTURE_TEST_CASE( pc_d16, InitFix  )
{    
    asm_m68k("lea (160, %PC), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 2+160 );
}

BOOST_FIXTURE_TEST_CASE( pc_xn, InitFix  )
{    
    regs.d[3] = 4;
    asm_m68k("lea (20, %PC, %D3.W*1), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 2+20+4 );
}

BOOST_FIXTURE_TEST_CASE( pc_bdw, InitFix  )
{    
    regs.d[3] = 4;
    asm_m68k("lea (300, %PC, %D3*1), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 2+300+4 );
}

BOOST_FIXTURE_TEST_CASE( pc_bdl, InitFix  )
{    
    regs.d[3] = 4;
    asm_m68k("lea (70000, %PC, %D3*1), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 2+70000+4 );
}

BOOST_FIXTURE_TEST_CASE( pc_post_od_none, InitFix  )
{    
    regs.d[3] = 4;
    raw_write32(2+30, 0x2000);
    asm_m68k("lea ([30, %PC], %D3*1), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2004 );
}

BOOST_FIXTURE_TEST_CASE( pc_post_odw, InitFix  )
{    
    regs.d[3] = 4;
    raw_write32(2+30, 0x2000);
    asm_m68k("lea ([30, %PC], %D3*1, 40), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2004 + 40 );
}

BOOST_FIXTURE_TEST_CASE( pc_post_odl, InitFix  )
{    
    regs.d[3] = 4;
    raw_write32(2+30, 0x2000);
    asm_m68k("lea ([30, %PC], %D3*1, 0x10000), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2004+0x10000 );
}

BOOST_FIXTURE_TEST_CASE( pc_pre_od_none, InitFix  )
{    
    regs.d[3] = 4;
    raw_write32(2+30+4, 0x2000);
    asm_m68k("lea ([30, %PC, %D3*1]), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2000 );
}

BOOST_FIXTURE_TEST_CASE( pc_pre_odw, InitFix  )
{    
    regs.d[3] = 4;
    raw_write32(2+30+4, 0x2000);
    asm_m68k("lea ([30, %PC, %D3*1], 40), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2000 + 40 );
}

BOOST_FIXTURE_TEST_CASE( pc_pre_odl, InitFix  )
{    
    regs.d[3] = 4;
    raw_write32(2+34, 0x2000);
    asm_m68k("lea ([30, %PC, %D3*1], 0x10000), %A4");
    m68k_do_execute();
    BOOST_TEST( regs.a[4] == 0x2000+0x10000 );
}


