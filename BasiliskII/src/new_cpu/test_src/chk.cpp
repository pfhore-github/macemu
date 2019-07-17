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
BOOST_FIXTURE_TEST_SUITE(chk_word, fixture);
BOOST_AUTO_TEST_CASE(pass) {
	xcpu.D[0] = 10;
	xcpu.D[1] = 3;
	xcpu.X = true;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0041600 } ) );
	BOOST_TEST( xcpu.X == true );
}
BOOST_AUTO_TEST_CASE(negative) {
	xcpu.D[0] = 10;
	xcpu.D[1] = -2;
	BOOST_CHECK_THROW( test_cpu( { 0041600 } ), ChkTrap );
	BOOST_TEST( xcpu.N == true );
}

BOOST_AUTO_TEST_CASE(over) {
	xcpu.D[0] = 10;
	xcpu.D[1] = 12;
	BOOST_CHECK_THROW( test_cpu( { 0041600 } ), ChkTrap );
	BOOST_TEST( xcpu.N == false );
}

BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(1, 8),
					 limit) {
	xcpu.D[limit] = 10;
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040600 | limit) } ));
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1000;
	test_write16( 0x1000, 10 );
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040620 | limit) } ));
}

BOOST_DATA_TEST_CASE(inc,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1000;
	test_write16( 0x1000, 10 );
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040630 | limit) } ));
	BOOST_TEST( xcpu.A[limit] == 0x1002 );
}

BOOST_DATA_TEST_CASE(dec,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1002;
	test_write16( 0x1000, 10 );
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040640 | limit) } ));
	BOOST_TEST( xcpu.A[limit] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1000;
	test_write16( 0x1010, 10 );
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040650 | limit), 0x10 } ));
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1000;
	test_write16( 0x1040, 10 );
	xcpu.D[0] = 0x10;
	xcpu.D[1] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0041660 | limit),
										0x0830 } ));
}

BOOST_AUTO_TEST_CASE(xxxw) {
	test_write16( 0x1000, 10 );
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0040670, 0x1000 } ) );
}

BOOST_AUTO_TEST_CASE(xxxl) {
	test_write16( 0x1000, 10 );
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0040671, 0, 0x1000 } ) );
}


BOOST_AUTO_TEST_CASE(d16pc) {
	test_write16( 0x12, 10 );
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0040672, 0x10 } ));
}

BOOST_AUTO_TEST_CASE(d8pcxn) {
	test_write16( 0x42, 10 );
	xcpu.D[0] = 0x10;
	xcpu.D[1] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0041673, 0x0830 } ));
}

BOOST_AUTO_TEST_CASE(imm) {
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0040674, 10 } ) );
}

BOOST_AUTO_TEST_SUITE_END();


BOOST_FIXTURE_TEST_SUITE(chk_long, fixture);
BOOST_AUTO_TEST_CASE(pass) {
	xcpu.D[0] = 100000;
	xcpu.D[1] = 3000;
	xcpu.X = true;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0041400 } ) );
	BOOST_TEST( xcpu.X == true );
}
BOOST_AUTO_TEST_CASE(negative) {
	xcpu.D[0] = 100000;
	xcpu.D[1] = -20000;
	BOOST_CHECK_THROW( test_cpu( { 0041400 } ), ChkTrap );
	BOOST_TEST( xcpu.N == true );
}

BOOST_AUTO_TEST_CASE(over) {
	xcpu.D[0] = 100000;
	xcpu.D[1] = 120000;
	BOOST_CHECK_THROW( test_cpu( { 0041400 } ), ChkTrap );
	BOOST_TEST( xcpu.N == false );
}

BOOST_DATA_TEST_CASE(dn,
					 bdata::xrange(1, 8),
					 limit) {
	xcpu.D[limit] = 1000000;
	xcpu.D[0] = 400000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040400 | limit) } ));
}

BOOST_DATA_TEST_CASE(mem,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1000;
	test_write32( 0x1000, 100000 );
	xcpu.D[0] = 40000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040420 | limit) } ));
}

BOOST_DATA_TEST_CASE(inc,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1000;
	test_write32( 0x1000, 100000 );
	xcpu.D[0] = 40000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040430 | limit) } ));
	BOOST_TEST( xcpu.A[limit] == 0x1004 );
}

BOOST_DATA_TEST_CASE(dec,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1004;
	test_write32( 0x1000, 100000 );
	xcpu.D[0] = 40000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040440 | limit) } ));
	BOOST_TEST( xcpu.A[limit] == 0x1000 );
}

BOOST_DATA_TEST_CASE(d16an,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1000;
	test_write32( 0x1010, 1000000 );
	xcpu.D[0] = 400000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0040450 | limit), 0x10 } ));
}

BOOST_DATA_TEST_CASE(d8anxn,
					 bdata::xrange(8),
					 limit) {
	xcpu.A[limit] = 0x1000;
	test_write32( 0x1040, 1000000 );
	xcpu.D[0] = 0x10;
	xcpu.D[1] = 40000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { (uint16_t)(0041460 | limit),
										0x0830 } ));
}

BOOST_AUTO_TEST_CASE(xxxw) {
	test_write32( 0x1000, 1000000 );
	xcpu.D[0] = 400000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0040470, 0x1000 } ) );
}

BOOST_AUTO_TEST_CASE(xxxl) {
	test_write32( 0x1000, 1000000 );
	xcpu.D[0] = 4;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0040471, 0, 0x1000 } ) );
}


BOOST_AUTO_TEST_CASE(d16pc) {
	test_write32( 0x12, 1000000 );
	xcpu.D[0] = 400000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0040472, 0x10 } ));
}

BOOST_AUTO_TEST_CASE(d8pcxn) {
	test_write32( 0x42, 1000000 );
	xcpu.D[0] = 0x10;
	xcpu.D[1] = 400000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0041473, 0x0830 } ));
}

BOOST_AUTO_TEST_CASE(imm) {
	xcpu.D[0] = 40000;
	BOOST_REQUIRE_NO_THROW( test_cpu( { 0040474, 12, 0 } ) );
}

BOOST_AUTO_TEST_SUITE_END();
