#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

void test_cpu(const std::vector<uint16_t>& c);

BOOST_FIXTURE_TEST_SUITE(roxl_byte, fixture);
BOOST_DATA_TEST_CASE( result, bdata::xrange(2), old_x)  {
	cpu.V = true;
	cpu.X = old_x;
	cpu.R[0] = (0x2|0xffffff00);
	test_cpu( { 0161420 } );
	BOOST_TEST( cpu.R[0] == (0x4 | old_x |0xffffff00) );
	cpu.V = false;
}

BOOST_AUTO_TEST_CASE( imm8)  {
	cpu.R[0] = 1;
	test_cpu( { 0160420 } );
	BOOST_TEST( cpu.R[0] == 0 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = cpu.X = true;
	cpu.R[0] = 0x23;
	cpu.R[1] = 0;
	test_cpu( { 0161460 } );
	BOOST_TEST( cpu.R[0] == 0x23 );
	BOOST_TEST( cpu.C == false );
	BOOST_TEST( cpu.X == true );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 0x80 : 0x40;
	test_cpu( { 0161420 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xff : 0;
	test_cpu( { 0161420 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 1;
	test_cpu( { 0161420 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[2] = 1;
	test_cpu( { 0163422 } );
	BOOST_TEST( cpu.R[2] == 8 );
}
BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[1] = 2;
	cpu.R[3] = 1;
	test_cpu( { 0161463 } );
	BOOST_TEST( cpu.R[3] == 4 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[1] = 66;
	cpu.R[3] = 1;
	test_cpu( { 0161463 } );
	BOOST_TEST( cpu.R[3] == 4 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(roxl_word, fixture);
BOOST_DATA_TEST_CASE( result, bdata::xrange(2), old_x)  {
	cpu.V = true;
	cpu.X = old_x;
	cpu.R[0] = (0x0400|0xffff0000);
	test_cpu( { 0161520 } );
	BOOST_TEST( cpu.R[0] == (0x0800|old_x|0xffff0000) );
	BOOST_TEST( cpu.V == false );
}

BOOST_AUTO_TEST_CASE( imm8)  {
	cpu.R[0] = 1;
	test_cpu( { 0160520 } );
	BOOST_TEST( cpu.R[0] == 0x100 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = false;
	cpu.X = true;
	cpu.R[0] = 0x2300;
	cpu.R[1] = 0;
	test_cpu( { 0161560 } );
	BOOST_TEST( cpu.R[0] == 0x2300 );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.C == true );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 0x8000 : 0x4000;
	test_cpu( { 0161520 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xffff : 0;
	test_cpu( { 0161520 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 1;
	test_cpu( { 0161520 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x100;
	test_cpu( { 0162523 } );
	BOOST_TEST( cpu.R[3] == 0x400 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x100;
	test_cpu( { 0164563 } );
	BOOST_TEST( cpu.R[3] == 0x400 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x100;
	test_cpu( { 0164563 } );
	BOOST_TEST( cpu.R[3] == 0x400 );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_write2, ea ) {
	test_ea_w(cpu, { 0162700 }, 0x100, ea, 0x200 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(roxl_long, fixture);
BOOST_DATA_TEST_CASE( result, bdata::xrange(2), old_x)  {
	cpu.V = true;
	cpu.X = old_x;
	cpu.R[0] = 0x00030000;
	test_cpu( { 0161620 } );
	BOOST_TEST( cpu.R[0] == 0x00060000 | old_x );
	BOOST_TEST( cpu.V == false );
}

BOOST_AUTO_TEST_CASE( imm8)  {
	cpu.R[0] = 0x100;
	test_cpu( { 0160620 } );
	BOOST_TEST( cpu.R[0] == 0x10000 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = false;
	cpu.X = true;
	cpu.R[0] = 0x230000;
	cpu.R[1] = 0;
	test_cpu( { 0161660 } );
	BOOST_TEST( cpu.R[0] == 0x230000 );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.C == true );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 0x80000000 : 0x40000000;
	test_cpu( { 0161620 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xffffffff : 0;
	test_cpu( { 0161620 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 1;
	test_cpu( { 0161620 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x10000;
	test_cpu( { 0162623 } );
	BOOST_TEST( cpu.R[3] == (0x40000) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[1] = 2;
	cpu.R[3] = 0x10000;
	test_cpu( { 0161663 } );
	BOOST_TEST( cpu.R[3] == (0x10000 << 2) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[1] = 66;
	cpu.R[3] = 0x10000;
	test_cpu( { 0161663 } );
	BOOST_TEST( cpu.R[3] == (0x10000 << 2) );
}
BOOST_AUTO_TEST_SUITE_END();
