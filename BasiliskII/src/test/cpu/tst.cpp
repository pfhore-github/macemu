#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(tst_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	test_cpu( { 0045000 } );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x80 : 1;
	test_cpu( { 0045000 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0045000 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(from_ea, ea_all, ea)
{
	test_ea_b(cpu, { 0045000 }, 1, ea, 1);
	BOOST_TEST( cpu.Z == false );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(tst_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	test_cpu( { 0045100 } );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0x8000 : 1;
	test_cpu( { 0045100 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0045100 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	cpu.R[1] = 0xdddd;
	test_ea_w(cpu, { 0045100 }, 1, ea, 1);
	BOOST_TEST( cpu.Z == false );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(tst_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = cpu.C = true;
	test_cpu( { 0045200 } );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0045200 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = ! z;
	test_cpu( { 0045200 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_DATA_TEST_CASE(from_ea, ea_data, ea)
{
	test_ea_l(cpu, { 0045200 }, 1, ea, 1);
	BOOST_TEST( cpu.Z == false );
}

BOOST_AUTO_TEST_SUITE_END();
