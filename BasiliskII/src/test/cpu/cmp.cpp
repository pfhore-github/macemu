#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(cmp_byte, fixture);

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = 0;
	cpu.R[1] = c;
	test_cpu( { 0130001 } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7f;
	cpu.R[1] = v * 0xff;
	test_cpu( { 0130001 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x80;
	cpu.R[1] = v;
	test_cpu( { 0130001 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0 : 1;
	cpu.R[1] = 1;
	test_cpu( { 0130001 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = z;
	test_cpu( { 0130001 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_all, ea)
{
	cpu.R[3] = 24;	
	test_ea_b(cpu, { 0133000 }, 23, ea, 23 );
	BOOST_TEST( cpu.Z == false );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cmp_word, fixture);
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = 0;
	cpu.R[1] = c;
	test_cpu( { 0130101 } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7fff;
	cpu.R[1] = v * 0xffff;
	test_cpu( { 0130101 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x8000;
	cpu.R[1] = v;
	test_cpu( { 0130101 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf000 : 1;
	cpu.R[1] = 1;
	test_cpu( { 0130101 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = z;
	test_cpu( { 0130101 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_all, ea)
{
	cpu.R[3] = 2424;	
	test_ea_w(cpu, { 0133100 }, 2323, ea, 2323 );
	BOOST_TEST( cpu.Z == false );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cmp_long, fixture);
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = 0;
	cpu.R[1] = c;
	test_cpu( { 0130201 } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7fffffff;
	cpu.R[1] = v * 0xffffffff;
	test_cpu( { 0130201 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x80000000;
	cpu.R[1] = v;
	test_cpu( { 0130201 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	cpu.R[1] = 1;
	test_cpu( { 0130201 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = z;
	test_cpu( { 0130201 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_all, ea)
{
	cpu.R[3] = 242424;	
	test_ea_l(cpu, { 0133200 }, 232323, ea, 232323 );
	BOOST_TEST( cpu.Z == false );
}
BOOST_AUTO_TEST_SUITE_END();
