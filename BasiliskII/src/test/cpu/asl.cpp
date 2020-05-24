#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;
BOOST_FIXTURE_TEST_SUITE(asl_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[0] = 0x23 | 0xffffff00;
	test_cpu( { 0161400 } );
	BOOST_TEST( cpu.R[0] == (0x46|0xffffff00) );
}

BOOST_AUTO_TEST_CASE( imm8)  {
	cpu.R[0] = 1;
	test_cpu( { 0160400 } );
	BOOST_TEST( cpu.R[0] == 0 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = cpu.X = true;
	cpu.R[0] = 0x23;
	cpu.R[1] = 0;
	test_cpu( { 0161440 } );
	BOOST_TEST( cpu.R[0] == 0x23 );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 0x80 : 0x40;
	test_cpu( { 0161400 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0x40 : 0x20;
	test_cpu( { 0161400 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0x80 : 0xff;
	test_cpu( { 0161400 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_3,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0x40 : 0x10;
	test_cpu( { 0162400 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_4,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0xa0 : 0xff;
	test_cpu( { 0161400 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xff : 0;
	test_cpu( { 0161400 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 1;
	test_cpu( { 0161400 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[2] = 1;
	test_cpu( { 0163402 } );
	BOOST_TEST( cpu.R[2] == 8 );
}
BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[1] = 2;
	cpu.R[3] = 1;
	test_cpu( { 0161443 } );
	BOOST_TEST( cpu.R[3] == 4 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[1] = 66;
	cpu.R[3] = 1;
	test_cpu( { 0161443 } );
	BOOST_TEST( cpu.R[3] == 4 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(asl_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[0] = (0x2300|0xffff0000);
	test_cpu( { 0161500 } );
	BOOST_TEST( cpu.R[0] == (0x4600|0xffff0000) );
}

BOOST_AUTO_TEST_CASE( immby0)  {
	cpu.R[0] = 1;
	test_cpu( { 0160500 } );
	BOOST_TEST( cpu.R[0] == 0x100 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = cpu.X = true;
	cpu.R[0] = 0x2300;
	cpu.R[1] = 0;
	test_cpu( { 0161540 } );
	BOOST_TEST( cpu.R[0] == 0x2300 );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 0x8000 : 0x4000;
	test_cpu( { 0161500 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0x4000 : 0x2000;
	test_cpu( { 0161500 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0x8000 : 0xffff;
	test_cpu( { 0161500 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_3,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0x4000 : 0x1000;
	test_cpu( { 0162500 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_4,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0xa000 : 0xffff;
	test_cpu( { 0161500 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xffff : 0;
	test_cpu( { 0161500 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 1;
	test_cpu( { 0161500 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x100;
	test_cpu( { 0162503 } );
	BOOST_TEST( cpu.R[3] == 0x400 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x100;
	test_cpu( { 0164543 } );
	BOOST_TEST( cpu.R[3] == 0x400 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x100;
	test_cpu( { 0164543 } );
	BOOST_TEST( cpu.R[3] == 0x400 );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	test_ea_w(cpu, { 0160700 }, 0x100, ea, 0x200);
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(asl_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	cpu.R[0] = 0x230000;
	test_cpu( { 0161600 } );
	BOOST_TEST( cpu.R[0] == 0x460000 );
}

BOOST_AUTO_TEST_CASE( immby0)  {
	cpu.R[0] = 0x100;
	test_cpu( { 0160600 } );
	BOOST_TEST( cpu.R[0] == 0x10000 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = cpu.X = true;
	cpu.R[0] = 0x230000;
	cpu.R[1] = 0;
	test_cpu( { 0161640 } );
	BOOST_TEST( cpu.R[0] == 0x230000 );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 0x80000000 : 0x40000000;
	test_cpu( { 0161600 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0x40000000 : 0x20000000;
	test_cpu( { 0161600 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0x80000000 : 0xffffffff;
	test_cpu( { 0161600 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_3,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0x40000000 : 0x10000000;
	test_cpu( { 0162600 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(v_4,
					 bdata::xrange(2),
					 v
	)
{
	cpu.R[0] = v ? 0xa0000000 : 0xffffffff;
	test_cpu( { 0161600 } );
	BOOST_TEST( cpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xffffffff : 0;
	test_cpu( { 0161600 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 0 : 1;
	test_cpu( { 0161600 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x10000;
	test_cpu( { 0162603 } );
	BOOST_TEST( cpu.R[3] == (0x40000) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[1] = 2;
	cpu.R[3] = 0x10000;
	test_cpu( { 0161643 } );
	BOOST_TEST( cpu.R[3] == (0x10000 << 2) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[1] = 66;
	cpu.R[3] = 0x10000;
	test_cpu( { 0161643 } );
	BOOST_TEST( cpu.R[3] == (0x10000 << 2) );
}

BOOST_AUTO_TEST_SUITE_END();
