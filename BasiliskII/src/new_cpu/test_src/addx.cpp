#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "init.h"
#include "../registers.hpp"
#include "../exceptions.hpp"
#include <vector>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;
extern CPU xcpu;
void test_cpu(const std::vector<uint16_t>& c);
BOOST_TEST_GLOBAL_FIXTURE(init);
BOOST_FIXTURE_TEST_SUITE(addx_byte, fixture);
BOOST_DATA_TEST_CASE(x_in,
					 bdata::xrange(2),
					 x) {
	xcpu.D[0] = 23;
	xcpu.D[1] = 34;
	xcpu.X = x;
	test_cpu( { 0151400 } );
	BOOST_TEST( xcpu.D[1] == ( 23 + 34 + x) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0xff;
	xcpu.D[1] = xc;
	xcpu.X = false;
	test_cpu( { 0151400 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7f;
	xcpu.D[1] = v;
	xcpu.X = false;
	test_cpu( { 0151400 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x80;
	xcpu.D[1] = 0x7f + v;
	xcpu.X = false;
	test_cpu( { 0151400 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0 : 1;
	xcpu.D[1] = 1;
	xcpu.X = false;
	test_cpu( { 0151400 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 z_old, z_new
	)
{
	xcpu.D[0] = ! z_new;
	xcpu.D[1] = ! z_new;
	xcpu.Z = z_old;
	test_cpu( { 0151400 } );
	BOOST_TEST( xcpu.Z == (z_old && z_new) );
}

BOOST_DATA_TEST_CASE(dn_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 23;
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150400 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.D[dst] == 68 );
	} else {
		BOOST_TEST( xcpu.D[dst] == 57 );
	}
}

BOOST_DATA_TEST_CASE(mem_mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	if( src != dst ) {
		test_write8(0x1000, 23);
		test_write8(0x1100, 34);
		xcpu.A[src] = 0x1001;
		xcpu.A[dst] = 0x1101;
		test_cpu( { (uint16_t)(0150410 | dst << 9 | src) } );
		BOOST_TEST( test_read8(0x1100) == 57 );
	} else {
		test_write8(0x1001, 23);
		test_write8(0x1000, 34);
		xcpu.A[dst] = 0x1002;
		test_cpu( { (uint16_t)(0150410 | dst << 9 | src) } );
		BOOST_TEST( test_read8(0x1000) == 57 );
	}
}





BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addx_word, fixture);
BOOST_DATA_TEST_CASE(x_in,
					 bdata::xrange(2),
					 x) {
	xcpu.D[0] = 2300;
	xcpu.D[1] = 3400;
	xcpu.X = x;
	test_cpu( { 0151500 } );
	BOOST_TEST( xcpu.D[1] == ( 2300 + 3400 + x) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0xffff;
	xcpu.D[1] = xc;
	xcpu.X = false;
	test_cpu( { 0151500 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7fff;
	xcpu.D[1] = v;
	xcpu.X = false;
	test_cpu( { 0151500 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x8000;
	xcpu.D[1] = 0x7fff + v;
	xcpu.X = false;
	test_cpu( { 0151500 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf000 : 1;
	xcpu.D[1] = 1;
	xcpu.X = false;
	test_cpu( { 0151500 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 z_old, z_new
	)
{
	xcpu.D[0] = ! z_new;
	xcpu.D[1] = ! z_new;
	xcpu.Z = z_old;
	test_cpu( { 0151500 } );
	BOOST_TEST( xcpu.Z == (z_old && z_new) );
}

BOOST_DATA_TEST_CASE(dn_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 2300;
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150500 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.D[dst] == 6800 );
	} else {
		BOOST_TEST( xcpu.D[dst] == 5700 );
	}
}

BOOST_DATA_TEST_CASE(mem_mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	if( src != dst ) {
		test_write16(0x1000, 2300);
		test_write16(0x1100, 3400);
		xcpu.A[src] = 0x1002;
		xcpu.A[dst] = 0x1102;
		test_cpu( { (uint16_t)(0150510 | dst << 9 | src) } );
		BOOST_TEST( test_read16(0x1100) == 5700 );
	} else {
		test_write16(0x1002, 2300);
		test_write16(0x1000, 3400);
		xcpu.A[dst] = 0x1004;
		test_cpu( { (uint16_t)(0150510 | dst << 9 | src) } );
		BOOST_TEST( test_read16(0x1000) == 5700 );
	}
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addx_long, fixture);

BOOST_DATA_TEST_CASE(x_in,
					 bdata::xrange(2),
					 x) {
	xcpu.D[0] = 230000;
	xcpu.D[1] = 340000;
	xcpu.X = x;
	test_cpu( { 0151600 } );
	BOOST_TEST( xcpu.D[1] == ( 230000 + 340000 + x) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0xffffffff;
	xcpu.D[1] = xc;
	xcpu.X = false;
	test_cpu( { 0151600 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7fffffff;
	xcpu.D[1] = v;
	xcpu.X = false;
	test_cpu( { 0151600 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x80000000;
	xcpu.D[1] = 0x7fffffff + v;
	xcpu.X = false;
	test_cpu( { 0151600 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0000000 : 1;
	xcpu.D[1] = 1;
	xcpu.X = false;
	test_cpu( { 0151600 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 z_old, z_new
	)
{
	xcpu.D[0] = ! z_new;
	xcpu.D[1] = ! z_new;
	xcpu.Z = z_old;
	test_cpu( { 0151600 } );
	BOOST_TEST( xcpu.Z == (z_old && z_new) );
}

BOOST_DATA_TEST_CASE(dn_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 230000;
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150600 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.D[dst] == 680000 );
	} else {
		BOOST_TEST( xcpu.D[dst] == 570000 );
	}
}

BOOST_DATA_TEST_CASE(mem_mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	if( src != dst ) {
		test_write32(0x1000, 230000);
		test_write32(0x1100, 340000);
		xcpu.A[src] = 0x1004;
		xcpu.A[dst] = 0x1104;
		test_cpu( { (uint16_t)(0150610 | dst << 9 | src) } );
		BOOST_TEST( test_read32(0x1100) == 570000 );
	} else {
		test_write32(0x1004, 230000);
		test_write32(0x1000, 340000);
		xcpu.A[dst] = 0x1008;
		test_cpu( { (uint16_t)(0150610 | dst << 9 | src) } );
		BOOST_TEST( test_read32(0x1000) == 570000 );
	}
}


BOOST_AUTO_TEST_SUITE_END();
