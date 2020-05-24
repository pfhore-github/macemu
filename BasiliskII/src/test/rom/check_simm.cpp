#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "mem.hpp"
extern uint32_t RAMSize;
using namespace ROM;
void prepare(uint32_t size) {
	fixture f;
	RAMSize = size;
	set_sr( 0x2700 );
	MOCK::reset_all();
}
// 4B0C
BOOST_AUTO_TEST_CASE( ok )  {
	prepare(0x2000);
	AR(0) = 0x1000;
	DR(1) = 0x12345678;
	DR(6) = 0x55555550;
	write_l( 0x1000, 0xdeadbeaf );
	write_l( 0x1004, 0x98765432 );
	TEST_ROM( A0286 );
	BOOST_CHECK_EQUAL( DR(6), 0x55555550 );
	BOOST_CHECK_EQUAL( read_l( 0x1000), 0xdeadbeaf );
	BOOST_CHECK_EQUAL( read_l( 0x1004), 0x98765432 );
}

BOOST_AUTO_TEST_CASE( ng1 )  {
	prepare(0x2000);
	AR(0) = 0x1FFD;
	DR(1) = 0x12345678;
	DR(6) = 0x55555550;
	write_l( 0x1FFC, 0xdeadbeaf );
	TEST_ROM( A0286 );
	BOOST_CHECK_EQUAL( DR(6), 0x55555558 );
	BOOST_CHECK_EQUAL( read_l( 0x1FFC), 0xdeadbeaf );
}

BOOST_AUTO_TEST_CASE( ng2 )  {
	prepare(0x2000);
	AR(0) = 0x1FFE;
	DR(1) = 0x12345678;
	DR(6) = 0x55555550;
	write_l( 0x1FFC, 0xdeadbeaf );
	TEST_ROM( A0286 );
	BOOST_CHECK_EQUAL( DR(6), 0x5555555C );
	BOOST_CHECK_EQUAL( read_l( 0x1FFC), 0xdeadbeaf );
}

BOOST_AUTO_TEST_CASE( ng3 )  {
	prepare(0x2000);
	AR(0) = 0x1FFF;
	DR(1) = 0x12345678;
	DR(6) = 0x55555550;
	write_l( 0x1FFC, 0xdeadbeaf );
	TEST_ROM( A0286 );
	BOOST_CHECK_EQUAL( DR(6), 0x5555555E );
	BOOST_CHECK_EQUAL( read_l( 0x1FFC), 0xdeadbeaf );
}

BOOST_AUTO_TEST_CASE( ng4 )  {
	prepare(0x2000);
	AR(0) = 0x2000;
	DR(1) = 0x12345678;
	DR(6) = 0x55555550;
	TEST_ROM( A0286 );
	BOOST_CHECK_EQUAL( DR(6), 0x5555555F );
}


