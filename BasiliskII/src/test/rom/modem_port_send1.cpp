#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "scc_impl.hpp"
#include "wrapper.hpp"
#include "via1.hpp"
using namespace ROM;
std::unique_ptr<std::ostringstream> written;
namespace ROM {
void write_modem_port_byte(uint8_t v) {
	*written << v;
}
RW_offset get_scc_addr() {
	return { 0u, 0u };
}
}
void prepare() {
	fixture f;
	rom_base = 0x40800000;
	motherboard = &motherboards[ 0 ];
	written = std::make_unique<std::ostringstream>();
	DEFINE_ROM( B9F46 );
	DEFINE_ROM( B9FDA );
	DEFINE_ROM( B9FF6 );
	DEFINE_ROM( BA182 );
}
BOOST_AUTO_TEST_CASE( no_bit16) {
	prepare();
	auto via = std::make_shared<IO_TEST<VIA1>>(0);
	machine->via1 = via;
	AR(2) = 0x50F00000;
	AR(3) = 0x50F04000;
	DR(3) = 0;
	DR(0) = 0x1f;
	TEST_ROM2( B9A4E, B9928 );
}


BOOST_AUTO_TEST_CASE( wait_notimer2) {
	prepare();
	auto via = std::make_shared<IO_TEST<VIA1>>(0);
	via->set_read_data( VIA_REG::IFR, { 0 } );
	machine->via1 = via;
	AR(2) = 0x50F00000;
	AR(3) = 0x50F04000;
	DR(3) = 0;
	DR(0) = 0x1f;
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_UNSEND + 16 ) |
		1 << ( INIT_FLAG_T::MODEM_PORT_WAIT + 16 );
	TEST_ROM2( B9A4E, B9928 );
}

BOOST_AUTO_TEST_CASE( wait_retry) {
	prepare();
	auto via = std::make_shared<IO_TEST<VIA1>>(0);
	via->set_read_data( VIA_REG::IFR, { 0x20 } );
	machine->via1 = via;
	AR(2) = 0x50F00000;
	AR(3) = 0x50F04000;
	DR(3) = 0;
	DR(0) = 0x1f;
	DR(4) = 4;
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_UNSEND + 16 ) |
		1 << ( INIT_FLAG_T::MODEM_PORT_WAIT + 16 );
	if( rom ) {
		test_rom( 0x408B9A4E, 0x408B9928 );
	} else {
		ROMWrapper::run_B9A4E();
	}
	via->verify( VIA_REG::IER, { 0x20 } );
	via->verify( VIA_REG::TIMER2_L, { 0xff } );
	via->verify( VIA_REG::TIMER2_H, { 0xff } );
	BOOST_TEST( DR(4) == 3 );
}

BOOST_AUTO_TEST_CASE( wait_ok) {
	prepare();
	auto via = std::make_shared<IO_TEST<VIA1>>(0);
	via->set_read_data( VIA_REG::IFR, { 0x20 } );
	machine->via1 = via;
	AR(2) = 0x50F00000;
	AR(3) = 0x50F04000;
	DR(3) = 0;
	DR(0) = 0x1f;
	DR(4) = 0;
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_UNSEND + 16 ) |
		1 << ( INIT_FLAG_T::MODEM_PORT_WAIT + 16 );
	TEST_ROM2( B9A4E, B9928 );
	via->verify( VIA_REG::IER, { 0x20 } );
	via->verify( VIA_REG::TIMER2_L, { 0xff } );
	via->verify( VIA_REG::TIMER2_H, { 0xff } );
	BOOST_CHECK( DR(7) & 1 << ( INIT_FLAG_T::MODEM_PORT_UNSEND + 16) );
	BOOST_CHECK_EQUAL( DR(4), 12 );
}

BOOST_AUTO_TEST_CASE( data) {
	prepare();
	AR(2) = 0x50F00000;
	AR(3) = 0x50F04000;
	DR(2) = 0;
	DR(3) = 0;
	DR(4) = 3;
	DR(6) = 0x12345678;
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_UNSEND + 16 )| 0x9ABC;
	TEST_ROM2( B9A4E, B9928 );
	BOOST_CHECK( ! ( DR(7) & 1 << ( INIT_FLAG_T::MODEM_PORT_UNSEND + 16)) );
	BOOST_CHECK_EQUAL( DR(4), 3 );
	BOOST_CHECK_EQUAL( written->str(), "*APPLE*123456789ABC*1*\r\n" );
}


