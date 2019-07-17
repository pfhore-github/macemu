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
BOOST_FIXTURE_TEST_SUITE(and_byte, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.X = xcpu.V = xcpu.C = true;
	xcpu.D[0] = 0xa3;
	xcpu.D[1] = 0xd2;
	test_cpu( { 0141000 } );
	BOOST_TEST( xcpu.D[1] == ( 0xa3 & 0xd2 ) );
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.V == false );
	BOOST_TEST( xcpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0x80 : 1;
	xcpu.D[1] = 0x80;
	test_cpu( { 0141000 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = 0xff;
	xcpu.D[1] = ! z;
	test_cpu( { 0141000 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dn_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 0xa5;
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140000 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.D[dst] == 0xdd );
	} else {
		BOOST_TEST( xcpu.D[dst] == 0x85 );
	}
}

BOOST_DATA_TEST_CASE(mem_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140020 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
}

BOOST_DATA_TEST_CASE(incr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140030 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
	BOOST_TEST( xcpu.A[src] == ( src == 7 ? 0x1002 : 0x1001) );
}

BOOST_DATA_TEST_CASE(decr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.A[src] = src == 7 ? 0x1002 : 0x1001;
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140040 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
	BOOST_TEST( xcpu.A[src] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1100, 0xa5);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140050 | dst << 9 | src), 0x100 } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
}

BOOST_DATA_TEST_CASE(d8anxn_dm,
					 bdata::xrange(8) * bdata::xrange(1, 8),
					 src, dst)
{
	test_write8(0x1040, 0xa5);
	xcpu.A[src] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140060 | dst << 9 | src),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
}

BOOST_DATA_TEST_CASE(xxxw_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140070 | dst << 9),
				0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
}

BOOST_DATA_TEST_CASE(xxxl_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140071 | dst << 9),
				0, 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
}

BOOST_DATA_TEST_CASE(d16pc_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write8(0x1002, 0xa5);
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140072 | dst << 9 ), 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
}

BOOST_DATA_TEST_CASE(d8pcxn_dm,
					 bdata::xrange(1, 8),
					 dst)
{
	test_write8(0x1032, 0xa5);
	xcpu.D[0] = 0x1000;
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140073 | dst << 9),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
}

BOOST_DATA_TEST_CASE(imm_dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 0xdd;
	test_cpu( { (uint16_t)(0140074 | dst << 9),
				0xa5 } );
	BOOST_TEST( xcpu.D[dst] == 0x85 );
}

BOOST_DATA_TEST_CASE(dm_mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 0xdd;
	test_cpu( { (uint16_t)(0140420 | src << 9 | dst) } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
}

BOOST_DATA_TEST_CASE(dm_incr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 0xdd;
	test_cpu( { (uint16_t)(0140430 | src << 9 | dst) } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
	BOOST_TEST( xcpu.A[dst] == (dst == 7 ? 0x1002 : 0x1001) );
}

BOOST_DATA_TEST_CASE(dm_decr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1000, 0xa5);
	xcpu.A[dst] = dst == 7 ? 0x1002 : 0x1001;
	xcpu.D[src] = 0xdd;
	test_cpu( { (uint16_t)(0140440 | src << 9 | dst) } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(dm_d16an,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1100, 0xa5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 0xdd;
	test_cpu( { (uint16_t)(0140450 | src << 9 | dst), 0x100 } );
	BOOST_TEST( test_read8(0x1100) == 0x85 );
}

BOOST_DATA_TEST_CASE(dm_d8anxn,
					 bdata::xrange(1,8) * bdata::xrange(8),
					 src, dst)
{
	test_write8(0x1040, 0xa5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[src] = 0xdd;
	test_cpu( { (uint16_t)(0140460 | src << 9 | dst),
				0x0830 } );
	BOOST_TEST( test_read8(0x1040) == 0x85 );
}

BOOST_DATA_TEST_CASE(dm_xxxw,
					 bdata::xrange(8),
					 src)
{
	test_write8(0x1000, 0xa5);
	xcpu.D[src] = 0xdd;
	test_cpu( { (uint16_t)(0140470 | src << 9),
				0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
}

BOOST_DATA_TEST_CASE(dm_xxxl,
					 bdata::xrange(8),
					 src)
{
	test_write8(0x1000, 0xa5);
	xcpu.D[src] = 0xdd;
	test_cpu( { (uint16_t)(0140471 | src << 9),
				0, 0x1000 } );
	BOOST_TEST( test_read8(0x1000) == 0x85 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(and_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.X = xcpu.V = xcpu.C = true;
	xcpu.D[0] = 0xa5a5;
	xcpu.D[1] = 0xdddd;
	test_cpu( { 0141100 } );
	BOOST_TEST( xcpu.D[1] == ( 0xa5a5 & 0xdddd ) );
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.V == false );
	BOOST_TEST( xcpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0x8000 : 1;
	xcpu.D[1] = 0xffff;
	test_cpu( { 0141100 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	xcpu.D[1] = 0xffff;
	test_cpu( { 0141100 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dn_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 0xa5a5;
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140100 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.D[dst] == 0xdddd );
	} else {
		BOOST_TEST( xcpu.D[dst] == 0x8585 );
	}
}

BOOST_DATA_TEST_CASE(mem_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140120 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
}

BOOST_DATA_TEST_CASE(incr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140130 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
	BOOST_TEST( xcpu.A[src] == 0x1002 );
}

BOOST_DATA_TEST_CASE(decr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.A[src] = 0x1002;
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140140 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
	BOOST_TEST( xcpu.A[src] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1100, 0xa5a5);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140150 | dst << 9 | src), 0x100 } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
}

BOOST_DATA_TEST_CASE(d8anxn_dm,
					 bdata::xrange(8) * bdata::xrange(1, 8),
					 src, dst)
{
	test_write16(0x1040, 0xa5a5);
	xcpu.A[src] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140160 | dst << 9 | src),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
}

BOOST_DATA_TEST_CASE(xxxw_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140170 | dst << 9),
				0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
}

BOOST_DATA_TEST_CASE(xxxl_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140171 | dst << 9),
				0, 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
}

BOOST_DATA_TEST_CASE(d16pc_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1002, 0xa5a5);
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140172 | dst << 9 ), 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
}

BOOST_DATA_TEST_CASE(d8pcxn_dm,
					 bdata::xrange(1, 8),
					 dst)
{
	test_write16(0x1032, 0xa5a5);
	xcpu.D[0] = 0x1000;
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140173 | dst << 9),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
}

BOOST_DATA_TEST_CASE(imm_dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 0xdddd;
	test_cpu( { (uint16_t)(0140174 | dst << 9),
				0xa5a5 } );
	BOOST_TEST( xcpu.D[dst] == 0x8585 );
}

BOOST_DATA_TEST_CASE(dm_mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 0xdddd;
	test_cpu( { (uint16_t)(0140520 | src << 9 | dst) } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
}

BOOST_DATA_TEST_CASE(dm_incr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 0xdddd;
	test_cpu( { (uint16_t)(0140530 | src << 9 | dst) } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
	BOOST_TEST( xcpu.A[dst] == 0x1002 );
}

BOOST_DATA_TEST_CASE(dm_decr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.A[dst] = 0x1002;
	xcpu.D[src] = 0xdddd;
	test_cpu( { (uint16_t)(0140540 | src << 9 | dst) } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(dm_d16an,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1100, 0xa5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 0xdddd;
	test_cpu( { (uint16_t)(0140550 | src << 9 | dst), 0x100 } );
	BOOST_TEST( test_read16(0x1100) == 0x8585 );
}

BOOST_DATA_TEST_CASE(dm_d8anxn,
					 bdata::xrange(1, 8) * bdata::xrange(8),
					 src, dst)
{
	test_write16(0x1040, 0xa5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[src] = 0xdddd;
	test_cpu( { (uint16_t)(0140560 | src << 9 | dst),
				0x0830 } );
	BOOST_TEST( test_read16(0x1040) == 0x8585 );
}

BOOST_DATA_TEST_CASE(dm_xxxw,
					 bdata::xrange(8),
					 src)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.D[src] = 0xdddd;
	test_cpu( { (uint16_t)(0140570 | src << 9),
				0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
}

BOOST_DATA_TEST_CASE(dm_xxxl,
					 bdata::xrange(8),
					 src)
{
	test_write16(0x1000, 0xa5a5);
	xcpu.D[src] = 0xdddd;
	test_cpu( { (uint16_t)(0140571 | src << 9),
				0, 0x1000 } );
	BOOST_TEST( test_read16(0x1000) == 0x8585 );
}


BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(and_long, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.X = xcpu.V = xcpu.C = true;
	xcpu.D[0] = 0xa5a5a5a5;
	xcpu.D[1] = 0xdddddddd;
	test_cpu( { 0141200 } );
	BOOST_TEST( xcpu.D[1] == ( 0xa5a5a5a5 &  0xdddddddd ) );
	BOOST_TEST( xcpu.X == true );
	BOOST_TEST( xcpu.V == false );
	BOOST_TEST( xcpu.C == false );
}

BOOST_DATA_TEST_CASE(n,
					 bdata::xrange(2),
					 n
	)
{
	xcpu.D[0] = n ? 0xf0000000 : 1;
	xcpu.D[1] = 0xffffffff;
	test_cpu( { 0141200 } );
	BOOST_TEST( xcpu.N == n );
}

BOOST_DATA_TEST_CASE(z,
					 bdata::xrange(2),
					 z
	)
{
	xcpu.D[0] = ! z;
	xcpu.D[1] = 0xffffffff;
	test_cpu( { 0141200 } );
	BOOST_TEST( xcpu.Z == z );
}

BOOST_DATA_TEST_CASE(dn_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 0xa5a5a5a5;
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140200 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.D[dst] == 0xdddddddd );
	} else {
		BOOST_TEST( xcpu.D[dst] == 0x85858585 );
	}
}

BOOST_DATA_TEST_CASE(mem_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140220 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
}

BOOST_DATA_TEST_CASE(incr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140230 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
	BOOST_TEST( xcpu.A[src] == 0x1004 );
}

BOOST_DATA_TEST_CASE(decr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.A[src] = 0x1004;
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140240 | dst << 9 | src) } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
	BOOST_TEST( xcpu.A[src] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1100, 0xa5a5a5a5);
	xcpu.A[src] = 0x1000;
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140250 | dst << 9 | src), 0x100 } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
}

BOOST_DATA_TEST_CASE(d8anxn_dm,
					 bdata::xrange(8) * bdata::xrange(1, 8),
					 src, dst)
{
	test_write32(0x1040, 0xa5a5a5a5);
	xcpu.A[src] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140260 | dst << 9 | src),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
}

BOOST_DATA_TEST_CASE(xxxw_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140270 | dst << 9),
				0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
}

BOOST_DATA_TEST_CASE(xxxl_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140271 | dst << 9),
				0, 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
}

BOOST_DATA_TEST_CASE(d16pc_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1002, 0xa5a5a5a5);
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140272 | dst << 9 ), 0x1000 } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
}

BOOST_DATA_TEST_CASE(d8pcxn_dm,
					 bdata::xrange(1, 8),
					 dst)
{
	test_write32(0x1032, 0xa5a5a5a5);
	xcpu.D[0] = 0x1000;
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140273 | dst << 9),
				0x0830 } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
}

BOOST_DATA_TEST_CASE(imm_dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.D[dst] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140274 | dst << 9),
				uint16_t(0xa5a5a5a5>>16), uint16_t(0xa5a5a5a5&0xffff) } );
	BOOST_TEST( xcpu.D[dst] == 0x85858585 );
}

BOOST_DATA_TEST_CASE(dm_mem,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140620 | src << 9 | dst) } );
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
}

BOOST_DATA_TEST_CASE(dm_incr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140630 | src << 9 | dst) } );
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
	BOOST_TEST( xcpu.A[dst] == 0x1004 );
}

BOOST_DATA_TEST_CASE(dm_decr,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1004;
	xcpu.D[src] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140640 | src << 9 | dst) } );
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
	BOOST_TEST( xcpu.A[dst] == 0x1000 );
}

BOOST_DATA_TEST_CASE(dm_d16an,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1100, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[src] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140650 | src << 9 | dst), 0x100 } );
	BOOST_TEST( test_read32(0x1100) == 0x85858585 );
}

BOOST_DATA_TEST_CASE(dm_d8anxn,
					 bdata::xrange(1,8) * bdata::xrange(8),
					 src, dst)
{
	test_write32(0x1040, 0xa5a5a5a5);
	xcpu.A[dst] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.D[src] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140660 | src << 9 | dst),
				0x0830 } );
	BOOST_TEST( test_read32(0x1040) == 0x85858585 );
}

BOOST_DATA_TEST_CASE(dm_xxxw,
					 bdata::xrange(8),
					 src)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.D[src] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140670 | src << 9),
				0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
}

BOOST_DATA_TEST_CASE(dm_xxxl,
					 bdata::xrange(8),
					 src)
{
	test_write32(0x1000, 0xa5a5a5a5);
	xcpu.D[src] = 0xdddddddd;
	test_cpu( { (uint16_t)(0140671 | src << 9),
				0, 0x1000 } );
	BOOST_TEST( test_read32(0x1000) == 0x85858585 );
}
BOOST_AUTO_TEST_SUITE_END();
