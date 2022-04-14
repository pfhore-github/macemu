#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_AUTO_TEST_SUITE( Byte )
BOOST_AUTO_TEST_CASE( in_range )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    regs.d[2] = 0x40;
    
    asm_m68k("cmp2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( ! regs.c );
}

BOOST_AUTO_TEST_CASE( low_eq )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    regs.d[2] = 0x10;
    
    asm_m68k("cmp2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
}

BOOST_AUTO_TEST_CASE( high )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    regs.d[2] = 0x50;
    
    asm_m68k("cmp2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
}

BOOST_AUTO_TEST_CASE( under )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    regs.d[2] = 0x05;
    
    asm_m68k("cmp2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
}

BOOST_AUTO_TEST_CASE( over )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    regs.d[2] = 0x55;
    
    asm_m68k("cmp2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Word )

BOOST_AUTO_TEST_CASE( in_range )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x4000;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( ! regs.c );
}

BOOST_AUTO_TEST_CASE( low_eq )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x1000;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
}

BOOST_AUTO_TEST_CASE( high )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x5000;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
}

BOOST_AUTO_TEST_CASE( under )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x0005;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
}

BOOST_AUTO_TEST_CASE( over )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x5500;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Long )

BOOST_AUTO_TEST_CASE( in_range )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x40000000;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( ! regs.c );
}

BOOST_AUTO_TEST_CASE( low_eq )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x10000000;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
}

BOOST_AUTO_TEST_CASE( high )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x50000000;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
}

BOOST_AUTO_TEST_CASE( under )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x00050000;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
}

BOOST_AUTO_TEST_CASE( over )
{    
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 0x55000000;
    
    regs.a[1] = 0x10;
    asm_m68k("cmp2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
}

BOOST_AUTO_TEST_SUITE_END()