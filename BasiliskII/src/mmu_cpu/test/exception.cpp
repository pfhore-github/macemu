#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "newcpu.h"
#include "memory.h"
#include "exception.h"
#include "test/test_common.h"
#include <vector>
void m68k_do_execute ();
extern std::vector<uint8_t> RAM;
struct Ex_Init {
    InitFix fix;
    Ex_Init() {
        regs.vbr = 0x1000;
        for(int i = 0; i < 256; ++i) {
            raw_write32( 0x1000 + 4 * i, 0x2000 + 4 * i);
        }
    }
};
BOOST_FIXTURE_TEST_SUITE( exception, Ex_Init )
BOOST_AUTO_TEST_CASE( ACCESS_FAULT )
{
    regs.a[2] = 0x4ffffffe;
    regs.isp = regs.a[7] = 0x1000;
    asm_m68k("move.w (%A2), %D1");
    m68k_do_execute();
    BOOST_TEST( regs.S );
    BOOST_TEST( regs.pc == 0x2000 + 4 * 2);
    BOOST_TEST( raw_read32( regs.a[7] + 2) == 0);
    BOOST_TEST( raw_read16( regs.a[7] + 6) == 0x7000 + 4 * 2);
    BOOST_TEST( raw_read32( regs.a[7] + 8) == 0);
    BOOST_TEST( raw_read16( regs.a[7] + 0x0c) == 1 << 8 | 1 << 3 | 2 << 5);
    BOOST_TEST( raw_read32( regs.a[7] + 0x14) == 0x4ffffffe);
}

BOOST_AUTO_TEST_CASE( ADDRESS_ERROR )
{
    regs.pc = 0x1001;
    regs.isp = regs.a[7] = 0x1000;
    m68k_do_execute();
    BOOST_TEST( regs.S );
    BOOST_TEST( regs.pc == 0x2000 + 4 * 3);
    BOOST_TEST( raw_read32( regs.a[7] + 2) == 0x1001);
    BOOST_TEST( raw_read16( regs.a[7] + 6) == 0x2000 + 4 * 3);
    BOOST_TEST( raw_read32( regs.a[7] + 8) == 0x1000);
}

BOOST_AUTO_TEST_CASE( ILLEGAL )
{
    regs.isp = regs.a[7] = 0x1000;
    asm_m68k("illegal");
    m68k_do_execute();
    BOOST_TEST( regs.S );
    BOOST_TEST( regs.pc == 0x2000 + 4 * 4);
    BOOST_TEST( raw_read32( regs.a[7] + 2) == 0);
    BOOST_TEST( raw_read16( regs.a[7] + 6) == 0x0000 + 4 * 4);
}

BOOST_AUTO_TEST_CASE( Div0 )
{
    regs.isp = regs.a[7] = 0x1000;
    regs.d[1] = 0;
    asm_m68k("divu.l %D1, %D2");
    m68k_do_execute();
    BOOST_TEST( regs.S );
    BOOST_TEST( regs.pc == 0x2000 + 4 * 5);
    BOOST_TEST( raw_read32( regs.a[7] + 2) == 4);
    BOOST_TEST( raw_read16( regs.a[7] + 6) == 0x2000 + 4 * 5);
    BOOST_TEST( raw_read32( regs.a[7] + 8) == 0);
}
BOOST_AUTO_TEST_SUITE_END()