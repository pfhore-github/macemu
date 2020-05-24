#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(cmpa_word, fixture);
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[8+0] = 0;
	cpu.R[1] = c;
	test_cpu( { 0130301 } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[8+0] = 0x7fff;
	cpu.R[1] = v * 0xffff;
	test_cpu( { 0130301 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[8+0] = 0x8000;
	cpu.R[1] = v;
	test_cpu( { 0130301 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[8+0] = n ? 0xf000 : 1;
	cpu.R[1] = 1;
	test_cpu( { 0130301 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[8+0] = 1;
	cpu.R[1] = z;
	test_cpu( { 0130301 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_all, ea)
{
	cpu.R[8+3] = 2323;
	test_ea_w(cpu, { 0133300 }, 2424, ea, 2424 );
	BOOST_TEST( cpu.Z == false );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cmpa_long, fixture);
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[8+1] = 0;
	cpu.R[0] = c;
	test_cpu( { 0131700 } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[8+1] = 0x7fffffff;
	cpu.R[0] = v * 0xffffffff;
	test_cpu( { 0131700 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v;
	cpu.R[8+1] = 0x80000000;
	test_cpu( { 0131700 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = 1;
	cpu.R[8+1] = n ? 0xf0000000 : 1;
	test_cpu( { 0131700 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 1;
	cpu.R[8+1] = z;
	test_cpu( { 0131700 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_all, ea)
{
	cpu.R[8+3] = 232323;
	test_ea_l(cpu, { 0133700 }, 242424, ea, 242424 );
	BOOST_TEST( cpu.Z == false );
}

BOOST_AUTO_TEST_SUITE_END();
