#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(subx_byte, fixture);
BOOST_DATA_TEST_CASE(x_in,
					 bdata::xrange(2),
					 x) {
	cpu.R[0] = 23;
	cpu.R[1] = 34 | 0xffffff00;
	cpu.X = x;
	test_cpu( { 0111400 } );
	BOOST_TEST( cpu.R[1] == (( 34 - 23 - x) | 0xffffff00 ));
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = ! xc;
	cpu.X = false;
	test_cpu( { 0111400 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = 0x81 - v;
	cpu.X = false;
	test_cpu( { 0111400 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x81;
	cpu.R[1] = v;
	cpu.X = false;
	test_cpu( { 0111400 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = n ? 0xf0 : 1;
	cpu.X = false;
	test_cpu( { 0111400 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 z_old, z_new
	)
{
	cpu.R[0] = 0;
	cpu.R[1] = ! z_new;
	cpu.Z = z_old;
	test_cpu( { 0111400 } );
	BOOST_TEST( cpu.Z == (z_old && z_new) );
}

BOOST_AUTO_TEST_CASE(dy_dx)
{
	cpu.R[0] = 23;
	cpu.R[1] = 34;
	test_cpu( { 0111400 } );
	BOOST_TEST( cpu.R[1] == 11 );
}

BOOST_AUTO_TEST_CASE(ay_ax)
{
	test_write8(0x1000, 23);
	test_write8(0x1100, 34);
	cpu.R[8+0] = 0x1001;
	cpu.R[8+1] = 0x1101;
	test_cpu( { 0111410 } );
	BOOST_TEST( test_read8(0x1100) == 11 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(subx_word, fixture);
BOOST_DATA_TEST_CASE(x_in,
					 bdata::xrange(2),
					 x) {
	cpu.R[0] = 2300;
	cpu.R[1] = 3400 | 0xffff0000;
	cpu.X = x;
	test_cpu( { 0111500 } );
	BOOST_TEST( cpu.R[1] == (( 3400 - 2300 - x) | 0xffff0000) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = !xc;
	cpu.X = false;
	test_cpu( { 0111500 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[1] = 0x8001 - v;
	cpu.R[0] = 1;
	cpu.X = false;
	test_cpu( { 0111500 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x8001;
	cpu.R[1] = v;
	cpu.X = false;
	test_cpu( { 0111500 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[1] = n ? 0xf000 : 1;
	cpu.R[0] = 1;
	cpu.X = false;
	test_cpu( { 0111500 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 z_old, z_new
	)
{
	cpu.R[0] = 0;
	cpu.R[1] = ! z_new;
	cpu.Z = z_old;
	test_cpu( { 0111500 } );
	BOOST_TEST( cpu.Z == (z_old && z_new) );
}

BOOST_AUTO_TEST_CASE(dy_dx)
{
	cpu.R[0] = 2300;
	cpu.R[1] = 3400;
	test_cpu( { 0111500 } );
	BOOST_TEST( cpu.R[1] == 1100 );
}

BOOST_AUTO_TEST_CASE(ay_ax)
{
	test_write16(0x1000, 2300);
	test_write16(0x1100, 3400);
	cpu.R[8+0] = 0x1002;
	cpu.R[8+1] = 0x1102;
	test_cpu( { 0111510 } );
	BOOST_TEST( test_read16(0x1100) == 1100 );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(subx_long, fixture);

BOOST_DATA_TEST_CASE(x_in,
					 bdata::xrange(2),
					 x) {
	cpu.R[0] = 230000;
	cpu.R[1] = 340000;
	cpu.X = x;
	test_cpu( { 0111600 } );
	BOOST_TEST( cpu.R[1] == ( 340000 - 230000 - x) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = ! xc;
	cpu.X = false;
	test_cpu( { 0111600 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = 0x80000001 - v;
	cpu.X = false;
	test_cpu( { 0111600 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = 0x80000001;
	cpu.R[1] = v;
	cpu.X = false;
	test_cpu( { 0111600 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = 1;
	cpu.R[1] = n ?0xf0000000 : 1;
	cpu.X = false;
	test_cpu( { 0111600 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 z_old, z_new
	)
{
	cpu.R[0] = 0;
	cpu.R[1] = ! z_new;
	cpu.Z = z_old;
	test_cpu( { 0111600 } );
	BOOST_TEST( cpu.Z == (z_old && z_new) );
}

BOOST_AUTO_TEST_CASE(dy_dx)
{
	cpu.R[0] = 230000;
	cpu.R[1] = 340000;
	test_cpu( { 0111600 } );
	BOOST_TEST( cpu.R[1] == 110000 );
}

BOOST_AUTO_TEST_CASE(ay_ax)
{
	test_write32(0x1000, 230000);
	test_write32(0x1100, 340000);
	cpu.R[8+0] = 0x1004;
	cpu.R[8+1] = 0x1104;
	test_cpu( { 0111610  } );
	BOOST_TEST( test_read32(0x1100) == 110000 );
}


BOOST_AUTO_TEST_SUITE_END();
