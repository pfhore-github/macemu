#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace bdata = boost::unit_test::data;

BOOST_FIXTURE_TEST_SUITE(lsr_byte, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[0] = 0x24 | 0xffffff00;
	cpu.V = true;
	test_cpu( { 0161010 } );
	BOOST_TEST( cpu.R[0] == ( 0x12 | 0xffffff00 ) );
	BOOST_TEST( cpu.V == false );
}
BOOST_AUTO_TEST_CASE( result_2)  {
	cpu.R[0] = 0xc0;
	test_cpu( { 0161010 } );
	BOOST_TEST( cpu.R[0] == 0x60 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = cpu.X = true;
	cpu.R[0] = 0x40;
	cpu.R[1] = 0;
	test_cpu( { 0161050 } );
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
	test_cpu( { 0161010 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}


BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 1 : 2;
	test_cpu( { 0161010 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x80;
	test_cpu( { 0161013 } );
	BOOST_TEST( cpu.R[3] == 0x40 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x40;
	test_cpu( { 0164053 } );
	BOOST_TEST( cpu.R[3] == 0x10  );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x40;
	test_cpu( { 0164053 } );
	BOOST_TEST( cpu.R[3] == 0x10  );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(lsr_word, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[0] = 0x2400 | 0xffff0000;
	cpu.V = true;
	test_cpu( { 0161110 } );
	BOOST_TEST( cpu.R[0] == ( 0x1200 | 0xffff0000 ) );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.N == false );
}
BOOST_AUTO_TEST_CASE( result_2)  {
	cpu.R[0] = 0xc000;
	test_cpu( { 0161110 } );
	BOOST_TEST( cpu.R[0] == 0x6000 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = cpu.X = true;
	cpu.R[0] = 0x2300;
	cpu.R[1] = 0;
	test_cpu( { 0161550 } );
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
	test_cpu( { 0161110 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 1 : 2;
	test_cpu( { 0161110 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x100;
	test_cpu( { 0161113 } );
	BOOST_TEST( cpu.R[3] == 0x80 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x100;
	test_cpu( { 0164153 } );
	BOOST_TEST( cpu.R[3] == (0x100 >> 2) );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x100;
	test_cpu( { 0164153 } );
	BOOST_TEST( cpu.R[3] == (0x100 >> 2) );
}

BOOST_DATA_TEST_CASE(ea_exec, ea_write2, ea ) {
	test_ea_w(cpu, { 0161300 }, 0x100, ea, 0x80 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(lsr_long, fixture);
BOOST_AUTO_TEST_CASE( result_1)  {
	cpu.R[0] = 0x24000000;
	cpu.V = true;
	test_cpu( { 0161210 } );
	BOOST_TEST( cpu.R[0] == 0x12000000 );
	BOOST_TEST( cpu.V == false );
	BOOST_TEST( cpu.N == false );
}
BOOST_AUTO_TEST_CASE( result_2)  {
	cpu.R[0] = 0xc0000000;
	test_cpu( { 0161210 } );
	BOOST_TEST( cpu.R[0] == 0x60000000 );
}


BOOST_AUTO_TEST_CASE( by0 )  {
	cpu.C = cpu.X = true;
	cpu.R[0] = 0x230000;
	cpu.R[1] = 0;
	test_cpu( { 0161250 } );
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
	test_cpu( { 0161210 } );
	BOOST_TEST( cpu.C == cx );
	BOOST_TEST( cpu.X == cx );
}



BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	cpu.R[0] = z ? 1 : 2;
	test_cpu( { 0161210 } );
	BOOST_TEST( cpu.Z == z );
}

BOOST_AUTO_TEST_CASE(shift_by_imm)
{
	cpu.R[3] = 0x10000;
	test_cpu( { 0161213 } );
	BOOST_TEST( cpu.R[3] == 0x8000 );
}

BOOST_AUTO_TEST_CASE(shift_by_reg)
{
	cpu.R[4] = 2;
	cpu.R[3] = 0x10000;
	test_cpu( { 0164253 } );
	BOOST_TEST( cpu.R[3] == 0x4000  );
}

BOOST_AUTO_TEST_CASE(shift_by_reg_over_64)
{
	cpu.R[4] = 66;
	cpu.R[3] = 0x10000;
	test_cpu( { 0164253 } );
	BOOST_TEST( cpu.R[3] == 0x4000  );
}
BOOST_AUTO_TEST_SUITE_END();
