#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;


BOOST_FIXTURE_TEST_SUITE(clr_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.N = cpu.V = cpu.C = true;
	cpu.Z = false;
	cpu.R[0] = 23;
	test_cpu( { 0041000 } );
	BOOST_TEST( cpu.R[0] == 0 );
	BOOST_TEST( cpu.N == false );
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(to_ea, ea_writedn, ea)
{
	test_ea_b(cpu, { 0041000 }, 34, ea, 0 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(clr_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.N = cpu.V = cpu.C = true;
	cpu.Z = false;
	cpu.R[0] = 2300;
	test_cpu( { 0041100 } );
	BOOST_TEST( cpu.R[0] == 0 );
	BOOST_TEST( cpu.N == false );
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(to_ea, ea_writedn, ea)
{
	test_ea_w(cpu, { 0041100 }, 3400, ea, 0 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(clr_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.N = cpu.V = cpu.C = true;
	cpu.Z = false;
	cpu.R[0] = 230000;
	test_cpu( { 0041200 } );
	BOOST_TEST( cpu.R[0] == 0 );
	BOOST_TEST( cpu.N == false );
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(to_ea, ea_writedn, ea)
{
	test_ea_l(cpu, { 0041200 }, 340000, ea, 0 );
}

BOOST_AUTO_TEST_SUITE_END();
