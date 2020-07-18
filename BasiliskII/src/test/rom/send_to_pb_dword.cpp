#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "msc.hpp"
#include "jaws.hpp"
#include "via.hpp"
#include "via_test.hpp"
#include "msc_test.hpp"
#include <memory>
using namespace ROM;
static bool is_msc;
namespace ROM {
bool send_to_pb_byte(uint8_t data, bool* msc) {
	if( msc ) { *msc = is_msc; }
	return MOCK::invoke<bool>("send_to_pb_byte", data);
}
}
void prepare() {
	fixture f( std::make_unique<JAWS>() );
	rom_base = 0x40800000;
	DR(2) = 0;
	DEFINE_ROM( 478C2 );
	ROMWrapper::run_02F64();
	MOCK::reset_all();
	MOCK::make<bool(uint8_t)>("send_to_pb_byte");
}

BOOST_AUTO_TEST_SUITE( test_msc);

BOOST_AUTO_TEST_CASE( byte_1 ) {
	prepare();
	is_msc = true;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x22 );
	DR(3) = 0x8022;
	TEST_ROM( 47840 );
	BOOST_CHECK( cpu.Z );
	MOCK::verify( "send_to_pb_byte", 1);
}

BOOST_AUTO_TEST_CASE( byte_2 ) {
	prepare();
	is_msc = true;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x33 );
	DR(3) = 0x33;
	TEST_ROM( 47840 );
	BOOST_CHECK( cpu.Z );
	MOCK::verify( "send_to_pb_byte", 1);
}

BOOST_AUTO_TEST_CASE( word ) {
	prepare();
	is_msc = true;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x2 )
		.nth_is(2, true, 0x3 );
	DR(4) = 0xffffffff;
	DR(3) = 0x22030102;
	TEST_ROM( 47840 );
	BOOST_CHECK( cpu.Z );
	MOCK::verify( "send_to_pb_byte", 2);
}
BOOST_AUTO_TEST_CASE( tword ) {
	prepare();
	is_msc = true;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x4 )
		.nth_is(2, true, 0x5 )
		.nth_is(3, true, 0x6 );
	DR(4) = 0xffffffff;
	DR(3) = 0x06050204;
	TEST_ROM( 47840 );
	BOOST_CHECK( cpu.Z );
	MOCK::verify( "send_to_pb_byte", 3);
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE( test_non_msc );


BOOST_AUTO_TEST_CASE( byte ) {
	prepare();
	is_msc = false;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 1 )
		.nth_is(2, true, 0 );
	DR(4) = 0xffffffff;
	DR(3) = 0x01;
	TEST_ROM( 47840 );
	BOOST_TEST( cpu.Z );
	MOCK::verify( "send_to_pb_byte", 2);
}

BOOST_AUTO_TEST_CASE( word ) {
	prepare();
	is_msc = false;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 2 )
		.nth_is(2, true, 1 )
		.nth_is(3, true, 3 );
	DR(4) = 0xffffffff;
	DR(3) = 0x22030102;
	TEST_ROM( 47840 );
	BOOST_TEST( cpu.Z );
	MOCK::verify( "send_to_pb_byte", 3);
}

BOOST_AUTO_TEST_CASE( tword ) {
	prepare();
	is_msc = false;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 4 )
		.nth_is(2, true, 2 )
		.nth_is(3, true, 5 )
		.nth_is(4, true, 6 );
	DR(4) = 0xffffffff;
	DR(3) = 0x06050204;
	TEST_ROM( 47840 );
	BOOST_TEST( cpu.Z );
	MOCK::verify( "send_to_pb_byte", 4);
}
BOOST_AUTO_TEST_SUITE_END();
