#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(ext_w, fixture);
BOOST_DATA_TEST_CASE( result, bdata::xrange(2), sign )  {
	cpu.V = cpu.C = true;
	cpu.R[2] = ( sign ? 0xc0 : 0x20 ) | 0xffffff00;
	test_cpu( { 0044202 } );
	BOOST_TEST( cpu.R[2] == ((sign ? 0xffc0 : 0x20 ) | 0xffff0000 ) );
	BOOST_TEST( cpu.N == sign );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );	
}

BOOST_AUTO_TEST_CASE( z )  {
	cpu.R[2] = 0;
	test_cpu( { 0044202 } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(ext_l, fixture);
BOOST_DATA_TEST_CASE( result, bdata::xrange(2), sign )  {
	cpu.V = cpu.C = true;
	cpu.R[2] = ( sign ? 0xc000 : 0x2000 );
	test_cpu( { 0044302 } );
	BOOST_TEST( cpu.R[2] == (sign ? 0xffffc000 : 0x2000 ));
	BOOST_TEST( cpu.N == sign );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );	
}

BOOST_AUTO_TEST_CASE( z )  {
	cpu.R[2] = 0;
	test_cpu( { 0044302 } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_SUITE_END();


BOOST_FIXTURE_TEST_SUITE(extb_l, fixture);
BOOST_DATA_TEST_CASE( result, bdata::xrange(2), sign )  {
	cpu.V = cpu.C = true;
	cpu.R[2] = ( sign ? 0xc0 : 0x20 ) | 0xffffff00;
	test_cpu( { 0044702 } );
	BOOST_TEST( cpu.R[2] == (sign ? 0xffffffc0 : 0x20 ));
	BOOST_TEST( cpu.N == sign );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );	
}

BOOST_AUTO_TEST_CASE( z )  {
	cpu.R[2] = 0;
	test_cpu( { 0044702 } );
	BOOST_TEST( cpu.Z == true );
}

BOOST_AUTO_TEST_SUITE_END();
