#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "scc_impl.hpp"
using namespace ROM;
static std::string written;
namespace ROM {
void write_modem_port_byte(uint8_t c) {
	written += c;
}
}
void prepare() {
	fixture f;
	rom_base = 0x40800000;
	DEFINE_ROM( B9FDA );
	written.clear();
}
BOOST_AUTO_TEST_CASE( exec) {
	prepare();
	AR(3) = 0x50F04000;
	DR(3) = 0;
	AR(1) = 0x408BA010;
	if( rom ) {
		test_rom( 0x408B9FF6 );
	} else {
		ROMWrapper::run_B9FF6();
	}
	BOOST_TEST( written == "*APPLE*" );
	BOOST_TEST( AR(1) == 0x408BA018 );
}

