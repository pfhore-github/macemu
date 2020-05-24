#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(eor_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa3;
	cpu.R[1] = 0xd2;
	test_cpu( { 0131400 } );
	BOOST_TEST( cpu.R[0] == ( 0xa3 ^ 0xd2 ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x80 : 1;
	cpu.R[1] = 0x7f;
	test_cpu( { 0131400 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = z;
	test_cpu( { 0131400 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	cpu.R[3] = 0xa5;
	test_ea_b(cpu, { 0133400 }, 0xdd, ea, 0x78 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(eor_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5;
	cpu.R[1] = 0xdddd;
	test_cpu( { 0131500 } );
	BOOST_TEST( cpu.R[0] == ( 0xa5a5 ^ 0xdddd ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x8000 : 1;
	cpu.R[1] = 0x7fff;
	test_cpu( { 0131500 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z;
	cpu.R[1] = 1;
	test_cpu( { 0131500 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	cpu.R[3] = 0xa5a5;
	test_ea_w(cpu, { 0133500 }, 0xdddd, ea, 0x7878 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(eor_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5a5a5;
	cpu.R[1] = 0xdddddddd;
	test_cpu( { 0131600 } );
	BOOST_TEST( cpu.R[0] == ( 0xa5a5a5a5 ^  0xdddddddd ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	cpu.R[1] = 0x7fffffff;
	test_cpu( { 0131600 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z;
	cpu.R[1] = 1;
	test_cpu( { 0131600 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	cpu.R[3] = 0xa5a5a5a5;
	test_ea_l(cpu, { 0133600 }, 0xdddddddd, ea, 0x78787878 );
}


BOOST_AUTO_TEST_SUITE_END();
