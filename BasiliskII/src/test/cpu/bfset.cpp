#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(bfset_reg, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	cpu.N = cpu.Z = cpu.V = cpu.C = true;
	cpu.R[0] = 0x12345678;
	test_cpu({ 0167300, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( cpu.R[0] == 0x12f45678 );
	BOOST_TEST( cpu.N == false );
	BOOST_TEST( cpu.Z == false );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.C == false );
}
BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = 0x12045678 | (( n ? 0xa : 2 ) << 20);
	test_cpu({ 0167300, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 0x12045678 | (( z ? 0 : 2 ) << 20);
	test_cpu({ 0167300, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(offset_reg)
{
	cpu.R[1] = 8;
	cpu.R[0] = 0x12345678;
	test_cpu({ 0167300, (uint16_t)(1 << 11 | 1 << 6 | 4) });
	BOOST_TEST( cpu.R[0] == 0x12f45678 );
}

BOOST_AUTO_TEST_CASE(width_reg)
{
	cpu.R[1] = 4;
	cpu.R[0] = 0x12345678;
	test_cpu({ 0167300, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( cpu.R[0] == 0x12f45678 );
}

BOOST_AUTO_TEST_CASE(width32)
{
	cpu.R[1] = 4;
	cpu.R[0] = 0x12345678;
	test_cpu({ 0167300, 0 });
	BOOST_TEST( cpu.R[0] == 0xffffffff );
}

BOOST_AUTO_TEST_CASE(width_wrap)
{
	cpu.R[1] = 36;
	cpu.R[0] = 0x12345678;
	test_cpu({ 0167300, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( cpu.R[0] == 0x12f45678 );
}

BOOST_AUTO_TEST_CASE(wrap)
{
	cpu.R[1] = 4;
	cpu.R[0] = 0x12345678;
	test_cpu({ 0167300, (uint16_t)( 28 << 6 | 8) });
	BOOST_TEST( cpu.R[0] == 0xf234567f );
}

BOOST_AUTO_TEST_CASE(negative_offset)
{
	cpu.R[1] = -4;
	cpu.R[0] = 0x12345678;
	test_cpu({ 0167300, (uint16_t)(1 << 11 | 1 << 6 | 8) });
	BOOST_TEST( cpu.R[0] == 0xf234567f );
}

BOOST_AUTO_TEST_CASE(dn)
{
	cpu.R[4] = 0x12345678;
	test_cpu({ 0167304, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( cpu.R[4] == 0x12f45678 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(bfset_mem, fixture);
BOOST_AUTO_TEST_CASE(in1)
{
	test_write8(0x1000, 0x12 );	
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(4 << 6 | 4) });
	BOOST_TEST( test_read8(0x1000) == 0x1f );
}
BOOST_AUTO_TEST_CASE(in2)
{
	test_write16(0x1000, 0x1234 );	
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(4 << 6 | 8) });
	BOOST_TEST( test_read16(0x1000) == 0x1ff4 );
}
BOOST_AUTO_TEST_CASE(in3)
{
	test_write32(0x1000, 0x12345678 );	
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(4 << 6 | 16) });
	BOOST_TEST( test_read32(0x1000) == 0x1ffff678 );
}
BOOST_AUTO_TEST_CASE(in4)
{
	test_write32(0x1000, 0x12345678 );	
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(4 << 6 | 24) });
	BOOST_TEST( test_read32(0x1000) == 0x1ffffff8 );
}
BOOST_AUTO_TEST_CASE(in5)
{
	test_write32(0x1000, 0x12345678 );	
	test_write8(0x1004, 0x9a );	
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(4 << 6 ) });
	BOOST_TEST( test_read32(0x1000) == 0x1fffffff );
	BOOST_TEST( test_read8(0x1004) == 0xfa);
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	test_write16(0x1000, 0x1204 | (( n ? 0xa : 2 ) << 4) );
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	test_write16(0x1000, 0x1204 | (( z ? 0: 2 ) << 4) );
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( cpu.Z == z );
}
BOOST_AUTO_TEST_CASE(offset_reg)
{
	cpu.R[1] = 8;
	test_write16(0x1000, 0x1234 );
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(1 << 11 | 1 << 6 | 4) });
	BOOST_TEST( test_read16(0x1000) == 0x12f4 );
}

BOOST_AUTO_TEST_CASE(width_reg)
{
	cpu.R[1] = 4;
	test_write16(0x1000, 0x1234 );
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( test_read16(0x1000) == 0x12f4 );
}

BOOST_AUTO_TEST_CASE(width_wrap32)
{
	cpu.R[1] = 36;
	test_write16(0x1000, 0x1234 );
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167320, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( test_read16(0x1000) == 0x12f4 );
}

BOOST_AUTO_TEST_CASE(negative_offset)
{
	cpu.R[1] = -4;
	test_write16(0x1000, 0x1234 );
	cpu.R[8+0] = 0x1001;
	test_cpu({ 0167320, (uint16_t)(1 << 11 | 1 << 6 | 8) });
	BOOST_TEST( test_read16(0x1000) == 0x1ff4 );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea) {
	test_ea_b(cpu, { 0167300, (uint16_t)(4 << 6 | 4) } , 0x12, ea, 0x1f );
}

BOOST_AUTO_TEST_SUITE_END();

