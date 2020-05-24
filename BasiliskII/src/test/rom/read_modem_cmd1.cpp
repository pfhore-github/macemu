#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "scc_impl.hpp"
using namespace ROM;
static std::string values;
static std::string to_read;
namespace ROM {
std::pair<int8_t, uint8_t> read_modem_port() {
	if( to_read.empty() ) {
		return std::pair(0x80, 0);
	}		
	char n = to_read[0];
	to_read = to_read.erase(0, 1);
	if( n >= 0 ) {
		return std::pair(0, n);
	} else {
		return std::pair(n, 0);
	}		
}
void write_modem_port_byte(uint8_t v) {
	values += v;
}
int8_t from_ascii(int8_t v) {
	v -= 0x30;
	if( v < 0 || v > 0x16 ) {
		return 0;
	}
	if( v > 9 ) {
		return v-7;
	}
	return v;	
}

}
void prepare(const std::string& r) {
	fixture f;
	rom_base = 0x40800000;
	AR(3) = 0x50F04000;
	DR(3) = 0;
	AR(6) = 0;
	AR(7) = 0x1000;
	DEFINE_ROM( B9FDA );
	DEFINE_ROM( BA0EA );
	DEFINE_ROM( BA11A );
	INIT_HW_FLG = 0;
	ROM_FLG = 0;
	HWCFG_FLAGS = 0;
	GID = 0;
	GEN = 0;
	values.clear();
	to_read = r;
}
BOOST_AUTO_TEST_CASE( byte ) {
	prepare("abcdef");
	DR(7) = 0;
	push32( 0x1000 );
	push32( 6 );
	push32( 0 );
	if( rom ) {
		test_rom( 0x408BA964 );
	} else {
		ROMWrapper::run_BA964();
	}
	BOOST_TEST( memcmp(&RAMBaseHost[0x1000], "abcdef", 6) == 0 );
}

BOOST_AUTO_TEST_CASE( echo ) {
	prepare("abcdef");
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_ECHO + 16);
	push32( 0x1000 );
	push32( 6 );
	push32( 0 );
	if( rom ) {
		test_rom( 0x408BA964 );
	} else {
		ROMWrapper::run_BA964();
	}
	BOOST_TEST( memcmp(&RAMBaseHost[0x1000], "abcdef", 6) == 0 );
	BOOST_TEST( values == "abcdef" );
}

BOOST_AUTO_TEST_CASE( half ) {
	prepare("ABCDEF");
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_ASCII + 16 );
	push32( 0x1000 );
	push32( 3 );
	push32( 0 );
	if( rom ) {
		test_rom( 0x408BA964 );
	} else {
		ROMWrapper::run_BA964();
	}
	BOOST_TEST( memcmp(&RAMBaseHost[0x1000], "\xab\xcd\xef", 3) == 0 );
}
