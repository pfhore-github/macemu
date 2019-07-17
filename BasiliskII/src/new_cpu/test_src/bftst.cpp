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
BOOST_FIXTURE_TEST_SUITE(bftst_reg, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	xcpu.X = xcpu.N = xcpu.Z = xcpu.V = xcpu.C = true;
	xcpu.D[0] = 0x12345678;
	test_cpu({ 0164300, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.N == false );
	BOOST_TEST( xcpu.Z == false );
	BOOST_TEST( xcpu.V == false );
	BOOST_TEST( xcpu.C == false );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = 0x12045678 | (( n ? 0xa : 2 ) << 20);
	test_cpu({ 0164300, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = 0x12045678 | (( z ? 0 : 2 ) << 20);
	test_cpu({ 0164300, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( xcpu.Z == z );
}

BOOST_AUTO_TEST_CASE(offset_reg)
{
	xcpu.D[1] = 8;
	xcpu.D[0] = 0x12045678;
	test_cpu({ 0164300, (uint16_t)(1 << 11 | 1 << 6 | 4) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(width_reg)
{
	xcpu.D[1] = 4;
	xcpu.D[0] = 0x12045678;
	test_cpu({ 0164300, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(width32)
{
	xcpu.D[1] = 4;
	xcpu.D[0] = 0x12345678;
	test_cpu({ 0164300, 0 });
	BOOST_TEST( xcpu.Z == false);
}

BOOST_AUTO_TEST_CASE(width_wrap)
{
	xcpu.D[1] = 36;
	xcpu.D[0] = 0x12045678;
	test_cpu({ 0164300, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(wrap)
{
	xcpu.D[1] = 4;
	xcpu.D[0] = 0x02345670;
	test_cpu({ 0164300, (uint16_t)( 28 << 6 | 8) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(negative_offset)
{
	xcpu.D[1] = -4;
	xcpu.D[0] = 0x02345670;
	test_cpu({ 0164300, (uint16_t)(1 << 11 | 1 << 6 | 8) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(8),
					 dst
	)
{
	xcpu.D[dst] = 0x12045678;
	test_cpu({ (uint16_t)(0164300 | dst), (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(bftst_mem, fixture);
BOOST_AUTO_TEST_CASE(in1)
{
	test_write8(0x1000, 0x10 );	
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(4 << 6 | 4) });
	BOOST_TEST( xcpu.Z == true );
}
BOOST_AUTO_TEST_CASE(in2)
{
	test_write16(0x1000, 0x1004 );	
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(4 << 6 | 8) });
	BOOST_TEST( xcpu.Z == true );
}
BOOST_AUTO_TEST_CASE(in3)
{
	test_write32(0x1000, 0x10000678 );	
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(4 << 6 | 16) });
	BOOST_TEST( xcpu.Z == true );
}
BOOST_AUTO_TEST_CASE(in4)
{
	test_write32(0x1000, 0x10000008 );	
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(4 << 6 | 24) });
	BOOST_TEST( xcpu.Z == true );
}
BOOST_AUTO_TEST_CASE(in5)
{
	test_write32(0x1000, 0x10000000 );	
	test_write8(0x1004, 0x0a );	
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(4 << 6 ) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write16(0x1000, 0x1204 | (( n ? 0xa : 2 ) << 4) );
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	test_write16(0x1000, 0x1204 | (( z ? 0: 2 ) << 4) );
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( xcpu.Z == z );
}
BOOST_AUTO_TEST_CASE(offset_reg)
{
	xcpu.D[1] = 8;
	test_write16(0x1000, 0x1204 );
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(1 << 11 | 1 << 6 | 4) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(width_reg)
{
	xcpu.D[1] = 4;
	test_write16(0x1000, 0x1204 );
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(width_wrap32)
{
	xcpu.D[1] = 36;
	test_write16(0x1000, 0x1204 );
	xcpu.A[0] = 0x1000;
	test_cpu({ 0164320, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(negative_offset)
{
	xcpu.D[1] = -4;
	test_write16(0x1000, 0x1004 );
	xcpu.A[0] = 0x1001;
	test_cpu({ 0164320, (uint16_t)(1 << 11 | 1 << 6 | 8) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1000, 0x10 );	
	xcpu.A[dst] = 0x1000;
	test_cpu({ (uint16_t)(0164320 | dst), (uint16_t)(4 << 6 | 4) });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1010, 0x10 );	
	xcpu.A[dst] = 0x1000;
	test_cpu({ (uint16_t)(0164350 | dst), (uint16_t)(4 << 6 | 4), 0x10 });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1040, 0x10 );	
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu({ (uint16_t)(0164360 | dst), (uint16_t)(4 << 6 | 4), 0x0830 });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(xxxw) {
	test_write8(0x1000, 0x10 );	
	test_cpu({ 0164370, (uint16_t)(4 << 6 | 4), 0x1000 });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(xxxl) {
	test_write8(0x1000, 0x10 );	
	test_cpu({ 0164371, (uint16_t)(4 << 6 | 4), 0, 0x1000 });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(d16pc)
{
	test_write8(0x14, 0x10 );	
	test_cpu({ 0164372, (uint16_t)(4 << 6 | 4), 0x10 });
	BOOST_TEST( xcpu.Z == true );
}

BOOST_AUTO_TEST_CASE(d8pcxn)
{
	test_write8(0x44, 0x10 );	
	xcpu.D[0] = 0x10;
	test_cpu({ 0164373, (uint16_t)(4 << 6 | 4), 0x0830 });
	BOOST_TEST( xcpu.Z == true );
}



BOOST_AUTO_TEST_SUITE_END();

