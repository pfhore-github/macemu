#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(nbcd, fixture);
BOOST_DATA_TEST_CASE(result_x,
					 bdata::xrange(2),
					 X
	)
{
	cpu.R[1] = 0x34;
	cpu.X = X;
	test_cpu( { 0044001 } );
	BOOST_TEST( cpu.R[1] == 0x66 - X);
}
BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	cpu.X = xc;
	cpu.R[0] = 0;
	test_cpu( { 0044000 } );
	BOOST_TEST( cpu.C == xc );
	BOOST_TEST( cpu.X == xc );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2) * bdata::xrange(2),
					 old_z, new_z
	)
{
	cpu.Z = old_z;
	cpu.X = ! new_z;
	cpu.R[0] = 0;
	test_cpu( { 0044000 } );
	BOOST_TEST( cpu.Z == ( old_z && new_z ) );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_writedn, ea ) {
	test_ea_b(cpu, { 0044000 }, 0x34, ea, 0x66 );
}


BOOST_AUTO_TEST_SUITE_END();
