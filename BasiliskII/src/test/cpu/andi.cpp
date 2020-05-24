#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(andi_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa3 | 0xffffff00;
	test_cpu( { 0001000, 0xd2 } );
	BOOST_TEST( cpu.R[0] == ( ( 0xa3 & 0xd2 ) | 0xffffff00) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x80 : 1;
	test_cpu( { 0001000, 0xff } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0001000, 0xff } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	test_ea_b(cpu, { 0001000, 0xdd }, 0xa5, ea, 0x85 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(andi_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5 | 0xffff0000;
	test_cpu( { 0001100, 0xdddd } );
	BOOST_TEST( cpu.R[0] == (( 0xa5a5 & 0xdddd ) | 0xffff0000) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x8000 : 1;
	test_cpu( { 0001100, 0xffff } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0001100, 0xffff } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	test_ea_w(cpu, { 0001100, 0xdddd }, 0xa5a5, ea, 0x8585 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(andi_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5a5a5;
	test_cpu( { 0001200, 0xdddd, 0xdddd } );
	BOOST_TEST( cpu.R[0] == ( 0xa5a5a5a5 &  0xdddddddd ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0001200, 0xffff, 0xffff } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0001200, 0xffff, 0xffff } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	test_ea_l(cpu, { 0001200, 0xdddd, 0xdddd },
			  0xa5a5a5a5, ea, 0x85858585 );
}

BOOST_AUTO_TEST_SUITE_END();


BOOST_FIXTURE_TEST_SUITE(andi_ccr, fixture);
BOOST_DATA_TEST_CASE(test,
					 bdata::xrange(2) * bdata::xrange(2) * bdata::xrange(2) *
					 bdata::xrange(2) * bdata::xrange(2),
					 x,n,z,v,c
	)
{
	cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
	uint16_t f = x << 4 | n << 3 | z << 2 | v << 1 | c;
	test_cpu( { 0001074, f } );
	BOOST_TEST( cpu.X == x );
	BOOST_TEST( cpu.N == n );
	BOOST_TEST( cpu.Z == z );
	BOOST_TEST( cpu.V == v );
	BOOST_TEST( cpu.C == c );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(andi_sr, fixture);
BOOST_AUTO_TEST_CASE(user)
{
	cpu.S = false;
	cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = true;
	test_cpu( { 0001174, 0 } );
	test_in_exception(8) ;
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.N == true );
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( cpu.V == true );
	BOOST_TEST( cpu.C == true );
}
BOOST_AUTO_TEST_CASE(system)
{
	cpu.S = true;
	cpu.T =3;
	cpu.M = true;
	cpu.intmask = 7;
	test_cpu( { 0001174, 0x0 } );
	test_in_exception(0);
	BOOST_TEST( cpu.T == 0 );
	BOOST_TEST( cpu.S == false );
	BOOST_TEST( cpu.M == false );
	BOOST_TEST( cpu.intmask == 0 );
}

BOOST_AUTO_TEST_SUITE_END();
