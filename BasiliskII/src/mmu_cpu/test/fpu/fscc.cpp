#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(FSCC, InitFix)
BOOST_AUTO_TEST_CASE( f )
{    
    asm_m68k("fsf %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
}
BOOST_DATA_TEST_CASE( eq, BIT, z )
{    
    regs.FPSR.z = z;
    asm_m68k("fseq %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (z * 0xff) );
}

BOOST_DATA_TEST_CASE( ogt, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsogt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (!(nan|z|n)) * 0xff );
}

BOOST_DATA_TEST_CASE( oge, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsoge %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (z|!(nan|n)) * 0xff );
}

BOOST_DATA_TEST_CASE( olt, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsolt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (n&!(nan|z)) * 0xff );
}

BOOST_DATA_TEST_CASE( ole, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsole %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (z|(n&!nan)) * 0xff );
}

BOOST_DATA_TEST_CASE( ogl, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsogl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == !(nan|z) * 0xff );
}

BOOST_DATA_TEST_CASE( un, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsun %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan) * 0xff );
}

BOOST_DATA_TEST_CASE( ueq, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsueq %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|z) * 0xff );
}

BOOST_DATA_TEST_CASE( ugt, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsugt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|!(n|z)) * 0xff );
}

BOOST_DATA_TEST_CASE( uge, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsuge %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|!n|z) * 0xff );
}

BOOST_DATA_TEST_CASE( ult, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsult %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|(n&!z)) * 0xff );
}

BOOST_DATA_TEST_CASE( ule, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsule %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|n|z) * 0xff );
}

BOOST_DATA_TEST_CASE( ne, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fsne %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == !z * 0xff );
}

BOOST_DATA_TEST_CASE( t, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    asm_m68k("fst %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
}

BOOST_AUTO_TEST_CASE( sf )
{    
    regs.FPSR.nan = true;
    regs.FPSR.bsun = false;
    asm_m68k("fssf %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0 );
    BOOST_TEST( regs.FPSR.bsun);
}
BOOST_DATA_TEST_CASE( seq, BIT, z )
{    
    regs.FPSR.z = z;
    regs.FPSR.bsun = false;
    regs.FPSR.nan = true;
    asm_m68k("fsseq %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (z * 0xff) );
    BOOST_TEST( regs.FPSR.bsun);
}

BOOST_DATA_TEST_CASE( gt, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsgt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (!(nan|z|n)) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( ge, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsge %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (z|!(nan|n)) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( lt, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fslt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (n&!(nan|z)) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( le, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsle %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (z|(n&!nan)) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( gl, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsgl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == !(nan|z) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( ngle, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsngle %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( ngl, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsngl %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|z) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( nle, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsnle %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|!(n|z)) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( nlt, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsnlt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|!n|z) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( nge, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsnge %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|(n&!z)) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( ngt, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsngt %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == (nan|n|z) * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( sne, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fssne %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == !z * 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}

BOOST_DATA_TEST_CASE( st, BIT * BIT * BIT, nan, z, n )
{    
    regs.FPSR.nan = nan;
    regs.FPSR.z = z;
    regs.FPSR.n = n;
    regs.FPSR.bsun = false;
    asm_m68k("fsst %D1");
    m68k_do_execute();
    BOOST_TEST( regs.d[1] == 0xff );
    BOOST_TEST( regs.FPSR.bsun == nan);
}
BOOST_AUTO_TEST_SUITE_END()