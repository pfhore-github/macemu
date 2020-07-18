#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "msc.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "glu.hpp"
#include "via_test.hpp"
#include "msc_test.hpp"
#include <memory>
using namespace ROM;
namespace ROM {
bool send_to_pb_dword(uint32_t v) { return MOCK::invoke<bool>("send_to_pb_dword", v); }
std::optional<uint8_t> recv_from_pb(bool* v) { return MOCK::invoke<std::optional<uint8_t>>("recv_from_pb", v); }
uint16_t _B375E(uint16_t d1);
}
void prepare() {
	fixture f(std::make_unique<MSC>(MSC::MODEL::PB_Duo270c));
	clear_global();
	MOCK::reset_all();
	set_sr( 0x2700 );
	DR(2) = 0;
	ROMWrapper::run_04A5E();
	DEFINE_ROM( B29C4 );
	DEFINE_ROM( 02F64 );
	DEFINE_ROM( 47840 );
	DEFINE_ROM( 47928 );
}
void run() {
	if( rom ) {
		DR(1) = 0x10;
		test_rom( 0x408B375E );
	} else {
		DR(1) = ROM::_B375E( 0x10 );
	}
}
BOOST_AUTO_TEST_CASE( bad1 ) {
	prepare();
	MOCK::make<bool(uint32_t)>("send_to_pb_dword")->
		nth_is(1, false, 0x110023a);
	run();
	BOOST_CHECK_EQUAL( DR(1), 0 );
}

BOOST_AUTO_TEST_CASE( once ) {
	prepare();
	MOCK::make<bool(uint32_t)>("send_to_pb_dword")->
		nth_is(1, true, 0x110023a);
	MOCK::make<std::optional<uint8_t>(bool*)>("recv_from_pb")
		->nth_do(1, [](bool*) ->std::optional<uint8_t> { return {}; });
	run();
	BOOST_CHECK_EQUAL( DR(1), 0 );
 	MOCK::get<std::optional<uint8_t>(bool*)>("recv_from_pb")->verify(1);
}

BOOST_AUTO_TEST_CASE( twice ) {
	prepare();
	MOCK::make<bool(uint32_t)>("send_to_pb_dword")->
		nth_is(1, true, 0x110023a);
	MOCK::make<std::optional<uint8_t>(bool*)>("recv_from_pb")
		->nth_do(1, [](bool*) ->std::optional<uint8_t> { return 1; })
		.nth_do(2, [](bool*) ->std::optional<uint8_t> { return 2; });
	run();
	BOOST_CHECK_EQUAL( DR(1), 2 );
 	MOCK::get<std::optional<uint8_t>(bool*)>("recv_from_pb")->verify(2);
}

BOOST_AUTO_TEST_CASE( twice_2 ) {
	prepare();
	MOCK::make<bool(uint32_t)>("send_to_pb_dword")->
		nth_is(1, true, 0x110023a);
	MOCK::make<std::optional<uint8_t>(bool*)>("recv_from_pb")
		->nth_do(1, [](bool*) ->std::optional<uint8_t> { return 0x3a; })
		.nth_do(2, [](bool*) ->std::optional<uint8_t> { return {}; });
	run();
	BOOST_CHECK_EQUAL( DR(1), 0 );
 	MOCK::get<std::optional<uint8_t>(bool*)>("recv_from_pb")->verify(2);
}

BOOST_AUTO_TEST_CASE( thrice ) {
	prepare();
	MOCK::make<bool(uint32_t)>("send_to_pb_dword")->
		nth_is(1, true, 0x110023a);
	MOCK::make<std::optional<uint8_t>(bool*)>("recv_from_pb")
		->nth_do(1, [](bool*) ->std::optional<uint8_t> { return 0x3a; })
		.nth_do(2, [](bool*) ->std::optional<uint8_t> { return 0x12; })
		.nth_do(3, [](bool*) ->std::optional<uint8_t> { return 0x11; });
	run();
	BOOST_CHECK_EQUAL( DR(1), 0x11 );
 	MOCK::get<std::optional<uint8_t>(bool*)>("recv_from_pb")->verify(3);
}

