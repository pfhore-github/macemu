#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(cas2_word, fixture);
BOOST_AUTO_TEST_CASE( result_zz)  {
	test_write16(0x1000, 2222);
	test_write16(0x1100, 3333);
	cpu.R[0] = 2222;
	cpu.R[1] = 3333;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 4444;
	cpu.R[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( test_read16(0x1000) ==  4444 );
	BOOST_TEST( test_read16(0x1100) ==  5555 );
	BOOST_TEST( cpu.R[0] == 2222 );
	BOOST_TEST( cpu.R[1] == 3333 );
	BOOST_TEST( cpu.Z == true );
	
}

BOOST_AUTO_TEST_CASE( result_zx)  {
	test_write16(0x1000, 2222);
	test_write16(0x1100, 3335);
	cpu.R[0] = 2222;
	cpu.R[1] = 3333;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 4444;
	cpu.R[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( test_read16(0x1000) ==  2222 );
	BOOST_TEST( test_read16(0x1100) ==  3335 );
	BOOST_TEST( cpu.R[0] == 2222 );
	BOOST_TEST( cpu.R[1] == 3335 );
	BOOST_TEST( cpu.Z == false );
	
}

BOOST_AUTO_TEST_CASE( result_x)  {
	test_write16(0x1000, 2226);
	test_write16(0x1100, 3333);
	cpu.R[0] = 2222;
	cpu.R[1] = 3333;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 4444;
	cpu.R[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( test_read16(0x1000) ==  2226 );
	BOOST_TEST( test_read16(0x1100) ==  3333 );
	BOOST_TEST( cpu.R[0] == 2226 );
	BOOST_TEST( cpu.R[1] == 3333 );
	BOOST_TEST( cpu.Z == false );
	
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write16(0x1000, 2222);
	test_write16(0x1100, 3333-n);
	cpu.R[0] = 2222;
	cpu.R[1] = 3333;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 4444;
	cpu.R[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( cpu.N == n );
	
}

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	test_write16(0x1000, 2222);
	test_write16(0x1100, 0);
	cpu.R[0] = 2222;
	cpu.R[1] = c;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 4444;
	cpu.R[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( cpu.C == c );
	
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	test_write16(0x1000, 2222);
	test_write16(0x1100, 0x8000);
	cpu.R[0] = 2222;
	cpu.R[1] = v;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 4444;
	cpu.R[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( cpu.V == v );
	
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	test_write16(0x1000, 2222);
	test_write16(0x1100, 0x7fff);
	cpu.R[0] = 2222;
	cpu.R[1] = 0x7fff+v;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 4444;
	cpu.R[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( cpu.V == v );
	
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cas2_long, fixture);
BOOST_AUTO_TEST_CASE( result_zz)  {
	cpu.X = true;
	test_write32(0x1000, 222222);
	test_write32(0x1100, 333333);
	cpu.R[0] = 222222;
	cpu.R[1] = 333333;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 444444;
	cpu.R[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( test_read32(0x1000) ==  444444 );
	BOOST_TEST( test_read32(0x1100) ==  555555 );
	BOOST_TEST( cpu.R[0] == 222222 );
	BOOST_TEST( cpu.R[1] == 333333 );
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( cpu.X == true );
	
}

BOOST_AUTO_TEST_CASE( result_zx)  {
	test_write32(0x1000, 222222);
	test_write32(0x1100, 333335);
	cpu.R[0] = 222222;
	cpu.R[1] = 333333;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 444444;
	cpu.R[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( test_read32(0x1000) ==  222222 );
	BOOST_TEST( test_read32(0x1100) ==  333335 );
	BOOST_TEST( cpu.R[0] == 222222 );
	BOOST_TEST( cpu.R[1] == 333335 );
	
}

BOOST_AUTO_TEST_CASE( result_x)  {
	test_write32(0x1000, 222226);
	test_write32(0x1100, 333333);
	cpu.R[0] = 222222;
	cpu.R[1] = 333333;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 444444;
	cpu.R[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( test_read32(0x1000) ==  222226 );
	BOOST_TEST( test_read32(0x1100) ==  333333 );
	BOOST_TEST( cpu.R[0] == 222226 );
	BOOST_TEST( cpu.R[1] == 333333 );
	BOOST_TEST( cpu.Z == false );
	
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write32(0x1000, 222222);
	test_write32(0x1100, 333333-n);
	cpu.R[0] = 222222;
	cpu.R[1] = 333333;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 444444;
	cpu.R[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( cpu.N == n );
	
}

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	test_write32(0x1000, 222222);
	test_write32(0x1100, 0);
	cpu.R[0] = 222222;
	cpu.R[1] = c;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 444444;
	cpu.R[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( cpu.C == c );
	
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	test_write32(0x1000, 222222);
	test_write32(0x1100, 0x80000000);
	cpu.R[0] = 222222;
	cpu.R[1] = v;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 444444;
	cpu.R[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( cpu.V == v );
	
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	test_write32(0x1000, 222222);
	test_write32(0x1100, 0x7fffffff);
	cpu.R[0] = 222222;
	cpu.R[1] = 0x7fffffffU+v;
	cpu.R[8+0] = 0x1000;
	cpu.R[8+1] = 0x1100;
	cpu.R[2] = 444444;
	cpu.R[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( cpu.V == v );
	
}

BOOST_AUTO_TEST_SUITE_END();
