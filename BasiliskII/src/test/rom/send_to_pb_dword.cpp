#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "msc.hpp"
#include "via.hpp"
#include "via_test.hpp"
#include <memory>
using namespace ROM;
struct MockMSC : public MSC_REG {
	std::vector<uint8_t> executed_cmd;
	uint8_t cmd(uint8_t c) override {
		executed_cmd.push_back( c );
		return 0;
	}
};
static bool is_msc;
namespace ROM {
bool send_to_pb_byte(uint8_t data, bool* msc) {
	if( msc ) { *msc = is_msc; }
	return MOCK::invoke<bool>("send_to_pb_byte", data);
}
}
uint8_t UT_get_JawsVia2(const std::shared_ptr<IO_BASE>& via);
uint8_t UT_get_SR_MSC(const std::shared_ptr<VIA>& via);
void prepare() {
	fixture f( MB_TYPE::JAWS );
	rom_base = 0x40800000;
	motherboard = &motherboards[ 5 ];
	DEFINE_ROM( 478C2 );
	MOCK::reset_all();
	MOCK::make<bool(uint8_t)>("send_to_pb_byte");
}
BOOST_AUTO_TEST_CASE( not_ready ) {
	prepare();
	auto via2 = std::make_shared<VIA_TEST>();
	via2->write(VIA_REG::DDRB, 0xbd);
	via2->write(VIA_REG::RB, 0x66);
	via2->reset_written();
	via2->b[1].set_read_data({ false } );
	via2->b[6].set_read_data({ false } );
	machine->via2 = via2;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	TEST_ROM( 47840 );
	BOOST_TEST( ! cpu.Z );
}
struct common_fixture {
	common_fixture() {
		prepare();
		auto via2 = std::make_shared<VIA_TEST>();
		via2->write(VIA_REG::DDRB, 0xbd);
		via2->write(VIA_REG::RB, 0x66);
		via2->reset_written();
		via2->b[1].set_read_data({ true } );
		via2->b[6].set_read_data({ true } );
		machine->via2 = via2;
		AR(0) = 0x408032D4;
		AR(2) = 0x50f00000;
		AR(3) = 0x50f02000;
	}
};
BOOST_FIXTURE_TEST_CASE( failure_byte1, common_fixture ) {
	is_msc = false;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->always( false, 0xab );
	DR(3) = 0xab;
	TEST_ROM(47840);
	BOOST_CHECK( ! cpu.Z );
}

BOOST_FIXTURE_TEST_SUITE( test_msc, common_fixture);

BOOST_AUTO_TEST_CASE( failure_byte2 ) {
	is_msc = true;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x23 )
		.nth_is(2, false, 0x01 );
	DR(3) = 0x010123;
	TEST_ROM( 47840 );
	BOOST_CHECK( ! cpu.Z );
	MOCK::verify( "send_to_pb_byte", 2);
}

BOOST_AUTO_TEST_CASE( failure_byte3 ) {
	is_msc = true;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x3 )
		.nth_is(2, true, 0x4 )
		.nth_is(3, false, 0x5 );
	DR(3) = 0x05040203;
	DR(4) = 0x1122;
	TEST_ROM( 47840 );
	BOOST_CHECK( ! cpu.Z );
	MOCK::verify( "send_to_pb_byte", 3);
}

BOOST_AUTO_TEST_CASE( success_8yxx ) {
	is_msc = true;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x22 );
	DR(3) = 0x8022;
	TEST_ROM( 47840 );
	BOOST_CHECK( cpu.Z );
	MOCK::verify( "send_to_pb_byte", 1);
}

BOOST_AUTO_TEST_CASE( success_00xx ) {
	is_msc = true;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x33 );
	DR(3) = 0x33;
	TEST_ROM( 47840 );
	BOOST_CHECK( cpu.Z );
	MOCK::verify( "send_to_pb_byte", 1);
}

BOOST_AUTO_TEST_CASE( success_xx01xx ) {
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
BOOST_AUTO_TEST_CASE( success_xxxx02xx ) {
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
BOOST_FIXTURE_TEST_SUITE( test_non_msc, common_fixture );

BOOST_AUTO_TEST_CASE( failure_byte2 ) {
	is_msc = false;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x3 )
		.nth_is(2, false, 0x2 );
	DR(3) = 0x0203;
	TEST_ROM( 47840 );
	BOOST_TEST( ! cpu.Z );
	MOCK::verify( "send_to_pb_byte", 2);
}

BOOST_AUTO_TEST_CASE( failure_byte3 ) {
	is_msc = false;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x3 )
		.nth_is(2, true, 0x2 )
		.nth_is(3, false, 0x1 );
	DR(3) = 0x010203;
	DR(4) = 0x1122;
	TEST_ROM( 47840 );
	BOOST_TEST( ! cpu.Z );
	MOCK::verify( "send_to_pb_byte", 3);
}

BOOST_AUTO_TEST_CASE( failure_byte4 ) {
	is_msc = false;
	MOCK::get<bool(uint8_t)>("send_to_pb_byte")
		->nth_is(1, true, 0x55 )
		.nth_is(2, true, 0x44 )
		.nth_is(3, true, 0x33 )
		.nth_is(4, false, 0x22 );
	DR(4) = 0xffffffff;
	DR(3) = 0x22334455;
	TEST_ROM( 47840 );
	BOOST_TEST( ! cpu.Z );
	MOCK::verify( "send_to_pb_byte", 4);
}

BOOST_AUTO_TEST_CASE( success_00xx ) {
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

BOOST_AUTO_TEST_CASE( success_xx01xx ) {
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

BOOST_AUTO_TEST_CASE( success_xxxx02xx ) {
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
