#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "msc.hpp"
#include "via.hpp"
#include "via_test.hpp"
#include <memory>
using namespace ROM;
static std::function<bool(uint32_t)> send_dword_mock;
static std::function<bool(uint8_t)> send_byte_mock;
static std::function<uint8_t()> recv_mock;
static std::function<void()> mock_getHwInfo;
static bool is_msc;
static bool fail;
namespace ROM {
void sad_mac( uint32_t) { fail = true; }
void getHWInfo(uint8_t) { mock_getHwInfo(); }
bool send_to_pb_dword(uint32_t v) {
	return send_dword_mock(v);
}
std::optional<uint8_t> recv_from_pb(bool* msc) {
	if( msc ) {
		*msc = is_msc;
	}
	return recv_mock();
}
bool send_to_pb_byte(uint8_t v, bool* msc) {
	if( msc ) {
		*msc = is_msc;
	}
	return send_byte_mock(v);
}
}
void prepare() {
	fixture f;
	rom_base = 0x40800000;
	fail = false;
	DEFINE_ROM( 02F64 );
	DEFINE_ROM( 47840 );
	DEFINE_ROM( 478C2 );
	DEFINE_ROM( 47928 );
	DEFINE_ROM( 4AA98 );

}
BOOST_AUTO_TEST_CASE( non_pb ) {
	prepare();
	mock_getHwInfo =
		[]() {
			GEN = 0x11;
			GID = 0x11;
			motherboard = &motherboards[0];
			model = &old_machines[0];
		};
	if( rom ) {		
		test_rom( 0x408B29C4 );
	} else {
		ROMWrapper::run_B29C4();
	}
}

BOOST_AUTO_TEST_CASE( fail1 ) {
	prepare();
	mock_getHwInfo =
		[]() {
			GEN = 0xA;
			GID = 0;
			motherboard = &motherboards[0];
			model = &old_machines[0];
		};
	send_dword_mock = [](uint32_t v) { return v != 0xE2; };
	if( rom ) {
		test_rom( 0x408B29C4 );
	} else {
		ROMWrapper::run_B29C4();
	}
	BOOST_TEST( fail );
}


BOOST_AUTO_TEST_CASE( success1 ) {
	prepare();
	mock_getHwInfo =
		[]() {
			GEN = 0xA;
			GID = 0;
			motherboard = &motherboards[0];
			model = &old_machines[0];
		};
	send_dword_mock = [](uint32_t v) { return v != 0xE1; };
	int i = 0;
	recv_mock = [&i]() { return i++ == 0 ? 0x62 : 0xff; };
	if( rom ) {
		test_rom( 0x408B29C4 );
	} else {
		ROMWrapper::run_B29C4();
	}
	BOOST_TEST( !fail );
}

BOOST_AUTO_TEST_CASE( fail3 ) {
	prepare();
	mock_getHwInfo =
		[]() {
			GEN = 0xA;
			GID = 0;
			motherboard = &motherboards[0];
			model = &old_machines[0];
		};
	int i1 = 0, i2 = 0, i3 = 0;
	send_dword_mock = [&i1](uint32_t v) { return ++i1 != 3; };
	send_byte_mock = [&i2](uint8_t) { return ++i2 != 11; };
	recv_mock = [&i3]() { return i3++ == 1 ? 0x62 : 0xff; };
	if( rom ) {
		test_rom( 0x408B29C4 );
	} else {
		ROMWrapper::run_B29C4();
	}
	BOOST_TEST( i2 == 11 );
	BOOST_TEST( fail );
}

BOOST_AUTO_TEST_CASE( fail4 ) {
	prepare();
	mock_getHwInfo =
		[]() {
			GEN = 0xA;
			GID = 0;
			motherboard = &motherboards[0];
			model = &old_machines[0];
		};
	int i1 = 0, i3 = 0;
	send_dword_mock = [&i1](uint32_t v) { return i1++ < 2; };
	send_byte_mock = [](uint8_t) { return true; };
	recv_mock = [&i3]() { return i3++ == 1 ? 0x62 : 0xff; };
	if( rom ) {
		test_rom( 0x408B29C4 );
	} else {
		ROMWrapper::run_B29C4();
	}
	BOOST_TEST( fail );
}

BOOST_AUTO_TEST_CASE( fail5 ) {
	prepare();
	mock_getHwInfo =
		[]() {
			GEN = 0xA;
			GID = 0;
			motherboard = &motherboards[0];
			model = &old_machines[0];
		};
	int i3 = 0;
	send_dword_mock = [](uint32_t v) { return true; };
	send_byte_mock = [](uint8_t) { return true; };
	recv_mock = [&i3]() { return 0xff; };
	if( rom ) {
		test_rom( 0x408B29C4 );
	} else {
		ROMWrapper::run_B29C4();
	}
	BOOST_TEST( fail );
}

BOOST_AUTO_TEST_CASE( success2 ) {
	prepare();
	mock_getHwInfo =
		[]() {
			GEN = 0xA;
			GID = 0;
			motherboard = &motherboards[0];
			model = &old_machines[0];
		};
	int i3 = 0;
	send_dword_mock = [](uint32_t v) { return true; };
	send_byte_mock = [](uint8_t) { return true; };
	recv_mock = [&i3]() { return i3++ == 1 ? 0x62 : 0xff; };
	if( rom ) {
		test_rom( 0x408B29C4 );
	} else {
		ROMWrapper::run_B29C4();
	}
	BOOST_TEST( ! fail );
}


