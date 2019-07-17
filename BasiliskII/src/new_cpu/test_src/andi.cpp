#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "init.h"
#include "../registers.hpp"
#include "../exceptions.hpp"
#include <vector>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
extern CPU xcpu;
void test_cpu(const std::vector<uint16_t>& c);
BOOST_TEST_GLOBAL_FIXTURE(init);
BOOST_FIXTURE_TEST_SUITE(andi_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.X = xcpu.V = xcpu.C = true;
	xcpu.D[0] = 0xa3;
	test_cpu( { 0001000, 0xd2 } );
	BOOST_TEST( xcpu.D[0] == ( 0xa3 & 0xd2 ) );
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.V == false );
	BOOST_TEST( xcpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0x80 : 1;
	test_cpu( { 0001000, 0xff } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	test_cpu( { 0001000, 0xff } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 0xa5;
	test_cpu( { (uint16_t)(0001000 | dst ), 0xdd } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
}


BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0001020 | dst ), 0xdd } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0001030 | dst ), 0xdd } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
	BOOST_TEST( xcpu.A[dst] == (dst == 7 ? 0x1002 : 0x1001) );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.A[dst] = dst == 7 ? 0x1002 : 0x1001;
	test_cpu( { (uint16_t)(0001040 | dst ), 0xdd } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1100, 0xa5);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0001050 | dst ), 0xdd, 0x100 } );
	BOOST_TEST( test_read8(0x1100) == 0x85 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1040, 0xa5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)(0001060 | dst ), 0xdd, 0x0830 } );
	BOOST_TEST( test_read8(0x1040) == 0x85 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write8(0x1000, 0xa5);
	test_cpu( { 0001070, 0xdd, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write8(0x1000, 0xa5);
	test_cpu( { 0001071, 0xdd, 0, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(andi_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.X = xcpu.V = xcpu.C = true;
	xcpu.D[0] = 0xa5a5;
	test_cpu( { 0001100, 0xdddd } );
	BOOST_TEST( xcpu.D[0] == ( 0xa5a5 & 0xdddd ) );
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.V == false );
	BOOST_TEST( xcpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0x8000 : 1;
	test_cpu( { 0001100, 0xffff } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	test_cpu( { 0001100, 0xffff } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 0xa5a5;
	test_cpu( { (uint16_t)(0001100 | dst ), 0xdddd } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
}


BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0001120 | dst ), 0xdddd } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0001130 | dst ), 0xdddd } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
	BOOST_TEST( xcpu.A[dst] == 0x1002 );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.A[dst] = 0x1002;
	test_cpu( { (uint16_t)(0001140 | dst ), 0xdddd } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1100, 0xa5a5);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0001150 | dst ), 0xdddd, 0x100 } );
	BOOST_TEST( test_read16(0x1100) == 0x8585 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1040, 0xa5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)(0001160 | dst ), 0xdddd, 0x0830 } );
	BOOST_TEST( test_read16(0x1040) == 0x8585 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write16(0x1000, 0xa5a5);
	test_cpu( { 0001170, 0xdddd, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write16(0x1000, 0xa5a5);
	test_cpu( { 0001171, 0xdddd, 0, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(andi_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.X = xcpu.V = xcpu.C = true;
	xcpu.D[0] = 0xa5a5a5a5;
	test_cpu( { 0001200, 0xdddd, 0xdddd } );
	BOOST_TEST( xcpu.D[0] == ( 0xa5a5a5a5 &  0xdddddddd ) );
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.V == false );
	BOOST_TEST( xcpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0001200, 0xffff, 0xffff } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	test_cpu( { 0001200, 0xffff, 0xffff } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 0xa5a5a5a5;
	test_cpu( { (uint16_t)(0001200 | dst ), 0xdddd, 0xdddd } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
}


BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0001220 | dst ), 0xdddd, 0xdddd } );
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0001230 | dst ), 0xdddd, 0xdddd } );
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
	BOOST_TEST( xcpu.A[dst] == 0x1004 );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1004;
	test_cpu( { (uint16_t)(0001240 | dst ), 0xdddd, 0xdddd } );
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1100, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0001250 | dst), 0xdddd, 0xdddd, 0x100 } );
	BOOST_TEST( test_read32(0x1100) == 0x85858585 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1040, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)(0001260 | dst), 0xdddd, 0xdddd, 0x0830 } );
	BOOST_TEST( test_read32(0x1040) == 0x85858585 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write32(0x1000, 0xa5a5a5a5);
	test_cpu( { 0001270, 0xdddd, 0xdddd, 0x1000 });
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write32(0x1000, 0xa5a5a5a5);
	test_cpu( { 0001271, 0xdddd, 0xdddd, 0, 0x1000 });
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(andi_ccr, fixture);
BOOST_DATA_TEST_CASE(test,
					 bdata::xrange(2) * bdata::xrange(2) * bdata::xrange(2) *
					 bdata::xrange(2) * bdata::xrange(2),
					 x,n,z,v,c
	)
{
	xcpu.X = xcpu.N = xcpu.Z = xcpu.V = xcpu.C = true;
	uint16_t f = x << 4 | n << 3 | z << 2 | v << 1 | c;
	test_cpu( { 0001074, f } );
	BOOST_TEST( xcpu.X == x );
	BOOST_TEST( xcpu.N == n );
	BOOST_TEST( xcpu.Z == z );
	BOOST_TEST( xcpu.V == v );
	BOOST_TEST( xcpu.C == c );
}
BOOST_AUTO_TEST_SUITE_END();
