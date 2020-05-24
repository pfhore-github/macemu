#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(rte, fixture);
BOOST_AUTO_TEST_CASE( user)  {
	cpu.S = false;
	test_cpu( { 0047163 } );
	test_in_exception(8) ;
}

BOOST_AUTO_TEST_CASE( format0 )  {
	cpu.S = true;
	cpu.X = cpu.Z = cpu.V = cpu.N = cpu.C = false;
	push16( 4 << 2 );
	push32( 0x2000 );
	push16( 0x1f );
	test_cpu( { 0047163 } );
	BOOST_TEST( cpu.PC == 0x2000 );
	BOOST_TEST( cpu.S == false );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.Z = true );
	BOOST_TEST( cpu.N = true );
	BOOST_TEST( cpu.V = true );
	BOOST_TEST( cpu.C = true );
}

BOOST_AUTO_TEST_CASE( format1 )  {
	cpu.S = true;
	cpu.X = cpu.Z = cpu.V = cpu.N = cpu.C = false;
	push16( (0 << 12) | 4 << 2 );
	push32( 0x2000 );
	push16( 0x1f );
	push16( (1 << 12) | 4 << 2 );
	push32( 0x3000 );
	push16( 1 << 13 );
	test_cpu( { 0047163 } );
	BOOST_TEST( cpu.PC == 0x2000 );
	BOOST_TEST( cpu.S == false );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.Z = true );
	BOOST_TEST( cpu.N = true );
	BOOST_TEST( cpu.V = true );
	BOOST_TEST( cpu.C = true );
}

BOOST_AUTO_TEST_CASE( format2 )  {
	cpu.S = true;
	cpu.X = cpu.Z = cpu.V = cpu.N = cpu.C = false;
	push32( 0x20000000 );
	push16( (2 << 12) | 4 << 2 );
	push32( 0x2000 );
	push16( 0x1f );
	test_cpu( { 0047163 } );
	BOOST_TEST( cpu.PC == 0x2000 );
	BOOST_TEST( cpu.S == false );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.Z = true );
	BOOST_TEST( cpu.N = true );
	BOOST_TEST( cpu.V = true );
	BOOST_TEST( cpu.C = true );
}

BOOST_AUTO_TEST_CASE( format3 )  {
	cpu.S = true;
	cpu.X = cpu.Z = cpu.V = cpu.N = cpu.C = false;
	push32( 0x20000000 );
	push16( (3 << 12) | 4 << 2 );
	push32( 0x2000 );
	push16( 0x1f );
	test_cpu( { 0047163 } );
	BOOST_TEST( cpu.PC == 0x2000 );
	BOOST_TEST( cpu.S == false );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.Z = true );
	BOOST_TEST( cpu.N = true );
	BOOST_TEST( cpu.V = true );
	BOOST_TEST( cpu.C = true );
}

BOOST_AUTO_TEST_CASE( format7 )  {
	cpu.S = true;
	cpu.X = cpu.Z = cpu.V = cpu.N = cpu.C = false;
	cpu.R[15] += 4*9;
	push32( 0x20000000 );
	cpu.R[15] += 2*3;
	push16( 0 );
	push32( 0x30000000 );
	push16( (7 << 12) | 4 << 2 );
	push32( 0x2000 );
	push16( 0x1f );
	test_cpu( { 0047163 } );
	BOOST_TEST( cpu.PC == 0x2000 );
	BOOST_TEST( cpu.S == false );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.Z = true );
	BOOST_TEST( cpu.N = true );
	BOOST_TEST( cpu.V = true );
	BOOST_TEST( cpu.C = true );
}

BOOST_AUTO_TEST_CASE( format7_CP )  {
	cpu.S = true;
	cpu.X = cpu.Z = cpu.V = cpu.N = cpu.C = false;
	cpu.ea_v = 0x4f000000;
	cpu.R[15] += 4*9;
	push32( 0x20000000 );
	cpu.R[15] += 2*3;
	push16( 1 << 15 );
	push32( 0x30000000 );
	push16( (7 << 12) | 4 << 2 );
	push32( 0x2000 );
	push16( 0x1f );
	test_cpu( { 0047163 } );
	test_in_exception( 55 );
	BOOST_TEST( test_read16( cpu.R[15] ) == 0x1f );
	BOOST_TEST( test_read32( cpu.R[15] + 2 ) == 0x2000 );
	BOOST_TEST( test_read16( cpu.R[15] + 6 ) == ( 3 << 12 | 55 << 2 ) );
	BOOST_TEST( test_read32( cpu.R[15] + 8 ) == 0x4f000000 );
}

BOOST_AUTO_TEST_CASE( format7_CU )  {
	cpu.S = true;
	cpu.X = cpu.Z = cpu.V = cpu.N = cpu.C = false;
	cpu.ea_v = 0x4f000000;
	cpu.R[15] += 4*9;
	push32( 0x20000000 );
	cpu.R[15] += 2*3;
	push16( 1 << 14 );
	push32( 0x30000000 );
	push16( (7 << 12) | 4 << 2 );
	push32( 0x2000 );
	push16( 0x1f );
	test_cpu( { 0047163 } );
	test_in_exception( 11 );
	BOOST_TEST( test_read16( cpu.R[15] ) == 0x1f );
	BOOST_TEST( test_read32( cpu.R[15] + 2 ) == 0x2000 );
	BOOST_TEST( test_read16( cpu.R[15] + 6 ) == ( 2 << 12 | 11 << 2 ) );
	BOOST_TEST( test_read32( cpu.R[15] + 8 ) == 0x4f000000 );
}


BOOST_AUTO_TEST_SUITE_END();
