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
BOOST_FIXTURE_TEST_SUITE(adda_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.N = xcpu.Z = xcpu.V = xcpu.C = xcpu.X = true;
	xcpu.A[0] = 0x2000;
	xcpu.D[0] = 0x50;
	test_cpu( { 0150300 } );
	BOOST_TEST( xcpu.A[0] == 0x2050 );
	BOOST_TEST( xcpu.N == true );
	BOOST_TEST( xcpu.Z == true );
	BOOST_TEST( xcpu.V == true );
	BOOST_TEST( xcpu.C == true );
	BOOST_TEST( xcpu.X == true );
}
BOOST_AUTO_TEST_CASE( resultX)  {
	xcpu.A[0] = 0xa000;
	xcpu.D[0] = 0x50;
	test_cpu( { 0150300 } );
	BOOST_TEST( xcpu.A[0] == 0xffffa050 );
}

BOOST_DATA_TEST_CASE(dn_am,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 0x2000;
	xcpu.A[dst] = 0x3000;
	test_cpu( { (uint16_t)(0150300 | dst << 9 | src) } );
	BOOST_TEST( xcpu.A[dst] == 0x5000 );
}

BOOST_DATA_TEST_CASE(an_am,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.A[src] = 0x2000;
	xcpu.A[dst] = 0x3000;
	test_cpu( { (uint16_t)(0150310 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.A[dst] == 0x6000 );
	} else {
		BOOST_TEST( xcpu.A[dst] == 0x5000 );
	}
}

BOOST_DATA_TEST_CASE(mem_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	
	test_write16(0x1000, 0x2000);
	xcpu.A[dst] = 0x3000;
	xcpu.A[src] = 0x1000;
	test_cpu( { (uint16_t)(0150320 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.A[dst] == 0x3000 );
	} else {
		BOOST_TEST( xcpu.A[dst] == 0x5000 );
	}
}
BOOST_DATA_TEST_CASE(incr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	if( src == dst )
		return;
	test_write16(0x1000, 0x2000);
	xcpu.A[src] = 0x1000;
	xcpu.A[dst] = 0x4000;
	test_cpu( { (uint16_t)(0150330 | dst << 9 | src) } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
	BOOST_TEST( xcpu.A[src] == 0x1002 );
}

BOOST_DATA_TEST_CASE(decr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	if( src == dst )
		return;
	test_write16(0x1000, 0x2000);
	xcpu.A[src] = 0x1002;
	xcpu.A[dst] = 0x4000;
	test_cpu( { (uint16_t)(0150340 | dst << 9 | src) } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
	BOOST_TEST( xcpu.A[src] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	if( src == dst )
		return;
	test_write16(0x1100, 0x2000);
	xcpu.A[src] = 0x1000;
	xcpu.A[dst] = 0x4000;
	test_cpu( { (uint16_t)(0150350 | dst << 9 | src), 0x100 } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
}

BOOST_DATA_TEST_CASE(d8anxn_dm,
					 bdata::xrange(8) * bdata::xrange(1, 8),
					 src, dst)
{
	if( src == dst )
		return;
	test_write16(0x1040, 0x2000);
	xcpu.A[src] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.A[dst] = 0x4000;
	test_cpu( { (uint16_t)(0150360 | dst << 9 | src),
				0x0830 } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
}

BOOST_DATA_TEST_CASE(xxxw_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 0x4000);
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150370 | dst << 9),
				0x1000 } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
}

BOOST_DATA_TEST_CASE(xxxl_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1000, 0x4000);
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150371 | dst << 9),
				0, 0x1000 } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
}

BOOST_DATA_TEST_CASE(d16pc_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write16(0x1002, 0x3000);
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150372 | dst << 9 ), 0x1000 } );
	BOOST_TEST( xcpu.A[dst] == 0x5000 );
}

BOOST_DATA_TEST_CASE(d8pcxn_dm,
					 bdata::xrange(1, 8),
					 dst)
{
	test_write16(0x1032, 0x3000);
	xcpu.D[0] = 0x1000;
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150373 | dst << 9),
				0x0830 } );
	BOOST_TEST( xcpu.A[dst] == 0x5000 );
}

BOOST_DATA_TEST_CASE(imm_dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150374 | dst << 9),
				0x3000 } );
	BOOST_TEST( xcpu.A[dst] == 0x5000 );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(addw_word, fixture);
BOOST_AUTO_TEST_CASE( result)  {
	xcpu.N = xcpu.Z = xcpu.V = xcpu.C = xcpu.X = true;
	xcpu.A[0] = 0x20000000;
	xcpu.D[0] = 0x5000;
	test_cpu( { 0150700 } );
	BOOST_TEST( xcpu.A[0] == 0x20005000 );
	BOOST_TEST( xcpu.N == true );
	BOOST_TEST( xcpu.Z == true );
	BOOST_TEST( xcpu.V == true );
	BOOST_TEST( xcpu.C == true );
	BOOST_TEST( xcpu.X == true );
}
BOOST_AUTO_TEST_CASE( resultX)  {
	xcpu.A[0] = 0xa000;
	xcpu.D[0] = 0x50;
	test_cpu( { 0150700 } );
	BOOST_TEST( xcpu.A[0] == 0xa050 );
}

BOOST_DATA_TEST_CASE(dn_am,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.D[src] = 0x20000000;
	xcpu.A[dst] = 0x30000000;
	test_cpu( { (uint16_t)(0150700 | dst << 9 | src) } );
	BOOST_TEST( xcpu.A[dst] == 0x50000000 );
}

BOOST_DATA_TEST_CASE(an_am,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	xcpu.A[src] = 0x20000000;
	xcpu.A[dst] = 0x30000000;
	test_cpu( { (uint16_t)(0150710 | dst << 9 | src) } );
	if( dst == src ) {
		BOOST_TEST( xcpu.A[dst] == 0x60000000 );
	} else {
		BOOST_TEST( xcpu.A[dst] == 0x50000000 );
	}
}

BOOST_DATA_TEST_CASE(mem_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	
	if( src == dst )
		return;
	test_write32(0x1000, 0x20000000);
	xcpu.A[src] = 0x1000;
	xcpu.A[dst] = 0x30000000;
	test_cpu( { (uint16_t)(0150720 | dst << 9 | src) } );
	BOOST_TEST( xcpu.A[dst] == 0x50000000 );
}
BOOST_DATA_TEST_CASE(incr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	if( src == dst )
		return;
	test_write32(0x1000, 0x20000000);
	xcpu.A[src] = 0x1000;
	xcpu.A[dst] = 0x40000000;
	test_cpu( { (uint16_t)(0150730 | dst << 9 | src) } );
	BOOST_TEST( xcpu.A[dst] == 0x60000000 );
	BOOST_TEST( xcpu.A[src] == 0x1004 );
}

BOOST_DATA_TEST_CASE(decr_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	if( src == dst )
		return;
	test_write32(0x1000, 0x20000000);
	xcpu.A[src] = 0x1004;
	xcpu.A[dst] = 0x40000000;
	test_cpu( { (uint16_t)(0150740 | dst << 9 | src) } );
	BOOST_TEST( xcpu.A[dst] == 0x60000000 );
	BOOST_TEST( xcpu.A[src] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an_dm,
					 bdata::xrange(8) * bdata::xrange(8),
					 src, dst)
{
	if( src == dst )
		return;
	test_write32(0x1100, 0x2000);
	xcpu.A[src] = 0x1000;
	xcpu.A[dst] = 0x4000;
	test_cpu( { (uint16_t)(0150750 | dst << 9 | src), 0x100 } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
}

BOOST_DATA_TEST_CASE(d8anxn_dm,
					 bdata::xrange(8) * bdata::xrange(1, 8),
					 src, dst)
{
	if( src == dst )
		return;
	test_write32(0x1040, 0x2000);
	xcpu.A[src] = 0x1000;
	xcpu.D[0] = 0x10;
	xcpu.A[dst] = 0x4000;
	test_cpu( { (uint16_t)(0150760 | dst << 9 | src),
				0x0830 } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
}

BOOST_DATA_TEST_CASE(xxxw_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 0x4000);
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150770 | dst << 9),
				0x1000 } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
}

BOOST_DATA_TEST_CASE(xxxl_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1000, 0x4000);
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150771 | dst << 9),
				0, 0x1000 } );
	BOOST_TEST( xcpu.A[dst] == 0x6000 );
}

BOOST_DATA_TEST_CASE(d16pc_dm,
					 bdata::xrange(8),
					 dst)
{
	test_write32(0x1002, 0x3000);
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150772 | dst << 9 ), 0x1000 } );
	BOOST_TEST( xcpu.A[dst] == 0x5000 );
}

BOOST_DATA_TEST_CASE(d8pcxn_dm,
					 bdata::xrange(1, 8),
					 dst)
{
	test_write32(0x1032, 0x3000);
	xcpu.D[0] = 0x1000;
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150773 | dst << 9),
				0x0830 } );
	BOOST_TEST( xcpu.A[dst] == 0x5000 );
}

BOOST_DATA_TEST_CASE(imm_dm,
					 bdata::xrange(8),
					 dst)
{
	xcpu.A[dst] = 0x2000;
	test_cpu( { (uint16_t)(0150774 | dst << 9),
				0, 0x3000 } );
	BOOST_TEST( xcpu.A[dst] == 0x5000 );
}

BOOST_AUTO_TEST_SUITE_END();
