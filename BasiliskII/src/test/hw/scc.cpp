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
		scc->write_reg( is_modem, 3, 0xA1 );
		scc->write_reg( is_modem, 5, 0x48 );
		break;
	case 7 :
		scc->write_reg( is_modem, 3, 0x61 );
		scc->write_reg( is_modem, 5, 0x28 );
		break;
	case 8 :
		scc->write_reg( is_modem, 3, 0xE1 );
		scc->write_reg( is_modem, 5, 0x68 );
		break;
	default :
		scc->write_reg( is_modem, 3, 0x21 );
		scc->write_reg( is_modem, 5, 0x08 );
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

	scc->write_reg( port, 3, 0x01 );
	scc->write_reg( port, 5, 0x08 );
	
	scc->write_reg( port, 8, 0xf1 );
	BOOST_CHECK_EQUAL( dev->s.index(), 4 );
	scc->write_reg( port, 5, 0xA );
	BOOST_REQUIRE_EQUAL( dev->s.index(), 1 );
	BOOST_CHECK_EQUAL( std::get<1>(dev->s).value, 1 );
}

BOOST_DATA_TEST_CASE( auto_enable, bdata::xrange(2), port ) {
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

	scc->write_reg( port, 3, 0x21 );
	scc->write_reg( port, 5, 0x02 );
	
	scc->write_reg( port, 8, 0xf1 );
	BOOST_CHECK_EQUAL( dev->s.index(), 4 );
	dev->hsk_i(true);
	BOOST_REQUIRE_EQUAL( dev->s.index(), 1 );
	BOOST_CHECK_EQUAL( std::get<1>(dev->s).value, 1 );
}


void send_and_test(bool port, int bit, std::optional<bool> parity_even,
				   uint8_t data, uint8_t expected, bool e_parity = false) {
	auto [ scc, dev ] = init_async(port, bit, parity_even);
	scc->write_reg( port, 8, data );
	BOOST_REQUIRE_EQUAL( dev->s.index(), 1 );
	BOOST_CHECK_EQUAL( std::get<1>(dev->s).value, expected );
	if( parity_even ) {
		BOOST_CHECK_EQUAL( std::get<1>(dev->s).parity, !*parity_even ^ e_parity );
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

std::pair<std::shared_ptr<SCC>, std::shared_ptr<TestDevice>>
init_sync(bool is_modem, int sync_size, std::optional<bool> crc = {}) {
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
	switch( sync_size ) {
	case 6 :
		scc->write_reg( is_modem, 4, 0 );
		scc->write_reg( is_modem, 10, 1 | ( crc ? *crc : 0 ) << 7);
		break;
	case 8 :
		scc->write_reg( is_modem, 4, 0 );
		scc->write_reg( is_modem, 10, ( crc ? *crc : 0 ) << 7);
		break;
	case 12 :
		scc->write_reg( is_modem, 4, 0x10 );
		scc->write_reg( is_modem, 10, 1 | ( crc ? *crc : 0 ) << 7);
		break;
	case 16 :
		scc->write_reg( is_modem, 4, 0x10 );
		scc->write_reg( is_modem, 10, ( crc ? *crc : 0 ) << 7);
		break;
	}
	scc->write_reg( is_modem, 3, 0xC1 | ( crc ? 1 : 0 ) << 3 );
	scc->write_reg( is_modem, 5, 0x68 | ( crc ? 1 : 0 ) );
	scc->write_reg( is_modem, 0, 1 << 6 );
	scc->write_reg( is_modem, 0, 2 << 6 );
	return { scc, dev };
}


BOOST_AUTO_TEST_SUITE( sync_io )
BOOST_AUTO_TEST_SUITE( send )

BOOST_DATA_TEST_CASE( value, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_sync(port, 6, false);
	scc->write_reg( port, 8, 0xff );
	scc->write_all_data( port, { 0x12, 0x34, 0x56 } );
	
	BOOST_REQUIRE_EQUAL( dev->s.index(), 2 );
	auto ret = std::get<2>(dev->s);
	std::vector<uint8_t> expected = { 0xff, 0x12, 0x34, 0x56 };
	BOOST_CHECK_EQUAL_COLLECTIONS( ret.values.begin(), ret.values.end(),
								   expected.begin(), expected.end() );
}

BOOST_DATA_TEST_CASE( crc, bdata::xrange(2) * bdata::xrange(2),
					  port, crc_init ) {
	auto [ scc, dev ] = init_sync(port, 6, crc_init);
	scc->write_reg( port, 6, 0xff );
	scc->write_all_data( port, { 0x35, 0x2a, 0xd4 } );
	
	BOOST_REQUIRE_EQUAL( dev->s.index(), 2 );
	auto ret = std::get<2>(dev->s);
	BOOST_CHECK_EQUAL( ret.crc, crc_init ? 0x801d : 0x4c81 );
}

BOOST_DATA_TEST_CASE( sync_6bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_sync(port, 6, false);
	scc->write_reg( port, 6, 0xff );
	scc->write_all_data( port, { 0x12 } );
	
	BOOST_REQUIRE_EQUAL( dev->s.index(), 2 );
	auto ret = std::get<2>(dev->s);
	BOOST_CHECK_EQUAL( ret.sync, 0x3f );
}

BOOST_DATA_TEST_CASE( sync_8bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_sync(port, 8, false);
	scc->write_reg( port, 6, 0xff );
	scc->write_all_data( port, { 0x12 } );
	
	BOOST_REQUIRE_EQUAL( dev->s.index(), 2 );
	auto ret = std::get<2>(dev->s);
	BOOST_CHECK_EQUAL( ret.sync, 0xff );
}

BOOST_DATA_TEST_CASE( sync_16bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_sync(port, 16, false);
	scc->write_reg( port, 6, 0x12 );
	scc->write_reg( port, 7, 0x34 );
	scc->write_all_data( port, { 0x12 } );
	
	BOOST_REQUIRE_EQUAL( dev->s.index(), 2 );
	auto ret = std::get<2>(dev->s);
	BOOST_CHECK_EQUAL( ret.sync, 0x3412 );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( recv )
BOOST_DATA_TEST_CASE( value, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_sync(port, 6, false);
	scc->write_reg( port, 7, 0x10 );
	dev_run( *scc, *dev, data_sync { 0x10, { 0x12, 0x34, 0x56 }, 0 } );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0x12 );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0x34 );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0x56 );		
}

BOOST_DATA_TEST_CASE( crc, bdata::xrange(2) * bdata::xrange(2),
					  port, crc_init ) {
	auto [ scc, dev ] = init_sync(port, 6, crc_init);
	scc->write_reg( port, 6, 0x10 );
	dev_run( *scc, *dev, data_sync { 0x10, { 0x35, 0x2a, 0xd4 }, uint16_t(crc_init ? 0x801d : 0x4c81) } );
	scc->read_reg( port, 8);
	scc->read_reg( port, 8);
	BOOST_CHECK( scc->read_reg( port, 1) & 0x40 );
	scc->read_reg( port, 8);
	BOOST_CHECK( ! (scc->read_reg( port, 1) & 0x40) );
}

BOOST_DATA_TEST_CASE( sync_6bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_sync(port, 6);
	scc->write_reg( port, 7, 0x10 );
	scc->write_reg( port, 3, 0xD1 );
	dev_run( *scc, *dev, data_sync { 0x13, { 0x11 }, 0 } );
	dev_run( *scc, *dev, data_sync { 0x10, { 0x22 }, 0 } );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0x22 );
}

BOOST_DATA_TEST_CASE( sync_8bit, bdata::xrange(2) * bdata::xrange(2),
					  port, inhibit ) {
	auto [ scc, dev ] = init_sync(port, 8);
	scc->write_reg( port, 7, 0xff );
	scc->write_reg( port, 3, 0xD1 | inhibit << 1);
	dev_run( *scc, *dev, data_sync { 0x7f, { 0x11 }, 0 } );
	dev_run( *scc, *dev, data_sync { 0xff, { 0x22 }, 0 } );
	if( ! inhibit ) {
		BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0xff );
	}
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0x22 );
}

BOOST_DATA_TEST_CASE( sync_12bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_sync(port, 12);
	scc->write_reg( port, 6, 0x3f );
	scc->write_reg( port, 7, 0x14 );
	scc->write_reg( port, 3, 0xD1 );
	dev_run( *scc, *dev, data_sync { 0x123, { 0x11 }, 0 } );
	dev_run( *scc, *dev, data_sync { 0x143, { 0x22 }, 0 } );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0x22 );
}

BOOST_DATA_TEST_CASE( sync_16bit, bdata::xrange(2), port ) {
	auto [ scc, dev ] = init_sync(port, 16);
	scc->write_reg( port, 6, 0xab );
	scc->write_reg( port, 7, 0xcd );
	scc->write_reg( port, 3, 0xD1 );
	dev_run( *scc, *dev, data_sync { 0xaaaa, { 0x11 }, 0 } );
	dev_run( *scc, *dev, data_sync { 0xcdab, { 0x22 }, 0 } );
	BOOST_CHECK_EQUAL( scc->read_reg( port, 8), 0x22 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

	
