#define BOOST_TEST_DYN_LINK
#include "newcpu.h"
#include "memory.h"
#include "test/test_common.h"
BOOST_FIXTURE_TEST_SUITE(MOVEP, InitFix)
BOOST_DATA_TEST_CASE(opc, REG() * REG(), dn, an) {
    BOOST_TEST(opc_map[0000410 | dn << 9 | an] == opc_map[0000410]);
    BOOST_TEST(opc_map[0000610 | dn << 9 | an] == opc_map[0000610]);
    BOOST_TEST(opc_map[0000510 | dn << 9 | an] == opc_map[0000510]);
    BOOST_TEST(opc_map[0000710 | dn << 9 | an] == opc_map[0000710]);
}
BOOST_AUTO_TEST_CASE( word_from )
{    
    regs.a[1] = 0x10;
    raw_write8(0x14, 0x12);
    raw_write8(0x15, 0x13);
    raw_write8(0x16, 0x14);
    raw_write8(0x17, 0x15);
    raw_write16(0, 0002411);
    raw_write16(2, 4);
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0x1214 );
}

BOOST_AUTO_TEST_CASE( word_to  )
{    
    regs.a[1] = 0x10;
    regs.d[2] = 0x1234;
    raw_write16(0, 0002611);
    raw_write16(2, 4);
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x14) == 0x12);
    BOOST_TEST( raw_read8(0x16) == 0x34);
}

BOOST_AUTO_TEST_CASE( long_from )
{    
    regs.a[1] = 0x10;
    raw_write8(0x14, 0x12);
    raw_write8(0x15, 0x13);
    raw_write8(0x16, 0x14);
    raw_write8(0x17, 0x15);
    raw_write8(0x18, 0x16);
    raw_write8(0x19, 0x17);
    raw_write8(0x1a, 0x18);
    raw_write8(0x1b, 0x19);
    raw_write16(0, 0002511);
    raw_write16(2, 4);
    m68k_do_execute();
    BOOST_TEST( regs.d[2] == 0x12141618 );
}

BOOST_AUTO_TEST_CASE( long_to )
{    
    regs.a[1] = 0x10;
    regs.d[2] = 0x12345678;
    raw_write16(0, 0002711);
    raw_write16(2, 4);
    m68k_do_execute();
    BOOST_TEST( raw_read8(0x14) == 0x12);
    BOOST_TEST( raw_read8(0x16) == 0x34);
    BOOST_TEST( raw_read8(0x18) == 0x56);
    BOOST_TEST( raw_read8(0x1a) == 0x78);
}
BOOST_AUTO_TEST_SUITE_END()