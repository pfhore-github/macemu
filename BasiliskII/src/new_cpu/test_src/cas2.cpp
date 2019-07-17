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
BOOST_FIXTURE_TEST_SUITE(cas2_word, fixture);
BOOST_AUTO_TEST_CASE( result_zz)  {
	xcpu.X = true;
	test_write16(0x1000, 2222);
	test_write16(0x1100, 3333);
	xcpu.D[0] = 2222;
	xcpu.D[1] = 3333;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 4444;
	xcpu.D[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( test_read16(0x1000) ==  4444 );
	BOOST_TEST( test_read16(0x1100) ==  5555 );
	BOOST_TEST( xcpu.D[0] == 2222 );
	BOOST_TEST( xcpu.D[1] == 3333 );
	BOOST_TEST( xcpu.Z == true );
	BOOST_TEST( xcpu.X == true );
	
}

BOOST_AUTO_TEST_CASE( result_zx)  {
	test_write16(0x1000, 2222);
	test_write16(0x1100, 3335);
	xcpu.D[0] = 2222;
	xcpu.D[1] = 3333;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 4444;
	xcpu.D[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( test_read16(0x1000) ==  2222 );
	BOOST_TEST( test_read16(0x1100) ==  3335 );
	BOOST_TEST( xcpu.D[0] == 2222 );
	BOOST_TEST( xcpu.D[1] == 3335 );
	BOOST_TEST( xcpu.Z == false );
	
}

BOOST_AUTO_TEST_CASE( result_x)  {
	test_write16(0x1000, 2226);
	test_write16(0x1100, 3333);
	xcpu.D[0] = 2222;
	xcpu.D[1] = 3333;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 4444;
	xcpu.D[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( test_read16(0x1000) ==  2226 );
	BOOST_TEST( test_read16(0x1100) ==  3333 );
	BOOST_TEST( xcpu.D[0] == 2226 );
	BOOST_TEST( xcpu.D[1] == 3333 );
	BOOST_TEST( xcpu.Z == false );
	
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write16(0x1000, 2222);
	test_write16(0x1100, 3333-n);
	xcpu.D[0] = 2222;
	xcpu.D[1] = 3333;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 4444;
	xcpu.D[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( xcpu.N == n );
	
}

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	test_write16(0x1000, 2222);
	test_write16(0x1100, 0);
	xcpu.D[0] = 2222;
	xcpu.D[1] = c;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 4444;
	xcpu.D[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( xcpu.C == c );
	
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	test_write16(0x1000, 2222);
	test_write16(0x1100, 0x8000);
	xcpu.D[0] = 2222;
	xcpu.D[1] = v;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 4444;
	xcpu.D[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( xcpu.V == v );
	
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	test_write16(0x1000, 2222);
	test_write16(0x1100, 0x7fff);
	xcpu.D[0] = 2222;
	xcpu.D[1] = 0x7fff+v;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 4444;
	xcpu.D[3] = 5555;
	test_cpu( { 0006374, 0100200, 0110301 } );
	BOOST_TEST( xcpu.V == v );
	
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cas2_long, fixture);
BOOST_AUTO_TEST_CASE( result_zz)  {
	xcpu.X = true;
	test_write32(0x1000, 222222);
	test_write32(0x1100, 333333);
	xcpu.D[0] = 222222;
	xcpu.D[1] = 333333;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 444444;
	xcpu.D[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( test_read32(0x1000) ==  444444 );
	BOOST_TEST( test_read32(0x1100) ==  555555 );
	BOOST_TEST( xcpu.D[0] == 222222 );
	BOOST_TEST( xcpu.D[1] == 333333 );
	BOOST_TEST( xcpu.Z == true );
	BOOST_TEST( xcpu.X == true );
	
}

BOOST_AUTO_TEST_CASE( result_zx)  {
	test_write32(0x1000, 222222);
	test_write32(0x1100, 333335);
	xcpu.D[0] = 222222;
	xcpu.D[1] = 333333;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 444444;
	xcpu.D[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( test_read32(0x1000) ==  222222 );
	BOOST_TEST( test_read32(0x1100) ==  333335 );
	BOOST_TEST( xcpu.D[0] == 222222 );
	BOOST_TEST( xcpu.D[1] == 333335 );
	
}

BOOST_AUTO_TEST_CASE( result_x)  {
	test_write32(0x1000, 222226);
	test_write32(0x1100, 333333);
	xcpu.D[0] = 222222;
	xcpu.D[1] = 333333;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 444444;
	xcpu.D[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( test_read32(0x1000) ==  222226 );
	BOOST_TEST( test_read32(0x1100) ==  333333 );
	BOOST_TEST( xcpu.D[0] == 222226 );
	BOOST_TEST( xcpu.D[1] == 333333 );
	BOOST_TEST( xcpu.Z == false );
	
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write32(0x1000, 222222);
	test_write32(0x1100, 333333-n);
	xcpu.D[0] = 222222;
	xcpu.D[1] = 333333;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 444444;
	xcpu.D[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( xcpu.N == n );
	
}

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	test_write32(0x1000, 222222);
	test_write32(0x1100, 0);
	xcpu.D[0] = 222222;
	xcpu.D[1] = c;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 444444;
	xcpu.D[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( xcpu.C == c );
	
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	test_write32(0x1000, 222222);
	test_write32(0x1100, 0x80000000);
	xcpu.D[0] = 222222;
	xcpu.D[1] = v;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 444444;
	xcpu.D[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( xcpu.V == v );
	
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	test_write32(0x1000, 222222);
	test_write32(0x1100, 0x7fffffff);
	xcpu.D[0] = 222222;
	xcpu.D[1] = 0x7fffffff+v;
	xcpu.A[0] = 0x1000;
	xcpu.A[1] = 0x1100;
	xcpu.D[2] = 444444;
	xcpu.D[3] = 555555;
	test_cpu( { 0007374, 0100200, 0110301 } );
	BOOST_TEST( xcpu.V == v );
	
}

BOOST_AUTO_TEST_SUITE_END();
