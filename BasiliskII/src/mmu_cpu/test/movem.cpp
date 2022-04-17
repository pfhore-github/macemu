#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE( Word, InitFix )
BOOST_FIXTURE_TEST_SUITE( To, InitFix )
BOOST_FIXTURE_TEST_CASE( addr, InitFix )
{    
    regs.d[1] = 0x1000;
    regs.d[2] = 0x2000;
    regs.d[3] = 0x3000;
    regs.a[1] = 0x4000;
    regs.a[2] = 0x5000;
    regs.a[3] = 0x6000;
    regs.in_movem = -1;
    asm_m68k("movem.w %D1-%D3/%A1-%A3, (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x5000) == 0x1000 );
    BOOST_TEST( raw_read16(0x5002) == 0x2000 );
    BOOST_TEST( raw_read16(0x5004) == 0x3000 );
    BOOST_TEST( raw_read16(0x5006) == 0x4000 );
    BOOST_TEST( raw_read16(0x5008) == 0x5000 );
    BOOST_TEST( raw_read16(0x500A) == 0x6000 );
}

BOOST_FIXTURE_TEST_CASE( restart_addr, InitFix )
{    
    regs.d[1] = 0x1000;
    regs.d[2] = 0x2000;
    regs.d[3] = 0x3000;
    regs.a[1] = 0x4000;
    regs.a[2] = 0x5000;
    regs.a[3] = 0x6000;
    regs.in_movem = 3;
    regs.i_ea = 0x5004;
    regs.i_eav = 0x5000;
    asm_m68k("movem.w %D1-%D3/%A1-%A3, (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x5004) == 0x3000 );
    BOOST_TEST( raw_read16(0x5006) == 0x4000 );
    BOOST_TEST( raw_read16(0x5008) == 0x5000 );
    BOOST_TEST( raw_read16(0x500A) == 0x6000 );
    BOOST_TEST( regs.in_movem == -1);
}

BOOST_FIXTURE_TEST_CASE( decr, InitFix )
{    
    regs.d[1] = 0x1000;
    regs.d[2] = 0x2000;
    regs.d[3] = 0x3000;
    regs.a[1] = 0x4000;
    regs.a[2] = 0x500C;
    regs.a[3] = 0x6000;
    asm_m68k("movem.w %D1-%D3/%A1-%A3, -(%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x500A) == 0x6000 );
    BOOST_TEST( raw_read16(0x5008) == 0x500A );
    BOOST_TEST( raw_read16(0x5006) == 0x4000 );
    BOOST_TEST( raw_read16(0x5004) == 0x3000 );
    BOOST_TEST( raw_read16(0x5002) == 0x2000 );
    BOOST_TEST( raw_read16(0x5000) == 0x1000 );
    BOOST_TEST( regs.a[2] == 0x5000);
}

BOOST_FIXTURE_TEST_CASE( restart_decr, InitFix )
{    
    regs.d[1] = 0x1000;
    regs.d[2] = 0x2000;
    regs.d[3] = 0x3000;
    regs.a[1] = 0x4000;
    regs.a[2] = 0x500A;
    regs.a[3] = 0x6000;
    regs.in_movem = 10;
    regs.i_ea = 0x500A;
    regs.i_eav = 0x500A;
    asm_m68k("movem.w %D1-%D3/%A1-%A3, -(%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read16(0x5008) == 0x500A );
    BOOST_TEST( raw_read16(0x5006) == 0x4000 );
    BOOST_TEST( raw_read16(0x5004) == 0x3000 );
    BOOST_TEST( raw_read16(0x5002) == 0x2000 );
    BOOST_TEST( raw_read16(0x5000) == 0x1000 );
    BOOST_TEST( regs.a[2] == 0x5000);
    BOOST_TEST( regs.in_movem == -1);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( Long, InitFix )
BOOST_FIXTURE_TEST_SUITE( To, InitFix )
BOOST_FIXTURE_TEST_CASE( addr, InitFix )
{    
    regs.d[1] = 0x100000;
    regs.d[2] = 0x200000;
    regs.d[3] = 0x300000;
    regs.a[1] = 0x400000;
    regs.a[2] = 0x5000;
    regs.a[3] = 0x600000;
    regs.in_movem = -1;
    asm_m68k("movem.l %D1-%D3/%A1-%A3, (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x5000) == 0x100000 );
    BOOST_TEST( raw_read32(0x5004) == 0x200000 );
    BOOST_TEST( raw_read32(0x5008) == 0x300000 );
    BOOST_TEST( raw_read32(0x500C) == 0x400000 );
    BOOST_TEST( raw_read32(0x5010) == 0x5000 );
    BOOST_TEST( raw_read32(0x5014) == 0x600000 );
}

BOOST_FIXTURE_TEST_CASE( restart_addr, InitFix )
{    
    regs.d[1] = 0x100000;
    regs.d[2] = 0x200000;
    regs.d[3] = 0x300000;
    regs.a[1] = 0x400000;
    regs.a[2] = 0x5000;
    regs.a[3] = 0x600000;
    regs.in_movem = 3;
    regs.i_ea = 0x5008;
    regs.i_eav = 0x5000;
    asm_m68k("movem.l %D1-%D3/%A1-%A3, (%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x5008) == 0x300000 );
    BOOST_TEST( raw_read32(0x500C) == 0x400000 );
    BOOST_TEST( raw_read32(0x5010) == 0x5000 );
    BOOST_TEST( raw_read32(0x5014) == 0x600000 );
    BOOST_TEST( regs.in_movem == -1);
}

BOOST_FIXTURE_TEST_CASE( decr, InitFix )
{    
    regs.d[1] = 0x100000;
    regs.d[2] = 0x200000;
    regs.d[3] = 0x300000;
    regs.a[1] = 0x400000;
    regs.a[2] = 0x5018;
    regs.a[3] = 0x600000;
    asm_m68k("movem.l %D1-%D3/%A1-%A3, -(%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x5014) == 0x600000 );
    BOOST_TEST( raw_read32(0x5010) == 0x5014 );
    BOOST_TEST( raw_read32(0x500C) == 0x400000 );
    BOOST_TEST( raw_read32(0x5008) == 0x300000 );
    BOOST_TEST( raw_read32(0x5004) == 0x200000 );
    BOOST_TEST( raw_read32(0x5000) == 0x100000 );
    BOOST_TEST( regs.a[2] == 0x5000);
}

BOOST_FIXTURE_TEST_CASE( restart_decr, InitFix )
{    
    regs.d[1] = 0x100000;
    regs.d[2] = 0x200000;
    regs.d[3] = 0x300000;
    regs.a[1] = 0x400000;
    regs.a[2] = 0x5014;
    regs.a[3] = 0x600000;
    regs.in_movem = 10;
    regs.i_ea = 0x5014;
    regs.i_eav = 0x5014;
    asm_m68k("movem.l %D1-%D3/%A1-%A3, -(%A2)");
    m68k_do_execute();
    BOOST_TEST( raw_read32(0x5010) == 0x5014 );
    BOOST_TEST( raw_read32(0x500C) == 0x400000 );
    BOOST_TEST( raw_read32(0x5008) == 0x300000 );
    BOOST_TEST( raw_read32(0x5004) == 0x200000 );
    BOOST_TEST( raw_read32(0x5000) == 0x100000 );
    BOOST_TEST( regs.a[2] == 0x5000);
    BOOST_TEST( regs.in_movem == -1);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()