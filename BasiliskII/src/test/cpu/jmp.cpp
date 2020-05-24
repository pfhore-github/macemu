#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(jmp, fixture);
BOOST_AUTO_TEST_CASE(mem) {
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0047322 } );
	BOOST_TEST( cpu.PC == 0x1000 );
}
BOOST_AUTO_TEST_CASE(d16an) {
	cpu.R[8+2] = 0x1000;
	test_cpu( { 0047352, 0x100 } );
	BOOST_TEST( cpu.PC == 0x1100 );
}
BOOST_AUTO_TEST_CASE(d8anxn)
{
	cpu.R[8+2] = 0x1000;
	cpu.R[0] = 0x10;
	test_cpu( { 0047362, 0x0830 } );
	BOOST_TEST( cpu.PC == 0x1040 );
}
BOOST_AUTO_TEST_CASE(xxx_w)
{
	test_cpu( { 0047370, 0x1000 } );
	BOOST_TEST( cpu.PC == 0x1000 );	
}
BOOST_AUTO_TEST_CASE(xxx_l)
{
	test_cpu( { 0047371, 0, 0x1000 } );
	BOOST_TEST( cpu.PC == 0x1000 );	
}
BOOST_AUTO_TEST_CASE(d16pc)
{
	test_cpu( { 0047372, 0x1000 } );
	BOOST_TEST( cpu.PC == 0x1002 );	
}

BOOST_AUTO_TEST_CASE(d8pcxn)
{
	cpu.R[0] = 0x1000;
	test_cpu( { 0047373, 0x0830 } );
	BOOST_TEST( cpu.PC == 0x1032 );	
}

BOOST_AUTO_TEST_SUITE_END();
