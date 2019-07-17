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
BOOST_FIXTURE_TEST_SUITE(asl_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.D[0] = 0x23;
	test_cpu( { 0161400 } );
	BOOST_TEST( xcpu.D[0] == 0x46 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	xcpu.C = xcpu.X = true;
	xcpu.D[0] = 0x23;
	xcpu.D[1] = 0;
	test_cpu( { 0161440 } );
	BOOST_TEST( xcpu.D[0] == 0x23 );
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.C == false );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	xcpu.D[0] = cx ? 0x80 : 0x40;
	test_cpu( { 0161400 } );
	BOOST_TEST( xcpu.C == cx );
	BOOST_TEST( xcpu.X == cx );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0x40 : 0x20;
	test_cpu( { 0161400 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0x80 : 0xff;
	test_cpu( { 0161400 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_3,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0x40 : 0x10;
	test_cpu( { 0162400 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_4,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0xa0 : 0xff;
	test_cpu( { 0161400 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xff : 0;
	test_cpu( { 0161400 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = z ? 0 : 1;
	test_cpu( { 0161400 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(shift_by_imm,
					 bdata::xrange(8) * bdata::xrange(8),
					 imm, dst
	)
{
	xcpu.D[dst] = 1;
	test_cpu( {(uint16_t)( 0160400 | imm << 9 | dst) } );
	BOOST_TEST( xcpu.D[dst] == (uint8_t)(1 << (imm ? imm : 8)) );
}

BOOST_DATA_TEST_CASE(shift_by_reg,
					 bdata::xrange(8) * bdata::xrange(8),
					 r, dst
	)
{
	xcpu.D[r] = 2;
	xcpu.D[dst] = 1;
	test_cpu( {(uint16_t)( 0160440 | r << 9 | dst) } );
	if( r == dst ) {
		BOOST_TEST( xcpu.D[dst] == (1 << 1) );
	} else {
		BOOST_TEST( xcpu.D[dst] == (1 << 2) );
	}
}

BOOST_DATA_TEST_CASE(shift_by_reg_over_64,
					 bdata::xrange(8) * bdata::xrange(8),
					 r, dst
	)
{
	xcpu.D[r] = 66;
	xcpu.D[dst] = 1;
	test_cpu( {(uint16_t)( 0160440 | r << 9 | dst) } );
	if( r == dst ) {
		BOOST_TEST( xcpu.D[dst] == (1 << 1) );
	} else {
		BOOST_TEST( xcpu.D[dst] == (1 << 2) );
	}
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(asl_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.D[0] = 0x2300;
	test_cpu( { 0161500 } );
	BOOST_TEST( xcpu.D[0] == 0x4600 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	xcpu.C = xcpu.X = true;
	xcpu.D[0] = 0x2300;
	xcpu.D[1] = 0;
	test_cpu( { 0161540 } );
	BOOST_TEST( xcpu.D[0] == 0x2300 );
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.C == false );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	xcpu.D[0] = cx ? 0x8000 : 0x4000;
	test_cpu( { 0161500 } );
	BOOST_TEST( xcpu.C == cx );
	BOOST_TEST( xcpu.X == cx );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0x4000 : 0x2000;
	test_cpu( { 0161500 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0x8000 : 0xffff;
	test_cpu( { 0161500 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_3,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0x4000 : 0x1000;
	test_cpu( { 0162500 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_4,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0xa000 : 0xffff;
	test_cpu( { 0161500 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xffff : 0;
	test_cpu( { 0161500 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = z ? 0 : 1;
	test_cpu( { 0161500 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(shift_by_imm,
					 bdata::xrange(8) * bdata::xrange(8),
					 imm, dst
	)
{
	xcpu.D[dst] = 0x100;
	test_cpu( {(uint16_t)( 0160500 | imm << 9 | dst) } );
	BOOST_TEST( xcpu.D[dst] == (uint16_t)(0x100 << (imm ? imm : 8)) );
}

BOOST_DATA_TEST_CASE(shift_by_reg,
					 bdata::xrange(8) * bdata::xrange(8),
					 r, dst
	)
{
	xcpu.D[r] = 2;
	xcpu.D[dst] = 0x100;
	test_cpu( {(uint16_t)( 0160540 | r << 9 | dst) } );
	if( r == dst ) {
		BOOST_TEST( xcpu.D[dst] == 0x100 );
	} else {
		BOOST_TEST( xcpu.D[dst] == (0x100 << 2) );
	}
}

BOOST_DATA_TEST_CASE(shift_by_reg_over_64,
					 bdata::xrange(8) * bdata::xrange(8),
					 r, dst
	)
{
	xcpu.D[r] = 66;
	xcpu.D[dst] = 0x100;
	test_cpu( {(uint16_t)( 0160540 | r << 9 | dst) } );
	if( r == dst ) {
		BOOST_TEST( xcpu.D[dst] == 0x100 );
	} else {
		BOOST_TEST( xcpu.D[dst] == (0x100 << 2) );
	}
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 0x100);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0160720 | dst )} );
	BOOST_TEST( test_read16(0x1000) == 0x200 );
}

BOOST_DATA_TEST_CASE(incr,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 0x100);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0160730 | dst )} );
	BOOST_TEST( test_read16(0x1000) == 0x200 );
	BOOST_TEST( xcpu.A[dst] == 0x1002 );
}

BOOST_DATA_TEST_CASE(decr,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 0x100);
	xcpu.A[dst] = 0x1002;
	test_cpu( { (uint16_t)(0160740 | dst )} );
	BOOST_TEST( test_read16(0x1000) == 0x200 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1100, 0x100);
	xcpu.A[dst] = 0x1000;
	test_cpu( { (uint16_t)(0160750 | dst ), 0x100 } );
	BOOST_TEST( test_read16(0x1100) == 0x200 );
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1040, 0x100);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	test_cpu( { (uint16_t)(0160760 | dst ), 0x0830 } );
	BOOST_TEST( test_read16(0x1040) == 0x200 );
}

BOOST_AUTO_TEST_CASE(xxxw)
{
	test_write16(0x1000, 0x100);
	test_cpu( { 0160770, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 0x200 );
}

BOOST_AUTO_TEST_CASE(xxxl)
{
	test_write16(0x1000, 0x100);
	test_cpu( { 0160771, 0, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 0x200 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(asl_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.D[0] = 0x230000;
	test_cpu( { 0161600 } );
	BOOST_TEST( xcpu.D[0] == 0x460000 );
}

BOOST_AUTO_TEST_CASE( by0 )  {
	xcpu.C = xcpu.X = true;
	xcpu.D[0] = 0x230000;
	xcpu.D[1] = 0;
	test_cpu( { 0161640 } );
	BOOST_TEST( xcpu.D[0] == 0x230000 );
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.C == false );
}

BOOST_DATA_TEST_CASE(cx,
					 bdata::xrange(2),
					 cx
	)
{
	xcpu.D[0] = cx ? 0x80000000 : 0x40000000;
	test_cpu( { 0161600 } );
	BOOST_TEST( xcpu.C == cx );
	BOOST_TEST( xcpu.X == cx );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0x40000000 : 0x20000000;
	test_cpu( { 0161600 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0x80000000 : 0xffffffff;
	test_cpu( { 0161600 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_3,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0x40000000 : 0x10000000;
	test_cpu( { 0162600 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_4,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = v ? 0xa0000000 : 0xffffffff;
	test_cpu( { 0161600 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xffffffff : 0;
	test_cpu( { 0161600 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = z ? 0 : 1;
	test_cpu( { 0161600 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(shift_by_imm,
					 bdata::xrange(8) * bdata::xrange(8),
					 imm, dst
	)
{
	xcpu.D[dst] = 0x10000;
	test_cpu( {(uint16_t)( 0160600 | imm << 9 | dst) } );
	BOOST_TEST( xcpu.D[dst] == (0x10000 << (imm ? imm : 8)) );
}

BOOST_DATA_TEST_CASE(shift_by_reg,
					 bdata::xrange(8) * bdata::xrange(8),
					 r, dst
	)
{
	xcpu.D[r] = 2;
	xcpu.D[dst] = 0x10000;
	test_cpu( {(uint16_t)( 0160640 | r << 9 | dst) } );
	if( r == dst ) {
		BOOST_TEST( xcpu.D[dst] == 0x10000 );
	} else {
		BOOST_TEST( xcpu.D[dst] == (0x10000 << 2) );
	}
}

BOOST_DATA_TEST_CASE(shift_by_reg_over_64,
					 bdata::xrange(8) * bdata::xrange(8),
					 r, dst
	)
{
	xcpu.D[r] = 66;
	xcpu.D[dst] = 0x10000;
	test_cpu( {(uint16_t)( 0160640 | r << 9 | dst) } );
	if( r == dst ) {
		BOOST_TEST( xcpu.D[dst] == 0x10000 );
	} else {
		BOOST_TEST( xcpu.D[dst] == (0x10000 << 2) );
	}
}

BOOST_AUTO_TEST_SUITE_END();
