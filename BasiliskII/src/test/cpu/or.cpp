#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(or_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa3;
	cpu.R[1] = 0xd2 | 0xffffff00;
	test_cpu( { 0101000 } );
	BOOST_TEST( cpu.R[1] == (( 0xa3 | 0xd2 ) | 0xffffff00) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x80 : 1;
	cpu.R[1] = 0;
	test_cpu( { 0101000 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 0;
	cpu.R[1] = ! z;
	test_cpu( { 0101000 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	cpu.R[1] = 0xdd;
	test_ea_b(cpu, { 0101000 }, 0xa5, ea, 0xa5);
	BOOST_TEST( cpu.R[1] == 0xfd );
}


BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[1] = 0xdd;
	test_ea_b(cpu, { 0101400 }, 0xa5, ea, 0xfd);
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(or_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5;
	cpu.R[1] = 0xdddd;
	test_cpu( { 0101100 } );
	BOOST_TEST( cpu.R[1] == ( 0xa5a5 | 0xdddd ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x8000 : 1;
	cpu.R[1] = 0;
	test_cpu( { 0101100 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	cpu.R[1] = 0;
	test_cpu( { 0101100 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	cpu.R[1] = 0xdddd;
	test_ea_w(cpu, { 0101100 }, 0xa5a5, ea, 0xa5a5);
	BOOST_TEST( cpu.R[1] == 0xfdfd );
}


BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[1] = 0xdddd;
	test_ea_w(cpu, { 0101500 }, 0xa5a5, ea, 0xfdfd);
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(or_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	cpu.R[0] = 0xa5a5a5a5;
	cpu.R[1] = 0xdddddddd;
	test_cpu( { 0101200 } );
	BOOST_TEST( cpu.R[1] == ( 0xa5a5a5a5 |  0xdddddddd ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	cpu.R[1] = 0;
	test_cpu( { 0101200 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	cpu.R[1] = 0;
	test_cpu( { 0101200 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	cpu.R[1] = 0xdddddddd;
	test_ea_l(cpu, { 0101200 }, 0xa5a5a5a5, ea, 0xa5a5a5a5);
	BOOST_TEST( cpu.R[1] == 0xfdfdfdfd );
}


BOOST_DATA_TEST_CASE(to_ea, ea_write2, ea)
{
	cpu.R[1] = 0xdddddddd;
	test_ea_l(cpu, { 0101600 }, 0xa5a5a5a5, ea, 0xfdfdfdfd);
}
BOOST_AUTO_TEST_SUITE_END();
