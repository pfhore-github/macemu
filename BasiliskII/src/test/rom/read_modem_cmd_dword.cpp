#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "scc_impl.hpp"
using namespace ROM;
static std::unique_ptr<std::ostringstream> values;
static std::unique_ptr<std::istringstream> to_read;
namespace ROM {
std::pair<int8_t, uint8_t> read_modem_port() {
	int n = to_read->get();
	if( n < 0) {
		return std::pair(0x80, 0);
	}		
	if( n < 0x80 ) {
		return std::pair(0, n);
	} else {
		return std::pair(n, 0);
	}		
}
void write_modem_port_byte(uint8_t v) {
	values->put( v );
}

}
void prepare(const std::string& r) {
	fixture f;
	rom_base = 0x40800000;
	AR(3) = 0x50F04000;
	DR(3) = 0;
	DEFINE_ROM( BA0EA );
	values = std::make_unique<std::ostringstream>();
	to_read = std::make_unique<std::istringstream>(r);
}
BOOST_AUTO_TEST_CASE( byte0 ) {
	prepare("12345678");
	DR(7) = 0;
	DR(2) = 0;
	TEST_ROM( BA138 );
	BOOST_CHECK_EQUAL( DR(1), 0 );
}


BOOST_AUTO_TEST_CASE( byte1 ) {
	prepare("12345678");
	DR(7) = 0;
	DR(2) = 1;
	TEST_ROM( BA138 );
	BOOST_TEST( DR(1) == 0x31 );
}

BOOST_AUTO_TEST_CASE( byte2 ) {
	prepare("12345678");
	DR(7) = 0;
	DR(2) = 2;
	TEST_ROM( BA138 );
	BOOST_TEST( DR(1) == 0x3132 );
}

BOOST_AUTO_TEST_CASE( byte3 ) {
	prepare("12345678");
	DR(7) = 0;
	DR(2) = 3;
	TEST_ROM( BA138 );
	BOOST_TEST( DR(1) == 0x313233 );
}

BOOST_AUTO_TEST_CASE( byte4 ) {
	prepare("12345678");
	DR(7) = 0;
	DR(2) = 4;
	TEST_ROM( BA138 );
	BOOST_TEST( DR(1) == 0x31323334 );
}

BOOST_AUTO_TEST_CASE( half1 ) {
	prepare("12345678");
	DR(7) = 1 << (INIT_FLAG_T::MODEM_PORT_ASCII+16);
	DR(2) = 1;
	TEST_ROM( BA138 );
	BOOST_TEST( DR(1) == 0x12 );
}

BOOST_AUTO_TEST_CASE( half2 ) {
	prepare("12345678");
	DR(7) = 1 << (INIT_FLAG_T::MODEM_PORT_ASCII+16);
	DR(2) = 2;
	TEST_ROM( BA138 );
	BOOST_TEST( DR(1) == 0x1234 );
}

BOOST_AUTO_TEST_CASE( half3 ) {
	prepare("12345678");
	DR(7) = 1 << (INIT_FLAG_T::MODEM_PORT_ASCII+16);
	DR(2) = 3;
	TEST_ROM( BA138 );
	BOOST_TEST( DR(1) == 0x123456 );
}

BOOST_AUTO_TEST_CASE( half4 ) {
	prepare("12345678");
	DR(7) = 1 << (INIT_FLAG_T::MODEM_PORT_ASCII+16);
	DR(2) = 4;
	TEST_ROM( BA138 );
	BOOST_TEST( DR(1) == 0x12345678 );
}

