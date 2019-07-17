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
BOOST_FIXTURE_TEST_SUITE(cas_byte, fixture);
BOOST_AUTO_TEST_CASE( result_z)  {
	xcpu.X = true;
	xcpu.A[0] = 0x1000;
	test_write8(0x1000, 23 );
	xcpu.D[0] = 23;
	xcpu.D[1] = 34;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read8(0x1000) ==  34 );
	BOOST_TEST( xcpu.D[0] == 23 );
	BOOST_TEST( xcpu.Z == true );
	BOOST_TEST( xcpu.X == true );
}
BOOST_AUTO_TEST_CASE( result_nz)  {
	xcpu.A[0] = 0x1000;
	test_write8(0x1000, 23 );
	xcpu.D[0] = 22;
	xcpu.D[1] = 34;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read8(0x1000) == 23 );
	BOOST_TEST( xcpu.D[0] == 23 );
	BOOST_TEST( xcpu.Z == false );
}
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	xcpu.A[0] = 0x1000;
	test_write8(0x1000, 0 );
	xcpu.D[0] = c;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.A[0] = 0x1000;
	test_write8(0x1000, 0x80 );
	xcpu.D[0] = v;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.A[0] = 0x1000;
	test_write8(0x1000, 0x7f );
	xcpu.D[0] = 0x7f + v;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.A[0] = 0x1000;
	test_write8(0x1000, 0 );
	xcpu.D[0] = n ? 1 : 0;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1000;
	test_write8(0x1000, 23 );
	xcpu.D[0] = 23;
	xcpu.D[1] = 34;
	test_cpu( { (uint16_t)( 0005320 | dst ), (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read8(0x1000) == 34 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1000;
	test_write8(0x1000, 23 );
	xcpu.D[0] = 23;
	xcpu.D[1] = 34;
	test_cpu( { (uint16_t)( 0005330 | dst ), (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read8(0x1000) == 34 );
	BOOST_TEST( xcpu.A[dst] == (dst == 7 ? 0x1002 : 0x1001) );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = dst == 7 ? 0x1002 : 0x1001;
	test_write8(0x1000, 23 );
	xcpu.D[0] = 23;
	xcpu.D[1] = 34;
	test_cpu( { (uint16_t)( 0005340 | dst ), (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read8(0x1000) == 34 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1000;
	test_write8(0x1010, 23 );
	xcpu.D[0] = 23;
	xcpu.D[1] = 34;
	test_cpu( { (uint16_t)( 0005350 | dst ), (uint16_t)(1 << 6), 0x10 } );
	BOOST_TEST( test_read8(0x1010) == 34 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1040, 23);
	xcpu.A[dst] = 0x1000;
	xcpu.D[2] = 23;
	xcpu.D[1] = 34;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)( 0005360 | dst ), (uint16_t)(1 << 6 | 2),
				0x0830 } );
	BOOST_TEST( test_read8(0x1040) == 34 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write8(0x1000, 23);
	xcpu.D[0] = 23;
	xcpu.D[1] = 34;
	test_cpu( { 0005370, (uint16_t)(1 << 6), 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 34 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write8(0x1000, 23);
	xcpu.D[0] = 23;
	xcpu.D[1] = 34;
	test_cpu( { 0005371, (uint16_t)(1 << 6), 0, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 34 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cas_word, fixture);
BOOST_AUTO_TEST_CASE( result_z)  {
	xcpu.X = true;
	xcpu.A[0] = 0x1000;
	test_write16(0x1000, 2323 );
	xcpu.D[0] = 2323;
	xcpu.D[1] = 3434;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read16(0x1000) ==  3434 );
	BOOST_TEST( xcpu.D[0] == 2323 );
	BOOST_TEST( xcpu.Z == true );
	BOOST_TEST( xcpu.X == true );
}
BOOST_AUTO_TEST_CASE( result_nz)  {
	xcpu.A[0] = 0x1000;
	test_write16(0x1000, 2323 );
	xcpu.D[0] = 2222;
	xcpu.D[1] = 3434;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read16(0x1000) == 2323 );
	BOOST_TEST( xcpu.D[0] == 2323 );
	BOOST_TEST( xcpu.Z == false );
}
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	xcpu.A[0] = 0x1000;
	test_write16(0x1000, 0 );
	xcpu.D[0] = c;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.A[0] = 0x1000;
	test_write16(0x1000, 0x8000 );
	xcpu.D[0] = v;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.A[0] = 0x1000;
	test_write16(0x1000, 0x7fff );
	xcpu.D[0] = 0x7fff + v;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.A[0] = 0x1000;
	test_write16(0x1000, 0 );
	xcpu.D[0] = n ? 1 : 0;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1000;
	test_write16(0x1000, 2323 );
	xcpu.D[0] = 2323;
	xcpu.D[1] = 3434;
	test_cpu( { (uint16_t)( 0006320 | dst ), (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read16(0x1000) == 3434 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1000;
	test_write16(0x1000, 2323 );
	xcpu.D[0] = 2323;
	xcpu.D[1] = 3434;
	test_cpu( { (uint16_t)( 0006330 | dst ), (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read16(0x1000) == 3434 );
	BOOST_TEST( xcpu.A[dst] == 0x1002 );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1002;
	test_write16(0x1000, 2323 );
	xcpu.D[0] = 2323;
	xcpu.D[1] = 3434;
	test_cpu( { (uint16_t)( 0006340 | dst ), (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read16(0x1000) == 3434 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1000;
	test_write16(0x1010, 2323 );
	xcpu.D[0] = 2323;
	xcpu.D[1] = 3434;
	test_cpu( { (uint16_t)( 0006350 | dst ), (uint16_t)(1 << 6), 0x10 } );
	BOOST_TEST( test_read16(0x1010) == 3434 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1040, 2323);
	xcpu.A[dst] = 0x1000;
	xcpu.D[2] = 2323;
	xcpu.D[1] = 3434;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)( 0006360 | dst ), (uint16_t)(1 << 6 | 2),
				0x0830 } );
	BOOST_TEST( test_read16(0x1040) == 3434 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write16(0x1000, 2323);
	xcpu.D[0] = 2323;
	xcpu.D[1] = 3434;
	test_cpu( { 0006370, (uint16_t)(1 << 6), 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 3434 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write16(0x1000, 2323);
	xcpu.D[0] = 2323;
	xcpu.D[1] = 3434;
	test_cpu( { 0006371, (uint16_t)(1 << 6), 0, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 3434 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cas_long, fixture);

BOOST_AUTO_TEST_CASE( result_z)  {
	xcpu.X = true;
	xcpu.A[0] = 0x1000;
	test_write32(0x1000, 232323 );
	xcpu.D[0] = 232323;
	xcpu.D[1] = 343434;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read32(0x1000) ==  343434 );
	BOOST_TEST( xcpu.D[0] == 232323 );
	BOOST_TEST( xcpu.Z == true );
	BOOST_TEST( xcpu.X == true );
}
BOOST_AUTO_TEST_CASE( result_nz)  {
	xcpu.A[0] = 0x1000;
	test_write32(0x1000, 232323 );
	xcpu.D[0] = 222222;
	xcpu.D[1] = 343434;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read32(0x1000) == 232323 );
	BOOST_TEST( xcpu.D[0] == 232323 );
	BOOST_TEST( xcpu.Z == false );
}
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	xcpu.A[0] = 0x1000;
	test_write32(0x1000, 0 );
	xcpu.D[0] = c;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.A[0] = 0x1000;
	test_write32(0x1000, 0x80000000 );
	xcpu.D[0] = v;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.A[0] = 0x1000;
	test_write32(0x1000, 0x7fffffff );
	xcpu.D[0] = 0x7fffffff + v;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.A[0] = 0x1000;
	test_write32(0x1000, 0 );
	xcpu.D[0] = n ? 1 : 0;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1000;
	test_write32(0x1000, 232323 );
	xcpu.D[0] = 232323;
	xcpu.D[1] = 343434;
	test_cpu( { (uint16_t)( 0007320 | dst ), (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read32(0x1000) == 343434 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1000;
	test_write32(0x1000, 232323 );
	xcpu.D[0] = 232323;
	xcpu.D[1] = 343434;
	test_cpu( { (uint16_t)( 0007330 | dst ), (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read32(0x1000) == 343434 );
	BOOST_TEST( xcpu.A[dst] == 0x1004 );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1004;
	test_write32(0x1000, 232323 );
	xcpu.D[0] = 232323;
	xcpu.D[1] = 343434;
	test_cpu( { (uint16_t)( 0007340 | dst ), (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read32(0x1000) == 343434 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x1000;
	test_write32(0x1010, 232323 );
	xcpu.D[0] = 232323;
	xcpu.D[1] = 343434;
	test_cpu( { (uint16_t)( 0007350 | dst ), (uint16_t)(1 << 6), 0x10 } );
	BOOST_TEST( test_read32(0x1010) == 343434 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1040, 232323);
	xcpu.A[dst] = 0x1000;
	xcpu.D[2] = 232323;
	xcpu.D[1] = 343434;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)( 0007360 | dst ), (uint16_t)(1 << 6 | 2),
				0x0830 } );
	BOOST_TEST( test_read32(0x1040) == 343434 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write32(0x1000, 232323);
	xcpu.D[0] = 232323;
	xcpu.D[1] = 343434;
	test_cpu( { 0007370, (uint16_t)(1 << 6), 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 343434 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write32(0x1000, 232323);
	xcpu.D[0] = 232323;
	xcpu.D[1] = 343434;
	test_cpu( { 0007371, (uint16_t)(1 << 6), 0, 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 343434 );
}

BOOST_AUTO_TEST_SUITE_END();
