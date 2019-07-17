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
BOOST_FIXTURE_TEST_SUITE(bcc, fixture);
BOOST_AUTO_TEST_CASE( d8_for )  {
	xcpu.Z = true;
	test_cpu( { (uint16_t)(0x6700 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE( d8_back ) {
	xcpu.Z = true;
	test_cpu( { (uint16_t)(0x6700 | (uint8_t)(-20) ) } );
	BOOST_TEST( xcpu.PC == -18 );
}

BOOST_AUTO_TEST_CASE( d16_for )  {
	xcpu.Z = true;
	test_cpu( { 0x6700,  2000 } );
	BOOST_TEST( xcpu.PC == 2002 );
}

BOOST_AUTO_TEST_CASE( d16_back ) {
	xcpu.Z = true;
	test_cpu( { 0x6700, (uint16_t)-2000 } );
	BOOST_TEST( xcpu.PC == -1998 );
}

BOOST_AUTO_TEST_CASE( d32 )  {
	xcpu.Z = true;
	test_cpu( { 0x67ff,  0x5, 0 } );
	BOOST_TEST( xcpu.PC == (0x50000 + 2) );
}

BOOST_AUTO_TEST_CASE(hi_t) {
	xcpu.C = false;
	xcpu.Z = false;
	test_cpu( { (uint16_t)(0x6200 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(hi_f1) {
	xcpu.C = true;
	test_cpu( { (uint16_t)(0x6200 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE(hi_f2) {
	xcpu.Z = true;
	test_cpu( { (uint16_t)(0x6200 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE(ls_t1) {
	xcpu.C = true;
	test_cpu( { (uint16_t)(0x6300 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(ls_t2) {
	xcpu.Z = true;
	test_cpu( { (uint16_t)(0x6300 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(ls_f) {
	xcpu.C = false;
	xcpu.Z = false;
	test_cpu( { (uint16_t)(0x6300 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}


BOOST_DATA_TEST_CASE(cc,
					 bdata::xrange(2),
					 f
	)
{
	xcpu.C = ! f;
	test_cpu( { (uint16_t)(0x6400 | 20) } );
	BOOST_TEST( xcpu.PC == ( f ? 22 : 2 ) );
}

BOOST_DATA_TEST_CASE(cs,
					 bdata::xrange(2),
					 f
	)
{
	xcpu.C = f;
	test_cpu( { (uint16_t)(0x6500 | 20) } );
	BOOST_TEST( xcpu.PC == ( f ? 22 : 2 ) );
}

BOOST_DATA_TEST_CASE(ne,
					 bdata::xrange(2),
					 f
	)
{
	xcpu.Z = ! f;
	test_cpu( { (uint16_t)(0x6600 | 20) } );
	BOOST_TEST( xcpu.PC == ( f ? 22 : 2 ) );
}

BOOST_DATA_TEST_CASE(eq,
					 bdata::xrange(2),
					 f
	)
{
	xcpu.Z = f;
	test_cpu( { (uint16_t)(0x6700 | 20) } );
	BOOST_TEST( xcpu.PC == ( f ? 22 : 2 ) );
}

BOOST_DATA_TEST_CASE(vc,
					 bdata::xrange(2),
					 f
	)
{
	xcpu.V = ! f;
	test_cpu( { (uint16_t)(0x6800 | 20) } );
	BOOST_TEST( xcpu.PC == ( f ? 22 : 2 ) );
}

BOOST_DATA_TEST_CASE(vs,
					 bdata::xrange(2),
					 f
	)
{
	xcpu.V = f;
	test_cpu( { (uint16_t)(0x6900 | 20) } );
	BOOST_TEST( xcpu.PC == ( f ? 22 : 2 ) );
}

BOOST_DATA_TEST_CASE(pl,
					 bdata::xrange(2),
					 f
	)
{
	xcpu.N = ! f;
	test_cpu( { (uint16_t)(0x6a00 | 20) } );
	BOOST_TEST( xcpu.PC == ( f ? 22 : 2 ) );
}

BOOST_DATA_TEST_CASE(mi,
					 bdata::xrange(2),
					 f
	)
{
	xcpu.N = f;
	test_cpu( { (uint16_t)(0x6b00 | 20) } );
	BOOST_TEST( xcpu.PC == ( f ? 22 : 2 ) );
}


BOOST_AUTO_TEST_CASE(ge_t1) {
	xcpu.N = true;
	xcpu.V = true;
	test_cpu( { (uint16_t)(0x6c00 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(ge_t2) {
	xcpu.N = false;
	xcpu.V = false;
	test_cpu( { (uint16_t)(0x6c00 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(ge_f1) {
	xcpu.N = true;
	xcpu.V = false;
	test_cpu( { (uint16_t)(0x6c00 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE(ge_f2) {
	xcpu.N = false;
	xcpu.V = true;
	test_cpu( { (uint16_t)(0x6c00 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE(lt_t1) {
	xcpu.N = true;
	xcpu.V = false;
	test_cpu( { (uint16_t)(0x6d00 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(lt_t2) {
	xcpu.N = false;
	xcpu.V = true;
	test_cpu( { (uint16_t)(0x6d00 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(lt_f1) {
	xcpu.N = true;
	xcpu.V = true;
	test_cpu( { (uint16_t)(0x6d00 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE(lt_f2) {
	xcpu.N = false;
	xcpu.V = false;
	test_cpu( { (uint16_t)(0x6d00 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE(gt_t1) {
	xcpu.N = true;
	xcpu.V = true;
	xcpu.Z = false;
	test_cpu( { (uint16_t)(0x6e00 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(gt_t2) {
	xcpu.N = false;
	xcpu.V = false;
	xcpu.Z = false;
	test_cpu( { (uint16_t)(0x6e00 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(gt_f1) {
	xcpu.Z = true;
	test_cpu( { (uint16_t)(0x6e00 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}


BOOST_AUTO_TEST_CASE(gt_f2) {
	xcpu.Z = false;
	xcpu.N = false;
	xcpu.V = true;
	test_cpu( { (uint16_t)(0x6e00 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE(gt_f3) {
	xcpu.Z = false;
	xcpu.N = true;
	xcpu.V = false;
	test_cpu( { (uint16_t)(0x6e00 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE(le_t1) {
	xcpu.Z = true;
	test_cpu( { (uint16_t)(0x6f00 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(le_t2) {
	xcpu.Z = false;
	xcpu.N = true;
	xcpu.V = false;
	test_cpu( { (uint16_t)(0x6f00 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(le_t3) {
	xcpu.Z = false;
	xcpu.N = false;
	xcpu.V = true;
	test_cpu( { (uint16_t)(0x6f00 | 20) } );
	BOOST_TEST( xcpu.PC == 22 );
}

BOOST_AUTO_TEST_CASE(le_f1) {
	xcpu.Z = false;
	xcpu.N = true;
	xcpu.V = true;
	test_cpu( { (uint16_t)(0x6f00 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}

BOOST_AUTO_TEST_CASE(le_f2) {
	xcpu.Z = false;
	xcpu.N = false;
	xcpu.V = false;
	test_cpu( { (uint16_t)(0x6f00 | 20) } );
	BOOST_TEST( xcpu.PC == 2 );
}


BOOST_AUTO_TEST_SUITE_END();
