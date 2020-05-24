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
	AR(0) = 0;
	AR(1) = 0x2000;
	write_l( 0x1000, 0x200 );
	AR(5) = 0x1000;
	DR(1) = 0x12345678;
	DR(6) = 0x55555550;
	DR(0) = 0x4000;
	DR(5) = 0x55555550;
	write_l( 0x1FFC, 0xdeadbeaf );
	TEST_ROM( A0242 );
	BOOST_CHECK_EQUAL( DR(0), 0x4000 + 0x2000 );
	BOOST_CHECK_EQUAL( AR(0), 0x2000 );
	BOOST_CHECK_EQUAL( DR(5), 0x55555555 );
	BOOST_CHECK_EQUAL( read_l( 0x1FFC), 0xdeadbeaf );
}

BOOST_AUTO_TEST_CASE( ng1 )  {
	prepare(0x2000);
	AR(0) = 0;
	AR(1) = 0x3000;
	write_l( 0x1000, 0x800 );
	AR(5) = 0x1000;
	DR(1) = 0x12345678;
	DR(6) = 0x55555550;
	DR(0) = 0x4000;
	DR(5) = 0x55555550;
	write_l( 0x1FFC, 0xdeadbeaf );
	TEST_ROM( A0242 );
	BOOST_CHECK_EQUAL( DR(0), 0x4000 + 0x2000 );
	BOOST_CHECK_EQUAL( AR(0), 0x2000 );
	BOOST_CHECK_EQUAL( DR(5), 0x55555553 );
	BOOST_CHECK_EQUAL( read_l( 0x1FFC), 0xdeadbeaf );
}

BOOST_AUTO_TEST_CASE( ng2 )  {
	prepare(0x2000);
	AR(0) = 0x3000;
	AR(1) = 0x3000;
	write_l( 0x1000, 0x800 );
	AR(5) = 0x1000;
	DR(1) = 0x12345678;
	DR(6) = 0x55555550;
	DR(0) = 0x4000;
	DR(5) = 0x55555550;
	write_l( 0x1FFC, 0xdeadbeaf );
	TEST_ROM( A0242 );
	BOOST_CHECK_EQUAL( DR(0), 0x4000 );
	BOOST_CHECK_EQUAL( AR(0), 0x3000 );
	BOOST_CHECK_EQUAL( DR(5), 0x55555550 );
	BOOST_CHECK_EQUAL( read_l( 0x1FFC), 0xdeadbeaf );
}


