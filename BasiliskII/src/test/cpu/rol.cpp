#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(rol_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = true;
	cpu.R[0] = (0x82|0xffffff00);
	test_cpu( { 0161430 } );
	BOOST_TEST( cpu.R[0] == (0x5|0xffffff00) );
	cpu.V = false;
}

BOOST_AUTO_TEST_CASE( imm8)  {
	cpu.R[0] = 1;
	test_cpu( { 0160430 } );
	BOOST_TEST( cpu.R[0] == 1 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = true;
	cpu.R[0] = 0x23;
	cpu.R[1] = 0;
	test_cpu( { 0161470 } );
	BOOST_TEST( cpu.R[0] == 0x23 );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = c ? 0x80 : 0x40;
	test_cpu( { 0161430 } );
	BOOST_TEST( cpu.C == c );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xff : 0;
	test_cpu( { 0161430 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 1;
	test_cpu( { 0161430 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[2] = 1;
	test_cpu( { 0163432 } );
	BOOST_TEST( cpu.R[2] == 8 );
}
BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[1] = 2;
	cpu.R[3] = 1;
	test_cpu( { 0161473 } );
	BOOST_TEST( cpu.R[3] == 4 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[1] = 66;
	cpu.R[3] = 1;
	test_cpu( { 0161473 } );
	BOOST_TEST( cpu.R[3] == 4 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(rol_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = true;
	cpu.R[0] = (0x8300|0xffff0000);
	test_cpu( { 0161530 } );
	BOOST_TEST( cpu.R[0] == (0x0601|0xffff0000) );
	BOOST_TEST( cpu.V == false );
}

BOOST_AUTO_TEST_CASE( imm8)  {
	cpu.R[0] = 1;
	test_cpu( { 0160530 } );
	BOOST_TEST( cpu.R[0] == 0x100 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = true;
	cpu.R[0] = 0x2300;
	cpu.R[1] = 0;
	test_cpu( { 0161570 } );
	BOOST_TEST( cpu.R[0] == 0x2300 );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = c ? 0x8000 : 0x4000;
	test_cpu( { 0161530 } );
	BOOST_TEST( cpu.C == c );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xffff : 0;
	test_cpu( { 0161530 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 1;
	test_cpu( { 0161530 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x100;
	test_cpu( { 0162533 } );
	BOOST_TEST( cpu.R[3] == 0x400 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x100;
	test_cpu( { 0164573 } );
	BOOST_TEST( cpu.R[3] == 0x400 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x100;
	test_cpu( { 0164573 } );
	BOOST_TEST( cpu.R[3] == 0x400 );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_write2, ea ) {
	test_ea_w(cpu, { 0163700 }, 0x100, ea, 0x200 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(rol_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.V = true;
	cpu.R[0] = 0x80030000;
	test_cpu( { 0161630 } );
	BOOST_TEST( cpu.R[0] == 0x00060001 );
	BOOST_TEST( cpu.V == false );
}

BOOST_AUTO_TEST_CASE( imm8)  {
	cpu.R[0] = 0x100;
	test_cpu( { 0160630 } );
	BOOST_TEST( cpu.R[0] == 0x10000 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = true;
	cpu.R[0] = 0x230000;
	cpu.R[1] = 0;
	test_cpu( { 0161670 } );
	BOOST_TEST( cpu.R[0] == 0x230000 );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = c ? 0x80000000 : 0x40000000;
	test_cpu( { 0161630 } );
	BOOST_TEST( cpu.C == c );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xffffffff : 0;
	test_cpu( { 0161630 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 1;
	test_cpu( { 0161630 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x10000;
	test_cpu( { 0162633 } );
	BOOST_TEST( cpu.R[3] == (0x40000) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[1] = 2;
	cpu.R[3] = 0x10000;
	test_cpu( { 0161673 } );
	BOOST_TEST( cpu.R[3] == (0x10000 << 2) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[1] = 66;
	cpu.R[3] = 0x10000;
	test_cpu( { 0161673 } );
	BOOST_TEST( cpu.R[3] == (0x10000 << 2) );
}
BOOST_AUTO_TEST_SUITE_END();
