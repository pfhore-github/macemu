#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(ori_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa3 | 0xffffff00;
	test_cpu( { 0000000, 0xd2 } );
	BOOST_TEST( cpu.R[0] == ( ( 0xa3 | 0xd2 ) | 0xffffff00) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x80 : 1;
	test_cpu( { 0000000, 0 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0000000, 0 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	test_ea_b(cpu, { 0000000, 0xdd }, 0xa5, ea, 0xfd );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(ori_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5 | 0xffff0000;
	test_cpu( { 0000100, 0xdddd } );
	BOOST_TEST( cpu.R[0] == (( 0xa5a5 | 0xdddd ) | 0xffff0000) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x8000 : 1;
	test_cpu( { 0000100, 0 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0000100, 0 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	test_ea_w(cpu, { 0000100, 0xdddd }, 0xa5a5, ea, 0xfdfd );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(ori_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5a5a5;
	test_cpu( { 0000200, 0xdddd, 0xdddd } );
	BOOST_TEST( cpu.R[0] == ( 0xa5a5a5a5 |  0xdddddddd ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0000200, 0, 0 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0000200, 0, 0 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	test_ea_l(cpu, { 0000200, 0xdddd, 0xdddd },
			  0xa5a5a5a5, ea, 0xfdfdfdfd );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(ori_ccr, fixture);
BOOST_DATA_TEST_CASE(test,
					 bdata::xrange(2) * bdata::xrange(2) * bdata::xrange(2) *
					 bdata::xrange(2) * bdata::xrange(2),
					 x,n,z,v,c
	)
{
	cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = false;
	uint16_t f = x << 4 | n << 3 | z << 2 | v << 1 | c;
	test_cpu( { 0000074, f } );
	BOOST_TEST( cpu.X == x );
	BOOST_TEST( cpu.N == n );
	BOOST_TEST( cpu.Z == z );
	BOOST_TEST( cpu.V == v );
	BOOST_TEST( cpu.C == c );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(ori_sr, fixture);
BOOST_AUTO_TEST_CASE(user)
{
	cpu.S = false;
	cpu.X = cpu.N = cpu.Z = cpu.V = cpu.C = false;
	test_cpu( { 0000174, 0xffff } );
	test_in_exception(8) ;
	BOOST_TEST( cpu.X == false );
	BOOST_TEST( cpu.N == false );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}
BOOST_AUTO_TEST_CASE(system)
{
	cpu.S = true;
	cpu.T =0;
	cpu.M = false;
	cpu.intmask = 0;
	test_cpu( { 0000174, 0xd700 } );
	test_in_exception(0);
	BOOST_TEST( cpu.T == 3 );
	BOOST_TEST( cpu.M == true );
	BOOST_TEST( cpu.intmask == 7 );
}

BOOST_AUTO_TEST_SUITE_END();
