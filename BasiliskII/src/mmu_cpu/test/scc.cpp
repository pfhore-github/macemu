#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_FIXTURE_TEST_CASE( t, InitFix  )
{    
    asm_m68k("st %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_FIXTURE_TEST_CASE( tmem, InitFix  )
{    
    regs.a[1] = 0x10;
    asm_m68k("st (%A1)");
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x10) == 0xff );
}

BOOST_FIXTURE_TEST_CASE( f, InitFix  )
{    
    asm_m68k("sf %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_FIXTURE_TEST_SUITE( hi, InitFix  )
BOOST_AUTO_TEST_CASE(  t)
{    
    regs.c = false;
    regs.z = false;
    asm_m68k("shi %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f1)
{    
    regs.c = true;
    regs.z = false;
    asm_m68k("shi %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_CASE( f2)
{    
    regs.c = false;
    regs.z = true;
    asm_m68k("shi %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( ls, InitFix  )
BOOST_AUTO_TEST_CASE(  t1)
{    
    regs.c = true;
    regs.z = false;
    asm_m68k("sls %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE(  t2)
{    
    regs.c = false;
    regs.z = true;
    asm_m68k("sls %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}
BOOST_AUTO_TEST_CASE( f)
{    
    regs.c = false;
    regs.z = false;
    asm_m68k("sls %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}


BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE( cc, InitFix  )
BOOST_AUTO_TEST_CASE(  t)
{    
    regs.c = false;
    asm_m68k("scc %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f)
{    
    regs.c = true;
    asm_m68k("scc %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( cs, InitFix  )
BOOST_AUTO_TEST_CASE(  t)
{    
    regs.c = true;
    asm_m68k("scs %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f)
{    
    regs.c = false;
    asm_m68k("scs %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( ne, InitFix  )
BOOST_AUTO_TEST_CASE(  t)
{    
    regs.z = false;
    asm_m68k("sne %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f)
{    
    regs.z = true;
    asm_m68k("sne %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( eq, InitFix  )
BOOST_AUTO_TEST_CASE(  t)
{    
    regs.z = true;
    asm_m68k("seq %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f)
{    
    regs.z = false;
    asm_m68k("seq %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( vc, InitFix  )
BOOST_AUTO_TEST_CASE(  t)
{    
    regs.v = false;
    asm_m68k("svc %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f)
{    
    regs.v = true;
    asm_m68k("svc %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( vs, InitFix  )
BOOST_AUTO_TEST_CASE(  t)
{    
    regs.v = true;
    asm_m68k("svs %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f)
{    
    regs.v = false;
    asm_m68k("svs %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( pl, InitFix  )
BOOST_AUTO_TEST_CASE(  t)
{    
    regs.n = false;
    asm_m68k("spl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f)
{    
    regs.n = true;
    asm_m68k("spl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( mi, InitFix  )
BOOST_AUTO_TEST_CASE(  t)
{    
    regs.n = true;
    asm_m68k("smi %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f)
{    
    regs.n = false;
    asm_m68k("smi %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( ge, InitFix  )
BOOST_AUTO_TEST_CASE(  t1)
{    
    regs.n = true;
    regs.v = true;
    asm_m68k("sge %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE(  t2)
{    
    regs.n = false;
    regs.v = false;
    asm_m68k("sge %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f1)
{    
    regs.n = true;
    regs.v = false;
    asm_m68k("sge %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_CASE( f2)
{    
    regs.n = false;
    regs.v = true;
    asm_m68k("sge %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( lt, InitFix  )
BOOST_AUTO_TEST_CASE(  t1)
{    
    regs.n = true;
    regs.v = false;
    asm_m68k("slt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE(  t2)
{    
    regs.n = false;
    regs.v = true;
    asm_m68k("slt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f1)
{    
    regs.n = true;
    regs.v = true;
    asm_m68k("slt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_CASE( f2)
{    
    regs.n = false;
    regs.v = false;
    asm_m68k("slt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( gt, InitFix  )
BOOST_AUTO_TEST_CASE(  t1)
{    
    regs.z = false;
    regs.n = true;
    regs.v = true;
    asm_m68k("sgt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE(  t2)
{    
    regs.z = false;
    regs.n = false;
    regs.v = false;
    asm_m68k("sgt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f1)
{    
    regs.z = false;
    regs.n = true;
    regs.v = false;
    asm_m68k("sgt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_CASE( f2)
{    
    regs.z = false;
    regs.n = false;
    regs.v = true;
    asm_m68k("sgt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_CASE( f3)
{    
    regs.z = true;
    asm_m68k("sgt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( le, InitFix  )
BOOST_AUTO_TEST_CASE(  t1)
{    
    regs.z = true;
    asm_m68k("sle %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
} 
BOOST_AUTO_TEST_CASE(  t2)
{    
    regs.z = false;
    regs.n = true;
    regs.v = false;
    asm_m68k("sle %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE(  t3)
{    
    regs.z = false;
    regs.n = false;
    regs.v = true;
    asm_m68k("sle %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( f1)
{    
    regs.z = false;
    regs.n = true;
    regs.v = true;
    asm_m68k("sle %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_CASE( f2)
{    
    regs.z = false;
    regs.n = false;
    regs.v = false;
    asm_m68k("sle %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}

BOOST_AUTO_TEST_SUITE_END()