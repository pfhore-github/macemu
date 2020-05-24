#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(addi_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[0] = 23 | 0xffffff00;
	test_cpu( { 0003000, 34 } );
	BOOST_TEST( cpu.R[0] == (( 23 + 34 )|0xffffff00) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = xc;
	test_cpu( { 0003000, 0xff } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v;
	test_cpu( { 0003000, 0x7f } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7f + v;
	test_cpu( { 0003000, 0x80 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0 : 1;
	cpu.R[1] = 1;
	test_cpu( { 0003000, 1 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0003000, 0 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_writedn, ea)
{
	test_ea_b(cpu, { 0003000, 23 }, 34, ea, 57 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addi_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[0] = 2300 | 0xffff0000;
	test_cpu( { 0003100, 3400 } );
	BOOST_TEST( cpu.R[0] == (( 2300 + 3400 )|0xffff0000) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = xc;
	test_cpu( { 0003100, 0xffff } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v;
	test_cpu( { 0003100, 0x7fff } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7fff + v;
	test_cpu( { 0003100, 0x8000 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf000 : 1;
	test_cpu( { 0003100, 1 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0003100, 0 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_writedn, ea)
{
	test_ea_w(cpu, { 0003100, 2300 }, 3400, ea, 5700 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addi_long, fixture);
constexpr uint16_t HIGH = uint16_t(340000>>16);
constexpr uint16_t LOW = uint16_t(340000 & 0xffff);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[0] = 230000;
	test_cpu( { 0003200, HIGH, LOW } );
	BOOST_TEST( cpu.R[0] == ( 230000 + 340000 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = xc;
	test_cpu( { 0003200, 0xffff, 0xffff } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v;
	test_cpu( { 0003200, 0x7fff, 0xffff } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7fffffffU + v;
	test_cpu( { 0003200, 0x8000, 0 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0003200, 0, 1 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0003200, 0, 0 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_writedn, ea)
{
	test_ea_l(cpu, { 0003200, HIGH, LOW }, 230000, ea, 570000 );
}


BOOST_AUTO_TEST_SUITE_END();
