#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "via1.hpp"
#include "z8530.hpp"
#include "machine.hpp"
#include "devices/serial.hpp"
namespace bdata = boost::unit_test::data;
struct TestDevice : public SerialDevice {
	stream_t s = data_break {};
	void hsk_o() override {}
	void transmit_xd(const stream_t& c) override { s = c; }
};
template<class T>
void dev_run(SCC& scc, TestDevice& dev, const T& w) {
	dev.recieve_xd(w);
}

std::pair<std::shared_ptr<SCC>, std::shared_ptr<TestDevice>>
init_async(bool is_modem, int bit = 8, std::optional<bool> parity_even = {} ) {
	init_machine(MB_TYPE::GLU);
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	if( is_modem ) {
		scc->connect_modem( dev );
	} else {
		scc->connect_printer( dev );
	}
	machine->via1->scc_wr_req = true;
	// WR
	scc->write_reg( is_modem, 9, 0xC0 );
	if( parity_even ) {
		scc->write_reg( is_modem, 4, 0xD | *parity_even << 1 );
	} else {
		scc->write_reg( is_modem, 4, 0xC );
	}

	switch( bit ) {
	case 6 :
		scc->write_reg( is_modem, 3, 0xA0 );
		scc->write_reg( is_modem, 5, 0x40 );
		break;
	case 7 :
		scc->write_reg( is_modem, 3, 0x60 );
		scc->write_reg( is_modem, 5, 0x20 );
		break;
	case 8 :
		scc->write_reg( is_modem, 3, 0xE0 );
		scc->write_reg( is_modem, 5, 0x60 );
		break;
	default :
		scc->write_reg( is_modem, 3, 0x20 );
		scc->write_reg( is_modem, 5, 0x00 );
		break;
	}
	return { scc, dev };
}
BOOST_AUTO_TEST_SUITE( irq )
// CH A
BOOST_DATA_TEST_CASE( int_break, bdata::xrange(2) * bdata::xrange(2),
					  port, high ) {
	auto [ scc, dev ] = init_async(port);	
	scc->write_reg( port, 1, 1 );
	scc->write_reg( port, 9, 8 | high << 4 );
	scc->write_reg( port, 15, 0x80 );
	dev_run(*scc, *dev, data_break {  } );
	static const uint8_t expected[2][2] = { { 2, 0x40 }, { 0xA, 0x50} };
	BOOST_CHECK_EQUAL( scc->read_regB( 2), expected[port][high] );
	BOOST_CHECK( scc->read_reg( port, 0 ) & 0x80 );
}

BOOST_DATA_TEST_CASE( cts, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async(port);	
	scc->write_reg( port, 1, 1 );
	scc->write_reg( port, 9, 8 );
	scc->write_reg( port, 15, 0x20 );

	dev->hsk_i( true );
	dev->hsk_i( false );
	BOOST_CHECK_EQUAL( scc->read_regB( 2), port ? 0xA : 2 );
	BOOST_CHECK( scc->read_reg(port, 0) & 0x20 );
	scc->write_reg( port, 0, 0x10 );
	BOOST_CHECK( ! (scc->read_reg(port, 0 ) & 0x20) );
}

BOOST_DATA_TEST_CASE( dcd, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async(port);	
	scc->write_reg( port, 1, 1 );
	scc->write_reg( port, 9, 8 );
	scc->write_reg( port, 15, 0x8 );

	dev->gp_i( true );
	dev->gp_i( false );
	BOOST_CHECK_EQUAL( scc->read_regB( 2 ), port ? 0xA : 2);
	BOOST_CHECK( scc->read_reg( port, 0 ) & 0x8 );
	scc->write_reg( port, 0, 0x10 );
	BOOST_CHECK( ! (scc->read_reg( port, 0 ) & 0x8) );
}

BOOST_DATA_TEST_CASE( recv_special, bdata::xrange(2) * bdata::xrange(2),
					  port, high ) {
	auto [ scc, dev ] = init_async(port);	
	scc->write_reg( port, 1, 0x18 );
	scc->write_reg( port, 9, 8 | high << 4 );
	dev_run(*scc, *dev, data_sync {} );
	static const uint8_t expected[2][2] = { { 0x6, 0x60 }, { 0xE, 0x70} };
	scc->read_reg( port, 8 ); // data read
	BOOST_CHECK_EQUAL( scc->read_regB( 2 ), expected[port][high] );
}

BOOST_DATA_TEST_CASE( recv_first_char, bdata::xrange(2) * bdata::xrange(2),
					  port, high ) {
	static const uint8_t expected[2][2] = { { 0x4, 0x20 }, { 0xC, 0x30} };
	auto [ scc, dev ] = init_async(port);	
	scc->write_reg( port, 1, 0x08 );
	scc->write_reg( port, 9, 8 | high << 4 );	
	dev_run(*scc, *dev, data_async(0) );
	BOOST_CHECK_EQUAL( scc->read_regB( 2 ), expected[port][high] );
	scc->write_reg( port, 2, 0 );
	dev_run(*scc, *dev, data_async(0) );
	BOOST_CHECK_EQUAL( scc->read_regB( 2 ), 0 );
	scc->write_reg( port, 0, 0x20 );
	dev_run(*scc, *dev, data_async(0) );
	BOOST_CHECK_EQUAL( scc->read_regB( 2 ), expected[port][high] );
}

BOOST_DATA_TEST_CASE( recv_every_char, bdata::xrange(2) * bdata::xrange(2),
					  port, high ) {
	static const uint8_t expected[2][2] = { { 0x4, 0x20 }, { 0xC, 0x30} };
	auto [ scc, dev ] = init_async(port);	
	scc->write_reg( port, 1, 0x10 );
	scc->write_reg( port, 9, 8 | high << 4 );
	dev_run(*scc, *dev, data_async(0) );

	BOOST_CHECK_EQUAL( scc->read_regB( 2 ), expected[port][high] );
	scc->write_reg( port, 2, 0 );
	dev_run(*scc, *dev, data_async(0) );
	BOOST_CHECK_EQUAL( scc->read_regB( 2 ),  expected[port][high] );
}

BOOST_DATA_TEST_CASE( trans, bdata::xrange(2) * bdata::xrange(2),
					  port, high  ) {
	static const uint8_t expected[2][2] = { { 0x0, 0x00 }, { 0x8, 0x10} };
	auto [ scc, dev ] = init_async(port);	
	scc->write_reg( port, 1, 2 );
	scc->write_reg( port, 9, 8 | high << 4);
	scc->write_reg( port, 8, 0xf1 );
	BOOST_CHECK_EQUAL( scc->read_regB( 2 ), expected[port][high] );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( async_io )
BOOST_AUTO_TEST_SUITE( send )

BOOST_DATA_TEST_CASE( rts, bdata::xrange(2), port ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	if( port ) {
		scc->connect_modem( dev );
	} else {
		scc->connect_printer( dev );
	}
	init_machine(MB_TYPE::GLU);
	machine->via1->scc_wr_req = true;
	// WR
	scc->write_reg( port, 9, 0xC0 );
	scc->write_reg( port, 4, 0xC );

	scc->write_reg( port, 3, 0x00 );
	scc->write_reg( port, 5, 0x00 );
	
	scc->write_reg( port, 8, 0xf1 );
	BOOST_CHECK_EQUAL( dev->s.index(), 3 );
	scc->write_reg( port, 5, 2 );
	BOOST_REQUIRE_EQUAL( dev->s.index(), 0 );
	BOOST_CHECK_EQUAL( std::get<0>(dev->s).value, 1 );
}

void send_and_test(bool port, int bit, std::optional<bool> parity_even,
				   uint8_t data, uint8_t expected, bool e_parity = false) {
	auto [ scc, dev ] = init_async(port, bit, parity_even);
	scc->write_reg( port, 8, data );
	BOOST_REQUIRE_EQUAL( dev->s.index(), 0 );
	BOOST_CHECK_EQUAL( std::get<0>(dev->s).value, expected );
	if( parity_even ) {
		BOOST_CHECK_EQUAL( std::get<0>(dev->s).parity, !*parity_even ^ e_parity );
	}
		
}


BOOST_DATA_TEST_CASE( no_parity, bdata::xrange(2), port ) {
	send_and_test( port, 5, {}, 0xf1, 1 );
}

BOOST_DATA_TEST_CASE( parity_odd, bdata::xrange(2), port ) {
	send_and_test( port, 5, false, 0xf1, 1, false );
}

BOOST_DATA_TEST_CASE( parity_1bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, 0xf1, 1, true);
}

BOOST_DATA_TEST_CASE( parity_2bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, 0xE1, 1, true );
}

BOOST_DATA_TEST_CASE( parity_3bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, 0xC5, 5, false );
}

BOOST_DATA_TEST_CASE( parity_4bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, 0x8A, 0xA, false );
}

BOOST_DATA_TEST_CASE( parity_5bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, 0x1A, 0x1A, true );
}

BOOST_DATA_TEST_CASE( parity_6bit, bdata::xrange(2), port ) {
	send_and_test( port, 6, true, 0xFF, 0x3F, false );
}

BOOST_DATA_TEST_CASE( parity_7bit, bdata::xrange(2), port ) {
	send_and_test( port, 7, true, 0xFF, 0x7F, true );
}

BOOST_DATA_TEST_CASE( parity_8bit, bdata::xrange(2), port ) {
	send_and_test( port, 8, true, 0xFF, 0xFF, false );
}

BOOST_DATA_TEST_CASE( transmit_done, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async(port);
	machine->via1->scc_wr_req = false;
	scc->write_reg( port, 1, 0x80 );
	scc->write_reg( port, 8, 0xf1 );
	BOOST_CHECK( ! machine->via1->scc_wr_req );
	BOOST_CHECK( scc->read_reg( port, 1) & 1 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( recv ) 
BOOST_DATA_TEST_CASE( abort, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async( port );
	dev_run(*scc, *dev, data_break {} );
	scc->read_reg( port, 8 );// data read
	BOOST_CHECK( scc->read_reg( port, 0 ) & 0x80 );
}


BOOST_DATA_TEST_CASE( framing_error, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async( port );
	dev_run(*scc, *dev, data_sync {} );
	dev_run(*scc, *dev, data_async(0) );
	scc->read_reg( port, 8 ); // data read
	BOOST_CHECK( scc->read_reg(port, 1) & 0x40  );
	scc->read_reg( port, 8 ); // data read
	BOOST_CHECK( ! (scc->read_reg(port, 1) & 0x40)  );
}

BOOST_DATA_TEST_CASE( parity_error, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async( port, 5, true );
	dev_run(*scc, *dev, data_async(0, true ) );
	dev_run(*scc, *dev, data_async(0) );
	scc->read_reg( port, 8 ); // data read
	BOOST_CHECK( scc->read_reg(port, 1) & 0x10 );
	scc->read_reg( port, 8 ); // data read
	BOOST_CHECK( scc->read_reg(port, 1) & 0x10 );
	scc->write_reg( port, 0, 0x30 ); // error reset
	dev_run(*scc, *dev, data_async(0) );
	scc->read_reg( port, 8 ); // data read
	BOOST_CHECK( ! ( scc->read_reg(port, 1) & 0x10) );
}

BOOST_DATA_TEST_CASE( no_parity, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async(port, 5);	
	dev_run(*scc, *dev, data_async(1) );
	BOOST_CHECK( scc->read_reg( port, 0) & 1 );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0xE1);
}

BOOST_DATA_TEST_CASE( parity_odd, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async(port, 5, false);	
	dev_run(*scc, *dev, data_async(0, true) );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0xE0);
}

BOOST_DATA_TEST_CASE( parity_5bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async(port, 5, true);	
	dev_run(*scc, *dev, data_async(1) );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0xE1);
}

BOOST_DATA_TEST_CASE( parity_6bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async(port, 6, true);	
	dev_run(*scc, *dev, data_async(3) );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8 ), 0xC3);
}

BOOST_DATA_TEST_CASE( parity_7bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async(port, 7, true);	
	dev_run(*scc, *dev, data_async(7) );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8 ), 0x87);
}

BOOST_DATA_TEST_CASE( parity_8bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_async(port, 8, true);	
	dev_run(*scc, *dev, data_async(0xf) );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8 ), 0xF);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE( sync_io )
BOOST_AUTO_TEST_SUITE( send )

BOOST_AUTO_TEST_CASE( sync_6bit ) {
}
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

	
