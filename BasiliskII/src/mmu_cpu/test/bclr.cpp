#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "newcpu.h"
#include "memory.h"
#include <vector>
#include "test/test_common.h"

BOOST_AUTO_TEST_SUITE( Imm )
BOOST_DATA_TEST_CASE( Byte,
boost::unit_test::data::xrange(1), value )
{
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    raw_write8(0x10, value << 3);
    asm_m68k("bclr #3, (%A1)");
    m68k_do_execute();
    BOOST_TEST( regs.z != value);
    BOOST_TEST( !(raw_read8(0x10) & 1 << 3));
}

BOOST_DATA_TEST_CASE( Long,
boost::unit_test::data::xrange(1), value )
{
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[1] = value << 13;
    asm_m68k("btst #13, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z != value);
    BOOST_TEST( !(regs.d[1] & 1 << 13));

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( Reg )
BOOST_DATA_TEST_CASE( Byte,
boost::unit_test::data::xrange(1), value )
{
    regs.pc = 0;
    regs.vbr = 0;
    regs.a[1] = 0x10;
    regs.d[2] = 3;
    raw_write8(0x10, value << 3);
    asm_m68k("bclr %D2, (%A1)");
    m68k_do_execute();
    BOOST_TEST( regs.z != value);
    BOOST_TEST( !(raw_read8(0x10) & 1 << 3));
}

BOOST_DATA_TEST_CASE( Long,
boost::unit_test::data::xrange(1), value )
{
    regs.pc = 0;
    regs.vbr = 0;
    regs.d[2] = 13;
    regs.d[1] = value << 13;
    asm_m68k("btst %D2, %D1");
    m68k_do_execute();
    BOOST_TEST( regs.z != value);
    BOOST_TEST( !(regs.d[1] & 1 << 13));

}

BOOST_AUTO_TEST_SUITE_END()