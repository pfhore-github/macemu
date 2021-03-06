#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(abcd, fixture);
BOOST_DATA_TEST_CASE(result_x,
					 bdata::xrange(2),
					 X
	)
{
	cpu.R[0] = 0x23;
	cpu.R[1] = 0x34;
	cpu.X = X;
	test_cpu( { 0141400 } );
	BOOST_TEST( cpu.R[1] == 0x57 + X);
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.R[0] = 0x98 + xc;
	cpu.R[1] = 1;
	test_cpu( { 0141400 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 old_z, new_z
	)
{
	cpu.R[0] = 0;
	cpu.R[1] = !new_z;
	cpu.Z = old_z;
	test_cpu( { 0141400 } );
	BOOST_TEST( cpu.Z == (old_z & new_z) );
}


BOOST_AUTO_TEST_CASE(dx_dy)
{
	cpu.R[0] = 0x23;
	cpu.R[1] = 0x34;
	test_cpu( { 0141400 } );
	BOOST_TEST( cpu.R[1] == 0x57 );
}

BOOST_AUTO_TEST_CASE(ax_ay)
{
	cpu.R[8+1] = 0x1001;
	cpu.R[8+0] = 0x1101;
	test_write8(0x1000, 0x23);
	test_write8(0x1100, 0x34);
	
	test_cpu( { 0140411 } );
	BOOST_TEST( test_read8(0x1100) == 0x57 );
	BOOST_TEST( cpu.R[8+1] = 0x1000 );
	BOOST_TEST( cpu.R[8+0] = 0x1100 );
}

BOOST_AUTO_TEST_SUITE_END();
