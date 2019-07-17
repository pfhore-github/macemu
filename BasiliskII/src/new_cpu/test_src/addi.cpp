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
BOOST_FIXTURE_TEST_SUITE(addi_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.D[0] = 23;
	test_cpu( { 0003000, 34 } );
	BOOST_TEST( xcpu.D[0] == ( 23 + 34 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = xc;
	test_cpu( { 0003000, 0xff } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v;
	test_cpu( { 0003000, 0x7f } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7f + v;
	test_cpu( { 0003000, 0x80 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0 : 1;
	xcpu.D[1] = 1;
	test_cpu( { 0003000, 1 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	test_cpu( { 0003000, 0 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(8) ,
					 dst)
{
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0003000 | dst), 23 } );
	BOOST_TEST( xcpu.D[dst] == 57 );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 23);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0003020 | dst), 34 } );
	BOOST_TEST( test_read8(0x1000) == 57 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 23);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0003030 | dst), 34 } );
	BOOST_TEST( test_read8(0x1000) == 57 );
	BOOST_TEST( xcpu.A[dst] == (dst == 7 ? 0x1002 : 0x1001) );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 23);
	xcpu.A[dst] = dst == 7 ? 0x1002 : 0x1001;
	test_cpu( { (uint16_t)(0003040 |  dst), 34 } );
	BOOST_TEST( test_read8(0x1000) == 57 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1100, 23);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0003050 | dst), 34, 0x100 } );
	BOOST_TEST( test_read8(0x1100) == 57 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1040, 23);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)(0003060 | dst), 34, 0x0830 } );
	BOOST_TEST( test_read8(0x1040) == 57 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write8(0x1000, 23);
	test_cpu( { 0003070, 34, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 57 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write8(0x1000, 23);
	test_cpu( { 0003071, 34, 0, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 57 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addi_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.D[0] = 2300;
	test_cpu( { 0003100, 3400 } );
	BOOST_TEST( xcpu.D[0] == ( 2300 + 3400 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = xc;
	test_cpu( { 0003100, 0xffff } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v;
	test_cpu( { 0003100, 0x7fff } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7fff + v;
	test_cpu( { 0003100, 0x8000 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf000 : 1;
	test_cpu( { 0003100, 1 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	test_cpu( { 0003100, 0 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 2300;
	test_cpu( { (uint16_t)(0003100 | dst), 3400 } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0003120 | dst), 3400 } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0003130 | dst), 3400 } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
	BOOST_TEST( xcpu.A[dst] == 0x1002 );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[dst] = 0x1002;
	test_cpu( { (uint16_t)(0003140 | dst), 3400 } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1100, 2300);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0003150 | dst), 3400, 0x100 } );
	BOOST_TEST( test_read16(0x1100) == 5700 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1040, 2300);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)(0003160 | dst), 3400, 
				0x0830 } );
	BOOST_TEST( test_read16(0x1040) == 5700 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write16(0x1000, 2300);
	test_cpu( { 0003170, 3400, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write16(0x1000, 2300);
	test_cpu( { 0003171, 3400, 0, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addi_long, fixture);
constexpr uint16_t HIGH = uint16_t(340000>>16);
constexpr uint16_t LOW = uint16_t(340000 & 0xffff);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.D[0] = 230000;
	xcpu.D[1] = 340000;
	test_cpu( { 0003200, HIGH, LOW } );
	BOOST_TEST( xcpu.D[0] == ( 230000 + 340000 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = xc;
	test_cpu( { 0003200, 0xffff, 0xffff } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v;
	test_cpu( { 0003200, 0x7fff, 0xffff } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7fffffff + v;
	test_cpu( { 0003200, 0x8000, 0 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0003200, 0, 1 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	test_cpu( { 0003200, 0, 0 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 230000;
	test_cpu( { uint16_t(0003200 | dst), HIGH, LOW } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[dst] = 0x1000;
	test_cpu( { uint16_t(0003220 | dst), HIGH, LOW } );
	BOOST_TEST( test_read32(0x1000) == 570000 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[dst] = 0x1000;
	test_cpu( { uint16_t(0003230 | dst), HIGH, LOW } );
	BOOST_TEST( test_read32(0x1000) == 570000 );
	BOOST_TEST( xcpu.A[dst] == 0x1004 );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[dst] = 0x1004;
	test_cpu( { uint16_t(0003240 | dst), HIGH, LOW } );
	BOOST_TEST( test_read32(0x1000) == 570000 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1100, 230000);
	xcpu.A[dst] = 0x1000;
	test_cpu( { uint16_t(0003250 | dst), HIGH, LOW, 0x100} );
	BOOST_TEST( test_read32(0x1100) == 570000 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1040, 230000);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { uint16_t(0003260 | dst), HIGH, LOW, 0x0830} );
	BOOST_TEST( test_read32(0x1040) == 570000 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write32(0x1000, 230000);
	test_cpu( { 0003270, HIGH, LOW, 0x1000} );
	BOOST_TEST( test_read32(0x1000) == 570000 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write32(0x1000, 230000);
	test_cpu( { 0003271, HIGH, LOW, 0, 0x1000} );
	BOOST_TEST( test_read32(0x1000) == 570000 );
}

BOOST_AUTO_TEST_SUITE_END();
