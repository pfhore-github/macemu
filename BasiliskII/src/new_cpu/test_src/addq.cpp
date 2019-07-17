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
BOOST_FIXTURE_TEST_SUITE(addq_byte, fixture);

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0xfe + xc;
	test_cpu( { 0051000 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7e + v;
	test_cpu( { 0051000 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0 : 1;
	test_cpu( { 0051000 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = z * 0xff;
	test_cpu( { 0051000 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(8) * bdata::xrange(8) ,
					 q, dst)
{
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0050000 | (q << 9) | dst)} );
	BOOST_TEST( xcpu.D[dst] == 34 + (q ? q : 8) );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8) * bdata::xrange(8) ,
					 q, dst)
{
	test_write8(0x1000, 23);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0050020 | (q << 9) | dst)} );
	BOOST_TEST( test_read8(0x1000) == 23 + (q ? q : 8) );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8) * bdata::xrange(8) ,
					 q, dst)
{
	test_write8(0x1000, 23);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0050030 | (q << 9) | dst)} );
	BOOST_TEST( test_read8(0x1000) == 23 + (q ? q : 8) );
	BOOST_TEST( xcpu.A[dst] == ( dst == 7 ? 0x1002 : 0x1001 ) );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8) * bdata::xrange(8) ,
					 q, dst)
{
	test_write8(0x1000, 23);
	xcpu.A[dst] = dst == 7 ? 0x1002 : 0x1001;
	test_cpu( { (uint16_t)(0050040 | (q << 9) | dst)} );
	BOOST_TEST( test_read8(0x1000) == 23 + (q ? q : 8) );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8) * bdata::xrange(8) ,
					 q, dst)
{
	test_write8(0x1100, 23);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0050050 | (q << 9) | dst), 0x100 } );
	BOOST_TEST( test_read8(0x1100) == 23 + (q ? q : 8) );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8) * bdata::xrange(8) ,
					 q, dst)
{
	test_write8(0x1040, 23);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)(0050060 | (q << 9) | dst), 0x0830 } );
	BOOST_TEST( test_read8(0x1040) == 23 + (q ? q : 8) );
}

BOOST_DATA_TEST_CASE(xxxw,
					 bdata::xrange(8),
					 q)
{
	test_write8(0x1000, 23);
	test_cpu( { (uint16_t)(0050070 | q << 9), 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 23 + (q ? q : 8) );
}

BOOST_DATA_TEST_CASE(xxxl,
					 bdata::xrange(8),
					 q)

{
	test_write8(0x1000, 23);
	test_cpu( { (uint16_t)(0050071 | q << 9), 0, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 23 + (q ? q : 8) );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addq_word, fixture);

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0xfffe + xc;
	test_cpu( { 0051100 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7ffe + v;
	test_cpu( { 0051100 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf000 : 1;
	test_cpu( { 0051100 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = z * 0xffff;
	test_cpu( { 0051100 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	xcpu.D[dst] = 2300;
	test_cpu( { (uint16_t)(0050100 | q << 9 | dst) } );
	BOOST_TEST( xcpu.D[dst] == 2300 + ( q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(an,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	xcpu.A[dst] = 2300;
	test_cpu( { (uint16_t)(0050110 | q << 9 | dst) } );
	BOOST_TEST( xcpu.A[dst] == 2300 + ( q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0050120 | q << 9 | dst) } );
	BOOST_TEST( test_read16(0x1000) == 2300 + ( q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0050130 | q << 9 | dst) } );
	BOOST_TEST( test_read16(0x1000) == 2300 + ( q ? q : 8 ) );
	BOOST_TEST( xcpu.A[dst] == 0x1002 );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[dst] = 0x1002;
	test_cpu( { (uint16_t)(0050140 | q << 9 | dst) } );
	BOOST_TEST( test_read16(0x1000) == 2300 + (q ? q : 8 ) );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write16(0x1100, 2300);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0050150 | q << 9 | dst), 0x100 } );
	BOOST_TEST( test_read16(0x1100) == 2300 + (q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write16(0x1040, 2300);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)(0050160 | q << 9 | dst),
				0x0830 } );
	BOOST_TEST( test_read16(0x1040) == 2300 + (q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(xxxw, bdata::xrange(8), q)
{
	test_write16(0x1000, 2300);
	test_cpu( { (uint16_t)(0050170 | q << 9), 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 2300 + (q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(xxxl, bdata::xrange(8), q)
{
	test_write16(0x1000, 2300);
	test_cpu( { (uint16_t)(0050171 | q << 9), 0, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 2300 + (q ? q : 8 ) );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addq_long, fixture);

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0xfffffffe + xc;
	test_cpu( { 0051200 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7ffffffe + v;
	test_cpu( { 0051200 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0000000 : 1;
	test_cpu( { 0051200 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = z * 0xffffffff;
	test_cpu( { 0051200 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	xcpu.D[dst] = 230000;
	test_cpu( { (uint16_t)(0050200 | q << 9 | dst) } );
	BOOST_TEST( xcpu.D[dst] == 230000 + (q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(am,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	xcpu.A[dst] = 230000;
	test_cpu( { (uint16_t)(0050210 | q << 9 | dst) } );
	BOOST_TEST( xcpu.A[dst] == 230000 + (q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0050220 | q << 9 | dst) } );
	BOOST_TEST( test_read32(0x1000) == 230000 + (q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0050230 | q << 9 | dst) } );
	BOOST_TEST( test_read32(0x1000) == 230000 + (q ? q : 8 ) );
	BOOST_TEST( xcpu.A[dst] == 0x1004 );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[dst] = 0x1004;
	test_cpu( { (uint16_t)(0050240 | q << 9 | dst) } );
	BOOST_TEST( test_read32(0x1000) == 230000 + (q ? q : 8 ) );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write32(0x1100, 230000);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0050250 | q << 9 | dst), 0x100 } );
	BOOST_TEST( test_read32(0x1100) == 230000 + (q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8) * bdata::xrange(8),
					 q, dst)
{
	test_write32(0x1040, 230000);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)(0050260 | q << 9 | dst), 0x0830 } );
	BOOST_TEST( test_read32(0x1040) == 230000 + (q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(xxxw,
					 bdata::xrange(8),
					 q)
{
	test_write32(0x1000, 230000);
	test_cpu( { (uint16_t)(0050270 | q << 9 ), 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 230000 + (q ? q : 8 ) );
}

BOOST_DATA_TEST_CASE(xxxl,
					 bdata::xrange(8),
					 q)
{
	test_write32(0x1000, 230000);
	test_cpu( { (uint16_t)(0050271 | q << 9 ), 0, 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 230000 + (q ? q : 8 ) );
}

BOOST_AUTO_TEST_SUITE_END();
