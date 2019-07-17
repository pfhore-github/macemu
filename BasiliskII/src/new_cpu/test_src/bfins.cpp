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
BOOST_FIXTURE_TEST_SUITE(bfins_reg, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	xcpu.X = xcpu.N = xcpu.Z = xcpu.V = xcpu.C = true;
	xcpu.D[0] = 0x12f45678;
	xcpu.D[1] = 3;
	test_cpu({ 0167700, (uint16_t)(1 << 12 | 8 << 6 | 4) });
	BOOST_TEST( xcpu.D[0] == 0x12345678 );
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
	xcpu.D[0] = 0x12345678;
	xcpu.D[1] = ( n ? 0xa : 2 );
	test_cpu({ 0167700, (uint16_t)(1 << 12 | 8 << 6 | 4) });
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = 0x12345678;
	xcpu.D[1] = ( z ? 0 : 2 );
	test_cpu({ 0167700, (uint16_t)(1 << 12 | 8 << 6 | 4) });
	BOOST_TEST( xcpu.Z == z );
}
BOOST_AUTO_TEST_CASE(offset_reg)
{
	xcpu.D[1] = 8;
	xcpu.D[0] = 0x12f45678;
	xcpu.D[2] = 3;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 1 << 11 | 1 << 6 | 4) });
	BOOST_TEST( xcpu.D[0] == 0x12345678 );
}

BOOST_AUTO_TEST_CASE(width_reg)
{
	xcpu.D[1] = 4;
	xcpu.D[0] = 0x12f45678;
	xcpu.D[2] = 3;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( xcpu.D[0] == 0x12345678 );
}

BOOST_AUTO_TEST_CASE(width32)
{
	xcpu.D[1] = 4;
	xcpu.D[0] = 0x12345678;
	xcpu.D[2] = 0xabcdef12;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 8 << 6) });
	BOOST_TEST( xcpu.D[0] == 0x12abcdef );
}

BOOST_AUTO_TEST_CASE(width_wrap)
{
	xcpu.D[1] = 36;
	xcpu.D[0] = 0x12f45678;
	xcpu.D[2] = 3;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( xcpu.D[0] == 0x12345678 );
}

BOOST_AUTO_TEST_CASE(wrap)
{
	xcpu.D[1] = 4;
	xcpu.D[0] = 0x12345673;
	xcpu.D[2] = 0xab;
	test_cpu({ 0167700, (uint16_t)( 2 << 12 | 28 << 6 | 8) });
	BOOST_TEST( xcpu.D[0] == 0xb234567a );
}

BOOST_AUTO_TEST_CASE(negative_offset)
{
	xcpu.D[1] = -4;
	xcpu.D[0] = 0xf234567f;
	xcpu.D[2] = 0xab;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 1 << 11 | 1 << 6 | 8) });
	BOOST_TEST( xcpu.D[0] == 0xb234567a );
}

BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(1,8),
					 dst
	)
{
	xcpu.D[dst] = 0x12f45678;
	xcpu.D[0] = 3;
	test_cpu({ (uint16_t)(0167700 | dst), (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( xcpu.D[dst] == 0x12345678 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(bfins_mem, fixture);
BOOST_AUTO_TEST_CASE(in1)
{
	test_write8(0x1000, 0x12 );	
	xcpu.A[0] = 0x1000;
	xcpu.D[0] = 3;
	test_cpu({ 0167720, (uint16_t)(4 << 6 | 4) });
	BOOST_TEST( test_read8(0x1000) == 0x13 );
}
BOOST_AUTO_TEST_CASE(in2)
{
	test_write16(0x1000, 0x1ff4 );	
	xcpu.A[0] = 0x1000;
	xcpu.D[0] = 0x56;
	test_cpu({ 0167720, (uint16_t)(4 << 6 | 8) });
	BOOST_TEST( test_read16(0x1000) == 0x1564 );
}
BOOST_AUTO_TEST_CASE(in3)
{
	test_write32(0x1000, 0x1ffff345 );	
	xcpu.A[0] = 0x1000;
	xcpu.D[0] = 0x5678;
	test_cpu({ 0167720, (uint16_t)(4 << 6 | 16) });
	BOOST_TEST( test_read32(0x1000) == 0x15678345 );
}
BOOST_AUTO_TEST_CASE(in4)
{
	test_write32(0x1000, 0x1ffffff3 );	
	xcpu.A[0] = 0x1000;
	xcpu.D[0] = 0x56789A;
	test_cpu({ 0167720, (uint16_t)(4 << 6 | 24) });
	BOOST_TEST( test_read32(0x1000) == 0x156789A3 );
}
BOOST_AUTO_TEST_CASE(in5)
{
	test_write32(0x1000, 0x1fffffff );	
	test_write8(0x1004, 0xf3 );	
	xcpu.A[0] = 0x1000;
	xcpu.D[0] = 0x56789ABC;
	test_cpu({ 0167720, (uint16_t)(4 << 6 ) });
	BOOST_TEST( test_read32(0x1000) == 0x156789AB );
	BOOST_TEST( test_read8(0x1004) == 0xC3 );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write16(0x1000, 0x1234);
	xcpu.D[0] = ( n ? -2 : 2 );
	xcpu.A[0] = 0x1000;
	test_cpu({ 0167720, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	test_write16(0x1000, 0x1234);
	xcpu.D[0] = ( z ? 0 : 2 );
	xcpu.A[0] = 0x1000;
	test_cpu({ 0167720, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( xcpu.Z == z );
}
BOOST_AUTO_TEST_CASE(offset_reg)
{
	xcpu.D[1] = 8;
	test_write16(0x1000, 0x12f4 );
	xcpu.D[0] = 3;
	xcpu.A[0] = 0x1000;
	test_cpu({ 0167720, (uint16_t)(1 << 11 | 1 << 6 | 4) });
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
}

BOOST_AUTO_TEST_CASE(width_reg)
{
	xcpu.D[1] = 4;
	test_write16(0x1000, 0x12f4 );
	xcpu.A[0] = 0x1000;
	xcpu.D[0] = 3;
	test_cpu({ 0167720, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
}



BOOST_AUTO_TEST_CASE(width_wrap32)
{
	xcpu.D[1] = 36;
	test_write16(0x1000, 0x1234 );
	xcpu.A[0] = 0x1000;
	xcpu.D[0] = 3;
	test_cpu({ 0167720, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
}

BOOST_AUTO_TEST_CASE(negative_offset)
{
	xcpu.D[1] = -4;
	test_write16(0x1000, 0x1ff4 );
	xcpu.A[0] = 0x1001;
	xcpu.D[0] = 0x56;
	test_cpu({ 0167720, (uint16_t)(1 << 11 | 1 << 6 | 8) });
	BOOST_TEST( test_read16(0x1000) == 0x1564 );
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1000, 0x1f );	
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 3;
	test_cpu({ (uint16_t)(0167720 | dst), (uint16_t)(4 << 6 | 4) });
	BOOST_TEST( test_read8(0x1000) == 0x13 );
}
BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1010, 0x1f );	
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 3;
	test_cpu({ (uint16_t)(0167750 | dst), (uint16_t)(4 << 6 | 4), 0x10 });
	BOOST_TEST( test_read8(0x1010) == 0x13 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst
	)
{
	test_write8(0x1040, 0x1f );	
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[1] = 3;
	test_cpu({ (uint16_t)(0167760 | dst), (uint16_t)(1 << 12 | 4 << 6 | 4), 0x0830 });
	BOOST_TEST( test_read8(0x1040) == 0x13 );

}

BOOST_AUTO_TEST_CASE(xxxw) {
	test_write8(0x1000, 0x1f );
	xcpu.D[0] = 3;
	test_cpu({ 0167770, (uint16_t)(4 << 6 | 4), 0x1000 });
	BOOST_TEST( test_read8(0x1000) == 0x13 );
}

BOOST_AUTO_TEST_CASE(xxxl) {
	test_write8(0x1000, 0x1f );	
	xcpu.D[0] = 3;
	test_cpu({ 0167771, (uint16_t)(4 << 6 | 4), 0, 0x1000 });
	BOOST_TEST( test_read8(0x1000) == 0x13 );
}


BOOST_AUTO_TEST_SUITE_END();

