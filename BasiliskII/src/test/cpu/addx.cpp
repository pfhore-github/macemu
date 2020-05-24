#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(addx_byte, fixture);
BOOST_DATA_TEST_CASE(x_in,
					 bdata::xrange(2),
					 x) {
	cpu.R[0] = 23;
	cpu.R[1] = 34 | 0xffffff00;
	cpu.X = x;
	test_cpu( { 0151400 } );
	BOOST_TEST( cpu.R[1] == (( 23 + 34 + x) | 0xffffff00 ));
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 0xff;
	cpu.R[1] = xc;
	cpu.X = false;
	test_cpu( { 0151400 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7f;
	cpu.R[1] = v;
	cpu.X = false;
	test_cpu( { 0151400 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x80;
	cpu.R[1] = 0x7f + v;
	cpu.X = false;
	test_cpu( { 0151400 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0 : 1;
	cpu.R[1] = 1;
	cpu.X = false;
	test_cpu( { 0151400 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 z_old, z_new
	)
{
	cpu.R[0] = ! z_new;
	cpu.R[1] = ! z_new;
	cpu.Z = z_old;
	test_cpu( { 0151400 } );
	BOOST_TEST( cpu.Z == (z_old && z_new) );
}

BOOST_AUTO_TEST_CASE(dy_dx)
{
	cpu.R[0] = 23;
	cpu.R[1] = 34;
	test_cpu( { 0151400 } );
	BOOST_TEST( cpu.R[1] == 57 );
}

BOOST_AUTO_TEST_CASE(ay_ax)
{
	test_write8(0x1000, 23);
	test_write8(0x1100, 34);
	cpu.R[8+0] = 0x1001;
	cpu.R[8+1] = 0x1101;
	test_cpu( { 0151410 } );
	BOOST_TEST( test_read8(0x1100) == 57 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addx_word, fixture);
BOOST_DATA_TEST_CASE(x_in,
					 bdata::xrange(2),
					 x) {
	cpu.R[0] = 2300;
	cpu.R[1] = 3400 | 0xffff0000;
	cpu.X = x;
	test_cpu( { 0151500 } );
	BOOST_TEST( cpu.R[1] == (( 2300 + 3400 + x) | 0xffff0000) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 0xffff;
	cpu.R[1] = xc;
	cpu.X = false;
	test_cpu( { 0151500 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7fff;
	cpu.R[1] = v;
	cpu.X = false;
	test_cpu( { 0151500 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x8000;
	cpu.R[1] = 0x7fff + v;
	cpu.X = false;
	test_cpu( { 0151500 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf000 : 1;
	cpu.R[1] = 1;
	cpu.X = false;
	test_cpu( { 0151500 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 z_old, z_new
	)
{
	cpu.R[0] = ! z_new;
	cpu.R[1] = ! z_new;
	cpu.Z = z_old;
	test_cpu( { 0151500 } );
	BOOST_TEST( cpu.Z == (z_old && z_new) );
}

BOOST_AUTO_TEST_CASE(dy_dx)
{
	cpu.R[0] = 2300;
	cpu.R[1] = 3400;
	test_cpu( { 0151500 } );
	BOOST_TEST( cpu.R[1] == 5700 );
}

BOOST_AUTO_TEST_CASE(ay_ax)
{
	test_write16(0x1000, 2300);
	test_write16(0x1100, 3400);
	cpu.R[8+0] = 0x1002;
	cpu.R[8+1] = 0x1102;
	test_cpu( { 0151510 } );
	BOOST_TEST( test_read16(0x1100) == 5700 );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addx_long, fixture);

BOOST_DATA_TEST_CASE(x_in,
					 bdata::xrange(2),
					 x) {
	cpu.R[0] = 230000;
	cpu.R[1] = 340000;
	cpu.X = x;
	test_cpu( { 0151600 } );
	BOOST_TEST( cpu.R[1] == ( 230000 + 340000 + x) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 0xffffffff;
	cpu.R[1] = xc;
	cpu.X = false;
	test_cpu( { 0151600 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x7fffffff;
	cpu.R[1] = v;
	cpu.X = false;
	test_cpu( { 0151600 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x80000000;
	cpu.R[1] = 0x7fffffffU + v;
	cpu.X = false;
	test_cpu( { 0151600 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	cpu.R[1] = 1;
	cpu.X = false;
	test_cpu( { 0151600 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 z_old, z_new
	)
{
	cpu.R[0] = ! z_new;
	cpu.R[1] = ! z_new;
	cpu.Z = z_old;
	test_cpu( { 0151600 } );
	BOOST_TEST( cpu.Z == (z_old && z_new) );
}

BOOST_AUTO_TEST_CASE(dy_dx)
{
	cpu.R[0] = 230000;
	cpu.R[1] = 340000;
	test_cpu( { 0151600 } );
	BOOST_TEST( cpu.R[1] == 570000 );
}

BOOST_AUTO_TEST_CASE(ay_ax)
{
	test_write32(0x1000, 230000);
	test_write32(0x1100, 340000);
	cpu.R[8+0] = 0x1004;
	cpu.R[8+1] = 0x1104;
	test_cpu( { 0151610  } );
	BOOST_TEST( test_read32(0x1100) == 570000 );
}


BOOST_AUTO_TEST_SUITE_END();
