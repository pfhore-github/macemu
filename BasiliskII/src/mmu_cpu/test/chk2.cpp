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
    regs.vbr = 0x100;
    regs.d[2] = 0x40;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.pc == 4);
}

BOOST_AUTO_TEST_CASE( low_eq )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x10;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.pc == 4);
}

BOOST_AUTO_TEST_CASE( high )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x50;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.pc == 4);
}

BOOST_AUTO_TEST_CASE( under )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x05;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_CASE( over )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x55;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2b (%A1), %D2");
    raw_write8(0x10, 0x10);
    raw_write8(0x11, 0x50);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Word )

BOOST_AUTO_TEST_CASE( in_range )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x4000;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.pc == 4);
}

BOOST_AUTO_TEST_CASE( low_eq )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x1000;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.pc == 4);
}

BOOST_AUTO_TEST_CASE( high )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x5000;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.pc == 4);
}

BOOST_AUTO_TEST_CASE( under )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x0005;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);

    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_CASE( over )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x5555;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2w (%A1), %D2");
    raw_write16(0x10, 0x1000);
    raw_write16(0x12, 0x5000);

    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Long )

BOOST_AUTO_TEST_CASE( in_range )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x40000000;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.pc == 4);
}

BOOST_AUTO_TEST_CASE( low_eq )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x10000000;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.pc == 4);
}

BOOST_AUTO_TEST_CASE( high )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x50000000;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);
    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( regs.z );
    BOOST_TEST( ! regs.c );
    BOOST_TEST( regs.pc == 4);
}

BOOST_AUTO_TEST_CASE( under )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x00050000;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);

    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_CASE( over )
{    
    regs.pc = 0;
    regs.vbr = 0x100;
    regs.d[2] = 0x55550000;
    
    regs.a[1] = 0x10;
    asm_m68k("chk2l (%A1), %D2");
    raw_write32(0x10, 0x10000000);
    raw_write32(0x14, 0x50000000);

    raw_write32(0x100+6*4, 0x12345678);
    m68k_do_execute();
    BOOST_TEST( ! regs.z );
    BOOST_TEST( regs.c );
    BOOST_TEST( regs.pc == 0x12345678);
}

BOOST_AUTO_TEST_SUITE_END()
