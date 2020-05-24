#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(roxr_byte, fixture);
BOOST_DATA_TEST_CASE( result, bdata::xrange(2), old_x)  {
	cpu.V = true;
	cpu.X = old_x;
	cpu.R[0] = (0x20|0xffffff00);
	test_cpu( { 0161020 } );
	BOOST_TEST( cpu.R[0] == (0x10|(old_x<<7)|0xffffff00) );
	cpu.V = false;
}

BOOST_AUTO_TEST_CASE( imm8)  {
	cpu.R[0] = 1;
	test_cpu( { 0160020 } );
	BOOST_TEST( cpu.R[0] == 2 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = false;
	cpu.X = true;
	cpu.R[0] = 0x23;
	cpu.R[1] = 0;
	test_cpu( { 0161060 } );
	BOOST_TEST( cpu.R[0] == 0x23 );
	BOOST_TEST( cpu.C == true );
	BOOST_TEST( cpu.X == true );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 1 : 2;
	test_cpu( { 0161020 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.X = n;
	test_cpu( { 0161020 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 2;
	test_cpu( { 0161020 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[2] = 8;
	test_cpu( { 0163022 } );
	BOOST_TEST( cpu.R[2] == 1 );
}
BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[1] = 2;
	cpu.R[3] = 4;
	test_cpu( { 0161063 } );
	BOOST_TEST( cpu.R[3] == 1 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[1] = 66;
	cpu.R[3] = 4;
	test_cpu( { 0161063 } );
	BOOST_TEST( cpu.R[3] == 1 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(roxr_word, fixture);
BOOST_DATA_TEST_CASE( result, bdata::xrange(2), old_x)  {
	cpu.V = true;
	cpu.X = old_x;
	cpu.R[0] = (0x0400|0xffff0000);
	test_cpu( { 0161120 } );
	BOOST_TEST( cpu.R[0] == (0x0200|(old_x<<15)|0xffff0000) );
	BOOST_TEST( cpu.V == false );
}

BOOST_AUTO_TEST_CASE( imm8)  {
	cpu.R[0] = 0x100;
	test_cpu( { 0160120 } );
	BOOST_TEST( cpu.R[0] == 1 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.X = true;
	cpu.C = false;
	cpu.R[0] = 0x2300;
	cpu.R[1] = 0;
	test_cpu( { 0161160 } );
	BOOST_TEST( cpu.R[0] == 0x2300 );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.C == true );
}

BOOST_DATA_TEST_CASE(c,
					 bdata::xrange(2),
					 c
	)
{
	cpu.R[0] = c ? 1 : 2;
	test_cpu( { 0161120 } );
	BOOST_TEST( cpu.C == c );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.X = n;
	test_cpu( { 0161120 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 2;
	test_cpu( { 0161120 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x400;
	test_cpu( { 0162123 } );
	BOOST_TEST( cpu.R[3] == 0x100 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x400;
	test_cpu( { 0164163 } );
	BOOST_TEST( cpu.R[3] == 0x100 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x400;
	test_cpu( { 0164163 } );
	BOOST_TEST( cpu.R[3] == 0x100 );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_write2, ea ) {
	test_ea_w(cpu, { 0162300 }, 0x200, ea, 0x100 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(roxr_long, fixture);
BOOST_DATA_TEST_CASE( result, bdata::xrange(2), old_x)  {
	cpu.V = true;
	cpu.X = old_x;
	cpu.R[0] = 0x00040000;
	test_cpu( { 0161220 } );
	BOOST_TEST( cpu.R[0] == 0x00020000 | (old_x << 31));
	BOOST_TEST( cpu.V == false );
}

BOOST_AUTO_TEST_CASE( imm8 )  {
	cpu.R[0] = 0x10000;
	test_cpu( { 0160220 } );
	BOOST_TEST( cpu.R[0] == 0x100 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.X = true;
	cpu.C = false;
	cpu.R[0] = 0x230000;
	cpu.R[1] = 0;
	test_cpu( { 0161260 } );
	BOOST_TEST( cpu.R[0] == 0x230000 );
	BOOST_TEST( cpu.C == true );
	BOOST_TEST( cpu.X == true );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 1 : 2;
	test_cpu( { 0161220 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.C == cx );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.X = n;
	test_cpu( { 0161220 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 2;
	test_cpu( { 0161220 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x40000;
	test_cpu( { 0162223 } );
	BOOST_TEST( cpu.R[3] == (0x10000) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[1] = 2;
	cpu.R[3] = 0x10000;
	test_cpu( { 0161263 } );
	BOOST_TEST( cpu.R[3] == (0x10000 >> 2) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[1] = 66;
	cpu.R[3] = 0x10000;
	test_cpu( { 0161263 } );
	BOOST_TEST( cpu.R[3] == (0x10000 >> 2) );
}
BOOST_AUTO_TEST_SUITE_END();
