#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "init.h"
#include "../registers.hpp"
#include "../exceptions.hpp"
#include <vector>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "test_common.hpp"

extern uint8_t *RAMBaseHost;
namespace bdata = boost::unit_test::data;
extern CPU xcpu;
void test_cpu(const std::vector<uint16_t>& c);
BOOST_TEST_GLOBAL_FIXTURE(init);
BOOST_FIXTURE_TEST_SUITE(add_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.D[0] = 23;
	xcpu.D[1] = 34;
	test_cpu( { 0151000 } );
	BOOST_TEST( xcpu.D[1] == ( 23 + 34 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0xff;
	xcpu.D[1] = xc;
	test_cpu( { 0151000 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7f;
	xcpu.D[1] = v;
	test_cpu( { 0151000 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x80;
	xcpu.D[1] = 0x7f + v;
	test_cpu( { 0151000 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0 : 1;
	xcpu.D[1] = 1;
	test_cpu( { 0151000 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	xcpu.D[1] = ! z;
	test_cpu( { 0151000 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dn_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 23;
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150000 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.D[dst] == 68 );
	} else {
		BOOST_TEST( xcpu.D[dst] == 57 );
	}
}

BOOST_DATA_TEST_CASE(mem_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 23);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150020 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 57 );
}

BOOST_DATA_TEST_CASE(incr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 23);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150030 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 57 );
	BOOST_TEST( xcpu.A[src] == ( src == 7 ? 0x1002 : 0x1001) );
}

BOOST_DATA_TEST_CASE(decr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 23);
	xcpu.A[src] = src == 7 ? 0x1002 : 0x1001;
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150040 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 57 );
	BOOST_TEST( xcpu.A[src] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1100, 23);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150050 | dst << 9 | src), 0x100 } );
	BOOST_TEST( xcpu.D[dst] == 57 );
}

BOOST_DATA_TEST_CASE(d8anxn_dm,
					 bdata::xrange(8) * bdata::xrange(1, 8),
					 src, dst)
{
	test_write8(0x1040, 23);
	xcpu.A[src] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150060 | dst << 9 | src),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 57 );
}

BOOST_DATA_TEST_CASE(xxxw_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 23);
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150070 | dst << 9),
				0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 57 );
}

BOOST_DATA_TEST_CASE(xxxl_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 23);
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150071 | dst << 9),
				0, 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 57 );
}

BOOST_DATA_TEST_CASE(d16pc_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1002, 23);
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150072 | dst << 9 ), 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 57 );
}

BOOST_DATA_TEST_CASE(d8pcxn_dm,
					 bdata::xrange(1, 8),
					 dst)
{
	test_write8(0x1032, 23);
	xcpu.D[0] = 0x1000;
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150073 | dst << 9),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 57 );
}

BOOST_DATA_TEST_CASE(imm_dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 34;
	test_cpu( { (uint16_t)(0150074 | dst << 9),
				23 } );
	BOOST_TEST( xcpu.D[dst] == 57 );
}


BOOST_DATA_TEST_CASE(dm_mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 23);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 34;
	test_cpu( { (uint16_t)(0150420 | src << 9 | dst) } );
	BOOST_TEST( test_read8(0x1000) == 57 );
}

BOOST_DATA_TEST_CASE(dm_incr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 23);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 34;
	test_cpu( { (uint16_t)(0150430 | src << 9 | dst) } );
	BOOST_TEST( test_read8(0x1000) == 57 );
	BOOST_TEST( xcpu.A[dst] == (dst == 7 ? 0x1002 : 0x1001) );
}

BOOST_DATA_TEST_CASE(dm_decr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 23);
	xcpu.A[dst] = dst == 7 ? 0x1002 : 0x1001;
	xcpu.D[src] = 34;
	test_cpu( { (uint16_t)(0150440 | src << 9 | dst) } );
	BOOST_TEST( test_read8(0x1000) == 57 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(dm_d16an,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1100, 23);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 34;
	test_cpu( { (uint16_t)(0150450 | src << 9 | dst), 0x100 } );
	BOOST_TEST( test_read8(0x1100) == 57 );
}

BOOST_DATA_TEST_CASE(dm_d8anxn,
					 bdata::xrange(1,8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1040, 23);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[src] = 34;
	test_cpu( { (uint16_t)(0150460 | src << 9 | dst),
				0x0830 } );
	BOOST_TEST( test_read8(0x1040) == 57 );
}

BOOST_DATA_TEST_CASE(dm_xxxw,
					 bdata::xrange(8),
					 src)
{
	test_write8(0x1000, 23);
	xcpu.D[src] = 34;
	test_cpu( { (uint16_t)(0150470 | src << 9),
				0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 57 );
}

BOOST_DATA_TEST_CASE(dm_xxxl,
					 bdata::xrange(8),
					 src)
{
	test_write8(0x1000, 23);
	xcpu.D[src] = 34;
	test_cpu( { (uint16_t)(0150471 | src << 9),
				0, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 57 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(add_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.D[0] = 2300;
	xcpu.D[1] = 3400;
	test_cpu( { 0151100 } );
	BOOST_TEST( xcpu.D[1] == ( 2300 + 3400 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0xffff;
	xcpu.D[1] = xc;
	test_cpu( { 0151100 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7fff;
	xcpu.D[1] = v;
	test_cpu( { 0151100 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x8000;
	xcpu.D[1] = 0x7fff + v;
	test_cpu( { 0151100 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf000 : 1;
	xcpu.D[1] = 1;
	test_cpu( { 0151100 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	xcpu.D[1] = ! z;
	test_cpu( { 0151100 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dn_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 2300;
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150100 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.D[dst] == 6800 );
	} else {
		BOOST_TEST( xcpu.D[dst] == 5700 );
	}
}

BOOST_DATA_TEST_CASE(mem_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150120 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
}

BOOST_DATA_TEST_CASE(incr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150130 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
	BOOST_TEST( xcpu.A[src] == 0x1002 );
}

BOOST_DATA_TEST_CASE(decr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[src] = 0x1002;
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150140 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
	BOOST_TEST( xcpu.A[src] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1100, 2300);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150150 | dst << 9 | src), 0x100 } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
}

BOOST_DATA_TEST_CASE(d8anxn_dm,
					 bdata::xrange(8) * bdata::xrange(1, 8),
					 src, dst)
{
	test_write16(0x1040, 2300);
	xcpu.A[src] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150160 | dst << 9 | src),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
}

BOOST_DATA_TEST_CASE(xxxw_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 2300);
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150170 | dst << 9),
				0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
}

BOOST_DATA_TEST_CASE(xxxl_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 2300);
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150171 | dst << 9),
				0, 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
}

BOOST_DATA_TEST_CASE(d16pc_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1002, 2300);
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150172 | dst << 9 ), 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
}

BOOST_DATA_TEST_CASE(d8pcxn_dm,
					 bdata::xrange(1, 8),
					 dst)
{
	test_write16(0x1032, 2300);
	xcpu.D[0] = 0x1000;
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150173 | dst << 9),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
}

BOOST_DATA_TEST_CASE(imm_dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 3400;
	test_cpu( { (uint16_t)(0150174 | dst << 9),
				2300 } );
	BOOST_TEST( xcpu.D[dst] == 5700 );
}

BOOST_DATA_TEST_CASE(dm_mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 3400;
	test_cpu( { (uint16_t)(0150520 | src << 9 | dst) } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
}

BOOST_DATA_TEST_CASE(dm_incr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 3400;
	test_cpu( { (uint16_t)(0150530 | src << 9 | dst) } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
	BOOST_TEST( xcpu.A[dst] == 0x1002 );
}

BOOST_DATA_TEST_CASE(dm_decr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 2300);
	xcpu.A[dst] = 0x1002;
	xcpu.D[src] = 3400;
	test_cpu( { (uint16_t)(0150540 | src << 9 | dst) } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(dm_d16an,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1100, 2300);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 3400;
	test_cpu( { (uint16_t)(0150550 | src << 9 | dst), 0x100 } );
	BOOST_TEST( test_read16(0x1100) == 5700 );
}

BOOST_DATA_TEST_CASE(dm_d8anxn,
					 bdata::xrange(1, 8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1040, 2300);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[src] = 3400;
	test_cpu( { (uint16_t)(0150560 | src << 9 | dst),
				0x0830 } );
	BOOST_TEST( test_read16(0x1040) == 5700 );
}

BOOST_DATA_TEST_CASE(dm_xxxw,
					 bdata::xrange(8),
					 src)
{
	test_write16(0x1000, 2300);
	xcpu.D[src] = 3400;
	test_cpu( { (uint16_t)(0150570 | src << 9),
				0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
}

BOOST_DATA_TEST_CASE(dm_xxxl,
					 bdata::xrange(8),
					 src)
{
	test_write16(0x1000, 2300);
	xcpu.D[src] = 3400;
	test_cpu( { (uint16_t)(0150571 | src << 9),
				0, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 5700 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(add_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.D[0] = 230000;
	xcpu.D[1] = 340000;
	test_cpu( { 0151200 } );
	BOOST_TEST( xcpu.D[1] == ( 230000 + 340000 ) );
}

BOOST_DATA_TEST_CASE(xc,
					 bdata::xrange(2),
					 xc
	)
{
	xcpu.D[0] = 0xffffffff;
	xcpu.D[1] = xc;
	test_cpu( { 0151200 } );
	BOOST_TEST( xcpu.C == xc );
	BOOST_TEST( xcpu.X == xc );
}

BOOST_DATA_TEST_CASE(v_1,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x7fffffff;
	xcpu.D[1] = v;
	test_cpu( { 0151200 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(v_2,
					 bdata::xrange(2),
					 v
	)
{
	xcpu.D[0] = 0x80000000;
	xcpu.D[1] = 0x7fffffff + v;
	test_cpu( { 0151200 } );
	BOOST_TEST( xcpu.V == v );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0000000 : 1;
	xcpu.D[1] = 1;
	test_cpu( { 0151200 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	xcpu.D[1] = ! z;
	test_cpu( { 0151200 } );
	BOOST_TEST( xcpu.Z == z );
}
BOOST_DATA_TEST_CASE(dn_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 230000;
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150200 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.D[dst] == 680000 );
	} else {
		BOOST_TEST( xcpu.D[dst] == 570000 );
	}
}

BOOST_DATA_TEST_CASE(mem_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150220 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
}

BOOST_DATA_TEST_CASE(incr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150230 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
	BOOST_TEST( xcpu.A[src] == 0x1004 );
}

BOOST_DATA_TEST_CASE(decr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[src] = 0x1004;
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150240 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
	BOOST_TEST( xcpu.A[src] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1100, 230000);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150250 | dst << 9 | src), 0x100 } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
}

BOOST_DATA_TEST_CASE(d8anxn_dm,
					 bdata::xrange(8) * bdata::xrange(1, 8),
					 src, dst)
{
	test_write32(0x1040, 230000);
	xcpu.A[src] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150260 | dst << 9 | src),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
}

BOOST_DATA_TEST_CASE(xxxw_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 230000);
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150270 | dst << 9),
				0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
}

BOOST_DATA_TEST_CASE(xxxl_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 230000);
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150271 | dst << 9),
				0, 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
}

BOOST_DATA_TEST_CASE(d16pc_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1002, 230000);
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150272 | dst << 9 ), 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
}

BOOST_DATA_TEST_CASE(d8pcxn_dm,
					 bdata::xrange(1, 8),
					 dst)
{
	test_write32(0x1032, 230000);
	xcpu.D[0] = 0x1000;
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150273 | dst << 9),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
}

BOOST_DATA_TEST_CASE(imm_dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 340000;
	test_cpu( { (uint16_t)(0150274 | dst << 9),
				uint16_t(230000>>16), uint16_t(230000&0xffff) } );
	BOOST_TEST( xcpu.D[dst] == 570000 );
}

BOOST_DATA_TEST_CASE(dm_mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 340000;
	test_cpu( { (uint16_t)(0150620 | src << 9 | dst) } );
	BOOST_TEST( test_read32(0x1000) == 570000 );
}

BOOST_DATA_TEST_CASE(dm_incr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 340000;
	test_cpu( { (uint16_t)(0150630 | src << 9 | dst) } );
	BOOST_TEST( test_read32(0x1000) == 570000 );
	BOOST_TEST( xcpu.A[dst] == 0x1004 );
}

BOOST_DATA_TEST_CASE(dm_decr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 230000);
	xcpu.A[dst] = 0x1004;
	xcpu.D[src] = 340000;
	test_cpu( { (uint16_t)(0150640 | src << 9 | dst) } );
	BOOST_TEST( test_read32(0x1000) == 570000 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(dm_d16an,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1100, 230000);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 340000;
	test_cpu( { (uint16_t)(0150650 | src << 9 | dst), 0x100 } );
	BOOST_TEST( test_read32(0x1100) == 570000 );
}

BOOST_DATA_TEST_CASE(dm_d8anxn,
					 bdata::xrange(1,8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1040, 230000);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[src] = 340000;
	test_cpu( { (uint16_t)(0150660 | src << 9 | dst),
				0x0830 } );
	BOOST_TEST( test_read32(0x1040) == 570000 );
}

BOOST_DATA_TEST_CASE(dm_xxxw,
					 bdata::xrange(8),
					 src)
{
	test_write32(0x1000, 230000);
	xcpu.D[src] = 340000;
	test_cpu( { (uint16_t)(0150670 | src << 9),
				0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 570000 );
}

BOOST_DATA_TEST_CASE(dm_xxxl,
					 bdata::xrange(8),
					 src)
{
	test_write32(0x1000, 230000);
	xcpu.D[src] = 340000;
	test_cpu( { (uint16_t)(0150671 | src << 9),
				0, 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 570000 );
}

BOOST_AUTO_TEST_SUITE_END();
