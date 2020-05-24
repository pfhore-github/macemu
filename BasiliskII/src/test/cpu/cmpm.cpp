#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(cmpm_byte, fixture);

BOOST_AUTO_TEST_CASE(reg) {
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130411 } );
	BOOST_TEST( cpu.R[8+0] == 0x1001 );
	BOOST_TEST( cpu.R[8+1] == 0x1101 );
}
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	test_write8(0x1000, 0);
	test_write8(0x1100, c);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130411 } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	test_write8(0x1000, 0x7f);
	test_write8(0x1100, v*0xff);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130411 } );
	BOOST_TEST( cpu.V == v );
}


BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	test_write8(0x1000, 0x80);
	test_write8(0x1100, v);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130411 } );
	BOOST_TEST( cpu.V == v );

}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write8(0x1000, n ? 0xf0 : 1);
	test_write8(0x1100, 1);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130411 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	test_write8(0x1000, 1);
	test_write8(0x1100, z);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130411 } );
	BOOST_TEST( cpu.Z == z );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cmpm_word, fixture);
BOOST_AUTO_TEST_CASE(reg) {
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130511 } );
	BOOST_TEST( cpu.R[8+0] == 0x1002 );
	BOOST_TEST( cpu.R[8+1] == 0x1102 );
}
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	test_write16(0x1000, 0);
	test_write16(0x1100, c);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130511 } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	test_write16(0x1000, 0x7fff);
	test_write16(0x1100, v*0xffff);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130511 } );
	BOOST_TEST( cpu.V == v );
}


BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	test_write16(0x1000, 0x8000);
	test_write16(0x1100, v);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130511 } );
	BOOST_TEST( cpu.V == v );

}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write16(0x1000, n ? 0xf000 : 1);
	test_write16(0x1100, 1);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130511 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	test_write16(0x1000, 1);
	test_write16(0x1100, z);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130511 } );
	BOOST_TEST( cpu.Z == z );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cmpm_long, fixture);
BOOST_AUTO_TEST_CASE(reg) {
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130611 } );
	BOOST_TEST( cpu.R[8+0] == 0x1004 );
	BOOST_TEST( cpu.R[8+1] == 0x1104 );
}
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	test_write32(0x1000, 0);
	test_write32(0x1100, c);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130611 } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	test_write32(0x1000, 0x7fffffff);
	test_write32(0x1100, v*0xffffffff);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130611 } );
	BOOST_TEST( cpu.V == v );
}


BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	test_write32(0x1000, 0x80000000);
	test_write32(0x1100, v);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130611 } );
	BOOST_TEST( cpu.V == v );

}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write32(0x1000, n ? 0xf0000000 : 1);
	test_write32(0x1100, 1);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130611 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	test_write32(0x1000, 1);
	test_write32(0x1100, z);
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	test_cpu( { 0130611 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_SUITE_END();
