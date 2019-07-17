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
BOOST_FIXTURE_TEST_SUITE(abcd, fixture);
BOOST_DATA_TEST_CASE(result_x,
					 bdata::xrange(2),
					 X
	)
{
	xcpu.D[0] = 0x23;
	xcpu.D[1] = 0x34;
	xcpu.X = X;
	test_cpu( { 0141400 } );
	BOOST_TEST( xcpu.D[1] == 0x57 + X);
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0x98 + xc;
	xcpu.D[1] = 1;
	test_cpu( { 0141400 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	xcpu.D[1] = ! z;
	test_cpu( { 0141400 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dy_dx,
					 bdata::xrange(8) * bdata::xrange(8),
					 y, x)
{
	xcpu.D[y] = 0x23;
	xcpu.D[x] = 0x34;
	test_cpu( { (uint16_t)(0140400 | (x << 9) | y) } );
	if( x == y ) {
		BOOST_TEST( xcpu.D[x] == 0x68 );
	} else {
		BOOST_TEST( xcpu.D[x] == 0x57 );
	}
}

BOOST_DATA_TEST_CASE(ay_ax,
					 bdata::xrange(8) * bdata::xrange(8),
					 y, x)
{
	if( x != y ) {
		xcpu.A[y] = 0x1001;
		xcpu.A[x] = 0x1101;
		test_write8(0x1000, 0x23);
		test_write8(0x1100, 0x34);
	
		test_cpu( { (uint16_t)(0140410 | (x << 9) | y) } );
		BOOST_TEST( test_read8(0x1100) == 0x57 );
		BOOST_TEST( xcpu.A[y] = 0x1000 );
		BOOST_TEST( xcpu.A[x] = 0x1100 );
	} else {
		xcpu.A[y] = 0x1002;
		test_write8(0x1001, 0x23);
		test_write8(0x1000, 0x34);
	
		test_cpu( { (uint16_t)(0140410 | (x << 9) | y) } );
		BOOST_TEST( test_read8(0x1000) == 0x57 );
		BOOST_TEST( xcpu.A[y] = 0x1000 );
	}
}

BOOST_AUTO_TEST_SUITE_END();
