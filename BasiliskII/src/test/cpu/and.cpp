#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(and_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa3;
	cpu.R[1] = 0xd2 | 0xffffff00;
	test_cpu( { 0141000 } );
	BOOST_TEST( cpu.R[1] == (( 0xa3 & 0xd2 ) | 0xffffff00) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x80 : 1;
	cpu.R[1] = 0x80;
	test_cpu( { 0141000 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 0xff;
	cpu.R[1] = ! z;
	test_cpu( { 0141000 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	cpu.R[1] = 0xdd;
	test_ea_b(cpu, { 0141000 }, 0xa5, ea, 0xa5);
	BOOST_TEST( cpu.R[1] == 0x85 );
}


BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[1] = 0xdd;
	test_ea_b(cpu, { 0141400 }, 0xa5, ea, 0x85);
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(and_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5;
	cpu.R[1] = 0xdddd;
	test_cpu( { 0141100 } );
	BOOST_TEST( cpu.R[1] == ( 0xa5a5 & 0xdddd ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x8000 : 1;
	cpu.R[1] = 0xffff;
	test_cpu( { 0141100 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	cpu.R[1] = 0xffff;
	test_cpu( { 0141100 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	cpu.R[1] = 0xdddd;
	test_ea_w(cpu, { 0141100 }, 0xa5a5, ea, 0xa5a5);
	BOOST_TEST( cpu.R[1] == 0x8585 );
}


BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[1] = 0xdddd;
	test_ea_w(cpu, { 0141500 }, 0xa5a5, ea, 0x8585);
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(and_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5a5a5;
	cpu.R[1] = 0xdddddddd;
	test_cpu( { 0141200 } );
	BOOST_TEST( cpu.R[1] == ( 0xa5a5a5a5 &  0xdddddddd ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	cpu.R[1] = 0xffffffff;
	test_cpu( { 0141200 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	cpu.R[1] = 0xffffffff;
	test_cpu( { 0141200 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	cpu.R[1] = 0xdddddddd;
	test_ea_l(cpu, { 0141200 }, 0xa5a5a5a5, ea, 0xa5a5a5a5);
	BOOST_TEST( cpu.R[1] == 0x85858585 );
}


BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[1] = 0xdddddddd;
	test_ea_l(cpu, { 0141600 }, 0xa5a5a5a5, ea, 0x85858585);
}
BOOST_AUTO_TEST_SUITE_END();
