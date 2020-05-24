#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(bfins_reg, fixture);
BOOST_AUTO_TEST_CASE(result)
{
	cpu.N = cpu.Z = cpu.V = cpu.C = true;
	cpu.R[0] = 0x12f45678;
	cpu.R[1] = 3;
	test_cpu({ 0167700, (uint16_t)(1 << 12 | 8 << 6 | 4) });
	BOOST_TEST( cpu.R[0] == 0x12345678 );
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
	cpu.R[0] = 0x12345678;
	cpu.R[1] = ( n ? 0xa : 2 );
	test_cpu({ 0167700, (uint16_t)(1 << 12 | 8 << 6 | 4) });
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = 0x12345678;
	cpu.R[1] = ( z ? 0 : 2 );
	test_cpu({ 0167700, (uint16_t)(1 << 12 | 8 << 6 | 4) });
	BOOST_TEST( cpu.Z == z );
}
BOOST_AUTO_TEST_CASE(offset_reg)
{
	cpu.R[1] = 8;
	cpu.R[0] = 0x12f45678;
	cpu.R[2] = 3;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 1 << 11 | 1 << 6 | 4) });
	BOOST_TEST( cpu.R[0] == 0x12345678 );
}

BOOST_AUTO_TEST_CASE(width_reg)
{
	cpu.R[1] = 4;
	cpu.R[0] = 0x12f45678;
	cpu.R[2] = 3;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( cpu.R[0] == 0x12345678 );
}

BOOST_AUTO_TEST_CASE(width32)
{
	cpu.R[1] = 4;
	cpu.R[0] = 0x12345678;
	cpu.R[2] = 0xabcdef12;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 8 << 6) });
	BOOST_TEST( cpu.R[0] == 0x12abcdef );
}

BOOST_AUTO_TEST_CASE(width_wrap)
{
	cpu.R[1] = 36;
	cpu.R[0] = 0x12f45678;
	cpu.R[2] = 3;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( cpu.R[0] == 0x12345678 );
}

BOOST_AUTO_TEST_CASE(wrap)
{
	cpu.R[1] = 4;
	cpu.R[0] = 0x12345673;
	cpu.R[2] = 0xab;
	test_cpu({ 0167700, (uint16_t)( 2 << 12 | 28 << 6 | 8) });
	BOOST_TEST( cpu.R[0] == 0xb234567a );
}

BOOST_AUTO_TEST_CASE(negative_offset)
{
	cpu.R[1] = -4;
	cpu.R[0] = 0xf234567f;
	cpu.R[2] = 0xab;
	test_cpu({ 0167700, (uint16_t)(2 << 12 | 1 << 11 | 1 << 6 | 8) });
	BOOST_TEST( cpu.R[0] == 0xb234567a );
}

BOOST_AUTO_TEST_CASE(dn)
{
	cpu.R[5] = 0x12f45678;
	cpu.R[0] = 3;
	test_cpu({ 0167705, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( cpu.R[5] == 0x12345678 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(bfins_mem, fixture);
BOOST_AUTO_TEST_CASE(in1)
{
	test_write8(0x1000, 0x12 );	
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 3;
	test_cpu({ 0167720, (uint16_t)(4 << 6 | 4) });
	BOOST_TEST( test_read8(0x1000) == 0x13 );
}
BOOST_AUTO_TEST_CASE(in2)
{
	test_write16(0x1000, 0x1ff4 );	
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 0x56;
	test_cpu({ 0167720, (uint16_t)(4 << 6 | 8) });
	BOOST_TEST( test_read16(0x1000) == 0x1564 );
}
BOOST_AUTO_TEST_CASE(in3)
{
	test_write32(0x1000, 0x1ffff345 );	
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 0x5678;
	test_cpu({ 0167720, (uint16_t)(4 << 6 | 16) });
	BOOST_TEST( test_read32(0x1000) == 0x15678345 );
}
BOOST_AUTO_TEST_CASE(in4)
{
	test_write32(0x1000, 0x1ffffff3 );	
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 0x56789A;
	test_cpu({ 0167720, (uint16_t)(4 << 6 | 24) });
	BOOST_TEST( test_read32(0x1000) == 0x156789A3 );
}
BOOST_AUTO_TEST_CASE(in5)
{
	test_write32(0x1000, 0x1fffffff );	
	test_write8(0x1004, 0xf3 );	
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 0x56789ABC;
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
	cpu.R[0] = ( n ? -2 : 2 );
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167720, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	test_write16(0x1000, 0x1234);
	cpu.R[0] = ( z ? 0 : 2 );
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167720, (uint16_t)(8 << 6 | 4) });
	BOOST_TEST( cpu.Z == z );
}
BOOST_AUTO_TEST_CASE(offset_reg)
{
	cpu.R[1] = 8;
	test_write16(0x1000, 0x12f4 );
	cpu.R[0] = 3;
	cpu.R[8+0] = 0x1000;
	test_cpu({ 0167720, (uint16_t)(1 << 11 | 1 << 6 | 4) });
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
}

BOOST_AUTO_TEST_CASE(width_reg)
{
	cpu.R[1] = 4;
	test_write16(0x1000, 0x12f4 );
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 3;
	test_cpu({ 0167720, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
}



BOOST_AUTO_TEST_CASE(width_wrap32)
{
	cpu.R[1] = 36;
	test_write16(0x1000, 0x1234 );
	cpu.R[8+0] = 0x1000;
	cpu.R[0] = 3;
	test_cpu({ 0167720, (uint16_t)(1 << 5 | 8 << 6 | 1) });
	BOOST_TEST( test_read16(0x1000) == 0x1234 );
}

BOOST_AUTO_TEST_CASE(negative_offset)
{
	cpu.R[1] = -4;
	test_write16(0x1000, 0x1ff4 );
	cpu.R[8+0] = 0x1001;
	cpu.R[0] = 0x56;
	test_cpu({ 0167720, (uint16_t)(1 << 11 | 1 << 6 | 8) });
	BOOST_TEST( test_read16(0x1000) == 0x1564 );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea) {
	cpu.R[0] = 3;
	test_ea_b(cpu, { 0167700, (uint16_t)(4 << 6 | 4) } , 0x1f, ea, 0x13 );
}


BOOST_AUTO_TEST_SUITE_END();

