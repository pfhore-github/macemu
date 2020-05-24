#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "scc_impl.hpp"
using namespace ROM;
static std::string values;
namespace ROM {
void write_modem_port_byte(uint8_t v) {
	values += v;
}
uint8_t to_ascii(uint8_t d0) {
	if( d0 >= 10 ) {
		return d0 + 0x37;
	} else {
		return d0 + 0x30;
	}
}

}
void prepare() {
	fixture f;
	rom_base = 0x40800000;
	AR(3) = 0x50F04000;
	DR(3) = 0;
	AR(6) = 0;
	DEFINE_ROM( B9FDA );
	DEFINE_ROM( BA174 );
	values.clear();
}
BOOST_AUTO_TEST_CASE( empty ) {
	prepare();
	DR(0) = 0;
	DR(2) = 0;
	DR(7) = 0;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values.empty() );
}

BOOST_AUTO_TEST_CASE( crlf ) {
	prepare();
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_CRLF + 16);
	DR(0) = 0;
	DR(2) = 0;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values == "\r\n" );
}

BOOST_AUTO_TEST_CASE( half1 ) {
	prepare();
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_ASCII + 16);
	DR(0) = 0x3456789A;
	DR(2) = 1;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values == "9A" );
}

BOOST_AUTO_TEST_CASE( half2 ) {
	prepare();
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_ASCII + 16);
	DR(0) = 0x456789AB;
	DR(2) = 2;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values == "89AB" );
}

BOOST_AUTO_TEST_CASE( half3 ) {
	prepare();
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_ASCII + 16);
	DR(0) = 0x456789AB;
	DR(2) = 3;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values == "6789AB" );
}

BOOST_AUTO_TEST_CASE( half4 ) {
	prepare();
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_ASCII + 16);
	DR(0) = 0x456789AB;
	DR(2) = 4;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values == "456789AB" );
}


BOOST_AUTO_TEST_CASE( byte1 ) {
	prepare();
	DR(0) = 0x3456789a;
	DR(2) = 1;
	DR(7) = 0;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values == "\x9a" );
}

BOOST_AUTO_TEST_CASE( byte2 ) {
	prepare();
	DR(7) = 0;
	DR(0) = 0x456789AB;
	DR(2) = 2;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values == "\x89\xAB" );
}

BOOST_AUTO_TEST_CASE( byte3 ) {
	prepare();
	DR(7) = 0;
	DR(0) = 0x456789AB;
	DR(0) = 0x456789AB;
	DR(2) = 3;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values == "\x67\x89\xAB" );
}

BOOST_AUTO_TEST_CASE( byte4 ) {
	prepare();
	DR(7) = 0;
	DR(0) = 0x456789AB;
	DR(2) = 4;
	if( rom ) {
		test_rom( 0x408BA182 );
	} else {
		ROMWrapper::run_BA182();
	}
	BOOST_TEST( values == "\x45\x67\x89\xAB" );
}

