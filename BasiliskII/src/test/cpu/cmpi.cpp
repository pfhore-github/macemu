#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(cmpi_byte, fixture);

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = 0;
	test_cpu( { 0006000, (uint16_t)c } );
	BOOST_TEST( cpu.C == c );
}
BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7f;
	test_cpu( { 0006000, (uint16_t)(v*0xff) } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x80;
	test_cpu( { 0006000, (uint16_t)v } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0 : 1;
	test_cpu( { 0006000, 1 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 1;
	test_cpu( { 0006000, (uint16_t)z } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_data, ea)
{
	test_ea_b(cpu, { 0006000, 23 }, 23, ea, 23 );
	BOOST_TEST( cpu.Z == true );
}



BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cmpi_word, fixture);
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = 0;
	test_cpu( { 0006100, (uint16_t)c } );
	BOOST_TEST( cpu.C == c );
}
BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7fff;
	test_cpu( { 0006100, (uint16_t)(v*0xffff) } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x8000;
	test_cpu( { 0006100, (uint16_t)v } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf000 : 1;
	test_cpu( { 0006100, 1 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 1;
	test_cpu( { 0006100, (uint16_t)z } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_data, ea)
{
	test_ea_w(cpu, { 0006100, 2323 }, 2323, ea, 2323 );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cmpi_long, fixture);
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = 0;
	test_cpu( { 0006200, 0, (uint16_t)c } );
	BOOST_TEST( cpu.C == c );
}
BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7fffffff;
	test_cpu( { 0006200, (uint16_t)(v*0xffff), (uint16_t)(v*0xffff) } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x80000000;
	test_cpu( { 0006200, 0, (uint16_t)v } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0006200, 0, 1 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 1;
	test_cpu( { 0006200, 0, (uint16_t)z } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_data, ea)
{
	test_ea_l(cpu, { 0006200, 0x2323, 0x3434 },
			  0x23233434, ea, 0x23233434 );
	BOOST_TEST( cpu.Z == true );
}
BOOST_AUTO_TEST_SUITE_END();
