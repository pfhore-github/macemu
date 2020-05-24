#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(cas_byte, fixture);
BOOST_AUTO_TEST_CASE( result_z)  {
	cpu.R[8+0] = 0x1000;
	test_write8(0x1000, 23 );
	cpu.R[0] = 23;
	cpu.R[1] = 34;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read8(0x1000) ==  34 );
	BOOST_TEST( cpu.R[0] == 23 );
	BOOST_TEST( cpu.Z == true );
}
BOOST_AUTO_TEST_CASE( result_nz)  {
	cpu.R[8+0] = 0x1000;
	test_write8(0x1000, 23 );
	cpu.R[0] = 22;
	cpu.R[1] = 34;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read8(0x1000) == 23 );
	BOOST_TEST( cpu.R[0] == 23 );
	BOOST_TEST( cpu.Z == false );
}
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[8+0] = 0x1000;
	test_write8(0x1000, 0 );
	cpu.R[0] = c;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[8+0] = 0x1000;
	test_write8(0x1000, 0x80 );
	cpu.R[0] = v;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[8+0] = 0x1000;
	test_write8(0x1000, 0x7f );
	cpu.R[0] = 0x7f + v;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[8+0] = 0x1000;
	test_write8(0x1000, 0 );
	cpu.R[0] = n ? 1 : 0;
	test_cpu( { 0005320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	cpu.R[0] = 23;
	cpu.R[1] = 34;
	test_ea_b(cpu, { 0005300, (uint16_t)(1 << 6) }, 23, ea, 34 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cas_word, fixture);
BOOST_AUTO_TEST_CASE( result_z)  {
	cpu.R[8+0] = 0x1000;
	test_write16(0x1000, 2323 );
	cpu.R[0] = 2323;
	cpu.R[1] = 3434;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read16(0x1000) ==  3434 );
	BOOST_TEST( cpu.R[0] == 2323 );
	BOOST_TEST( cpu.Z == true );
}
BOOST_AUTO_TEST_CASE( result_nz)  {
	cpu.R[8+0] = 0x1000;
	test_write16(0x1000, 2323 );
	cpu.R[0] = 2222;
	cpu.R[1] = 3434;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read16(0x1000) == 2323 );
	BOOST_TEST( cpu.R[0] == 2323 );
	BOOST_TEST( cpu.Z == false );
}
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[8+0] = 0x1000;
	test_write16(0x1000, 0 );
	cpu.R[0] = c;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[8+0] = 0x1000;
	test_write16(0x1000, 0x8000 );
	cpu.R[0] = v;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[8+0] = 0x1000;
	test_write16(0x1000, 0x7fff );
	cpu.R[0] = 0x7fff + v;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[8+0] = 0x1000;
	test_write16(0x1000, 0 );
	cpu.R[0] = n ? 1 : 0;
	test_cpu( { 0006320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	cpu.R[0] = 2323;
	cpu.R[1] = 3434;
	test_ea_w(cpu, { 0006300, (uint16_t)(1 << 6) }, 2323, ea, 3434 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(cas_long, fixture);

BOOST_AUTO_TEST_CASE( result_z)  {
	cpu.R[8+0] = 0x1000;
	test_write32(0x1000, 232323 );
	cpu.R[0] = 232323;
	cpu.R[1] = 343434;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read32(0x1000) ==  343434 );
	BOOST_TEST( cpu.R[0] == 232323 );
	BOOST_TEST( cpu.Z == true );
}
BOOST_AUTO_TEST_CASE( result_nz)  {
	cpu.R[8+0] = 0x1000;
	test_write32(0x1000, 232323 );
	cpu.R[0] = 222222;
	cpu.R[1] = 343434;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( test_read32(0x1000) == 232323 );
	BOOST_TEST( cpu.R[0] == 232323 );
	BOOST_TEST( cpu.Z == false );
}
BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[8+0] = 0x1000;
	test_write32(0x1000, 0 );
	cpu.R[0] = c;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.C == c );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[8+0] = 0x1000;
	test_write32(0x1000, 0x80000000 );
	cpu.R[0] = v;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[8+0] = 0x1000;
	test_write32(0x1000, 0x7fffffff );
	cpu.R[0] = 0x7fffffffU + v;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[8+0] = 0x1000;
	test_write32(0x1000, 0 );
	cpu.R[0] = n ? 1 : 0;
	test_cpu( { 0007320, (uint16_t)(1 << 6) } );
	BOOST_TEST( cpu.N == n );
}
BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	cpu.R[0] = 232323;
	cpu.R[1] = 343434;
	test_ea_l(cpu, { 0007300, (uint16_t)(1 << 6) }, 232323, ea, 343434 );
}


BOOST_AUTO_TEST_SUITE_END();
