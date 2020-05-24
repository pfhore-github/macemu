#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(asr_byte, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[0] = 0x24 | 0xffffff00;
	cpu.V = true;
	test_cpu( { 0161000 } );
	BOOST_TEST( cpu.R[0] == ( 0x12 | 0xffffff00 ) );
	BOOST_TEST( cpu.V == false );
}
BOOST_AUTO_TEST_CASE( result_2)  {
	cpu.R[0] = 0xc0;
	test_cpu( { 0161000 } );
	BOOST_TEST( cpu.R[0] == 0xe0 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = cpu.X = true;
	cpu.R[0] = 0x40;
	cpu.R[1] = 0;
	test_cpu( { 0161040 } );
	BOOST_TEST( cpu.R[0] == 0x40 );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 0x21 : 0x20;
	test_cpu( { 0161000 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xff : 0;
	test_cpu( { 0161000 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 1 : 2;
	test_cpu( { 0161000 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x80;
	test_cpu( { 0161003 } );
	BOOST_TEST( cpu.R[3] == 0xc0 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x40;
	test_cpu( { 0164043 } );
	BOOST_TEST( cpu.R[3] == 0x10  );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x40;
	test_cpu( { 0164043 } );
	BOOST_TEST( cpu.R[3] == 0x10  );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(asr_word, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[0] = 0x2400 | 0xffff0000;
	cpu.V = true;
	test_cpu( { 0161100 } );
	BOOST_TEST( cpu.R[0] == ( 0x1200 | 0xffff0000 ) );
	BOOST_TEST( cpu.V == false );
}
BOOST_AUTO_TEST_CASE( result_2)  {
	cpu.R[0] = 0xc000;
	test_cpu( { 0161100 } );
	BOOST_TEST( cpu.R[0] == 0xe000 );
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
	cpu.R[0] = cx ? 0x2001 : 0x2000;
	test_cpu( { 0161100 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xffff : 0;
	test_cpu( { 0161100 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 1 : 2;
	test_cpu( { 0161100 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x100;
	test_cpu( { 0161103 } );
	BOOST_TEST( cpu.R[3] == 0x80 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x100;
	test_cpu( { 0164143 } );
	BOOST_TEST( cpu.R[3] == (0x100 >> 2) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x100;
	test_cpu( { 0164143 } );
	BOOST_TEST( cpu.R[3] == (0x100 >> 2) );
}

BOOST_DATA_TEST_CASE(ea_test, ea_write2, ea)
{
	test_ea_w(cpu, { 0160300 }, 0x100, ea, 0x80);
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(asr_long, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[0] = 0x24000000;
	cpu.V = true;
	test_cpu( { 0161200 } );
	BOOST_TEST( cpu.R[0] == 0x12000000 );
	BOOST_TEST( cpu.V == false );
}
BOOST_AUTO_TEST_CASE( result_2)  {
	cpu.R[0] = 0xc0000000;
	test_cpu( { 0161200 } );
	BOOST_TEST( cpu.R[0] == 0xe0000000 );
}


BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = cpu.X = true;
	cpu.R[0] = 0x230000;
	cpu.R[1] = 0;
	test_cpu( { 0161240 } );
	BOOST_TEST( cpu.R[0] == 0x230000 );
	BOOST_TEST( cpu.X == true );
	BOOST_TEST( cpu.C == false );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	cpu.R[0] = cx ? 0x20000001 : 0x20000000;
	test_cpu( { 0161200 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}


BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	cpu.R[0] = n ? 0xffffffff : 0;
	test_cpu( { 0161200 } );
	BOOST_TEST( cpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 1 : 2;
	test_cpu( { 0161200 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x10000;
	test_cpu( { 0161203 } );
	BOOST_TEST( cpu.R[3] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x10000;
	test_cpu( { 0164243 } );
	BOOST_TEST( cpu.R[3] == 0x4000  );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x10000;
	test_cpu( { 0164243 } );
	BOOST_TEST( cpu.R[3] == 0x4000  );
}
BOOST_AUTO_TEST_SUITE_END();
