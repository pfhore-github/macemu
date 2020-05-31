#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "via1.hpp"
#include "z8530.hpp"
#include "machine.hpp"
#include "devices/serial.hpp"
namespace bdata = boost::unit_test::data;
struct TestDevice : public SerialDevice {
	std::vector<bool> w;
	void hsk_o() override {}
	void transmit_xd(const std::vector<bool>& v) override { w = v; }
};
void dev_run(SCC& scc, TestDevice& dev, const std::vector<bool>& w) {
	dev.recieve_xd(w);
}
void init_async(SCC& scc, int bit, bool auto_enable, bool parity, bool parity_even, SCC_REG::SCC_REG_t reg = SCC_REG::A_CMD) {
	init_machine(MB_TYPE::GLU);
	machine->via1->scc_wr_req = true;
	// WR
	scc.write( reg, 9 );
	scc.write( reg, 0xC0 );

	scc.write( reg, 4 );
	scc.write( reg, 0xC | parity_even << 1 | parity );

	switch( bit ) {
	case 6 :
		scc.write( reg, 3 );
		scc.write( reg, 0x80 | auto_enable << 5);
		scc.write( reg, 5 );
		scc.write( reg, 0x40 ); 
		break;
	case 7 :
		scc.write( reg, 3 );
		scc.write( reg, 0x40  | auto_enable << 5);
		scc.write( reg, 5 );
		scc.write( reg, 0x20 );
		break;
	case 8 :
		scc.write( reg, 3 );
		scc.write( reg, 0xC0 | auto_enable << 5 );
		scc.write( reg, 5 );
		scc.write( reg, 0x60 );
		break;
	default :
		scc.write( reg, 3 );
		scc.write( reg, 0x00 | auto_enable << 5 );
		scc.write( reg, 5 );
		scc.write( reg, 0x00 );
		break;
	}
}
BOOST_AUTO_TEST_SUITE( irq )
// CH A
BOOST_DATA_TEST_CASE( ch_a_external, bdata::xrange(2), high ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, false, false );
	scc->write( SCC_REG::A_CMD, 1 );
	scc->write( SCC_REG::A_CMD, 1 );
	scc->write( SCC_REG::A_CMD, 9 );
	scc->write( SCC_REG::A_CMD, 8 | high << 4 );
	scc->write( SCC_REG::A_CMD, 15 );
	scc->write( SCC_REG::A_CMD, 0x80 );
	
	dev_run(*scc, *dev, {  } );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), high ? 0x50 : 5 );
}

BOOST_DATA_TEST_CASE( ch_a_recv_special, bdata::xrange(2), high ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, false, false );
	scc->write( SCC_REG::A_CMD, 1 );
	scc->write( SCC_REG::A_CMD, 0x18 );
	scc->write( SCC_REG::A_CMD, 9 );
	scc->write( SCC_REG::A_CMD, 8 | high << 4 );
	
	dev_run(*scc, *dev, { false } );
	scc->read( SCC_REG::A_DATA ); // data read
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), high ? 0x70 : 7 );
}

BOOST_DATA_TEST_CASE( ch_a_recv_first_char, bdata::xrange(2), high ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, false, false );
	scc->write( SCC_REG::A_CMD, 1 );
	scc->write( SCC_REG::A_CMD, 0x08 );
	scc->write( SCC_REG::A_CMD, 9 );
	scc->write( SCC_REG::A_CMD, 8 | high << 4 );
	
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), high ? 0x30 : 6 );
	scc->write( SCC_REG::A_CMD, 2 );
	scc->write( SCC_REG::A_CMD, 0 );
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), 0 );
	scc->write( SCC_REG::A_CMD, 0x20 );
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), high ? 0x30 : 6 );
}

BOOST_DATA_TEST_CASE( ch_a_recv_every_char, bdata::xrange(2), high ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, false, false );
	scc->write( SCC_REG::A_CMD, 1 );
	scc->write( SCC_REG::A_CMD, 0x10 );
	scc->write( SCC_REG::A_CMD, 9 );
	scc->write( SCC_REG::A_CMD, 8 | high << 4 );
	
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ),  high ? 0x30 :6 );
	scc->write( SCC_REG::A_CMD, 2 );
	scc->write( SCC_REG::A_CMD, 0 );
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ),  high ? 0x30 :6 );
}

BOOST_DATA_TEST_CASE( ch_a_trans, bdata::xrange(2), high  ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, false, false, SCC_REG::A_CMD );
	scc->write( SCC_REG::A_CMD, 1 );
	scc->write( SCC_REG::A_CMD, 2 );
	scc->write( SCC_REG::A_CMD, 9 );
	scc->write( SCC_REG::A_CMD, 8 | high << 4);
	scc->write( SCC_REG::A_DATA, 0xf1 );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), high ? 0x10 : 4 );
}

// CH B
BOOST_DATA_TEST_CASE( ch_b_external, bdata::xrange(2), high ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_printer( dev );
	init_async(*scc, 5, true, false, false, SCC_REG::B_CMD );
	scc->write( SCC_REG::B_CMD, 1 );
	scc->write( SCC_REG::B_CMD, 1 );
	scc->write( SCC_REG::B_CMD, 9 );
	scc->write( SCC_REG::B_CMD, 8 | high << 4 );
	scc->write( SCC_REG::B_CMD, 15 );
	scc->write( SCC_REG::B_CMD, 0x80 );
	
	dev_run(*scc, *dev, {  } );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), high ? 0x40 : 1 );
}
BOOST_DATA_TEST_CASE( ch_b_recv_special, bdata::xrange(2), high ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_printer( dev );
	init_async(*scc, 5, true, false, false, SCC_REG::B_CMD );
	scc->write( SCC_REG::B_CMD, 1 );
	scc->write( SCC_REG::B_CMD, 0x18 );
	scc->write( SCC_REG::B_CMD, 9 );
	scc->write( SCC_REG::B_CMD, 8 | high << 4 );
	
	dev_run(*scc, *dev, { false } );
	scc->read( SCC_REG::B_DATA ); // data read
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), high ? 0x60 : 3 );
}

BOOST_DATA_TEST_CASE( ch_b_recv_first_char, bdata::xrange(2), high ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_printer( dev );
	init_async(*scc, 5, true, false, false, SCC_REG::B_CMD );
	scc->write( SCC_REG::B_CMD, 1 );
	scc->write( SCC_REG::B_CMD, 0x08 );
	scc->write( SCC_REG::B_CMD, 9 );
	scc->write( SCC_REG::B_CMD, 8 | high << 4 );
	
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), high ? 0x20 : 2 );
	scc->write( SCC_REG::B_CMD, 2 );
	scc->write( SCC_REG::B_CMD, 0 );
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), 0 );
	scc->write( SCC_REG::B_CMD, 0x20 );
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), high ? 0x20 : 2 );
}

BOOST_DATA_TEST_CASE( ch_b_recv_every_char, bdata::xrange(2), high ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_printer( dev );
	init_async(*scc, 5, true, false, false, SCC_REG::B_CMD );
	scc->write( SCC_REG::B_CMD, 1 );
	scc->write( SCC_REG::B_CMD, 0x10 );
	scc->write( SCC_REG::B_CMD, 9 );
	scc->write( SCC_REG::B_CMD, 8 | high << 4 );
	
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ),  high ? 0x20 :2 );
	scc->write( SCC_REG::B_CMD, 2 );
	scc->write( SCC_REG::B_CMD, 0 );
	dev_run(*scc, *dev, { false, false,false,false,false} );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ),  high ? 0x20 :2 );
}

BOOST_DATA_TEST_CASE( ch_b_trans, bdata::xrange(2), high  ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_printer( dev );
	init_async(*scc, 5, true, false, false, SCC_REG::B_CMD );
	scc->write( SCC_REG::B_CMD, 1 );
	scc->write( SCC_REG::B_CMD, 2 );
	scc->write( SCC_REG::B_CMD, 9 );
	scc->write( SCC_REG::B_CMD, 8 | high << 4);
	scc->write( SCC_REG::B_DATA, 0xf1 );
	scc->write( SCC_REG::B_CMD, 2 );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::B_CMD ), 0 );
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( async_send )

BOOST_DATA_TEST_CASE( rts, bdata::xrange(2), port ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	SCC_REG::SCC_REG_t reg = port ? SCC_REG::A_CMD : SCC_REG::B_CMD;
	SCC_REG::SCC_REG_t reg2 = port ? SCC_REG::A_DATA : SCC_REG::B_DATA;
	if( port ) {
		scc->connect_modem( dev );
	} else {
		scc->connect_printer( dev );
	}
	init_async(*scc, 5, false, false, false, reg );
	scc->write( reg2, 0xf1 );
	BOOST_CHECK( dev->w.empty() );
	scc->write( reg, 5 );
	scc->write( reg, 2 );
	std::vector expected = { true } ;
	BOOST_CHECK_EQUAL_COLLECTIONS( dev->w.begin(), dev->w.end(), expected.begin(), expected.end() );
}

void send_and_test(bool port, int bit, bool parity, bool parity_even,
				   uint8_t data, const std::vector<bool>& expected) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	SCC_REG::SCC_REG_t reg = port ? SCC_REG::A_CMD : SCC_REG::B_CMD;
	SCC_REG::SCC_REG_t reg2 = port ? SCC_REG::A_DATA : SCC_REG::B_DATA;
	if( port ) {
		scc->connect_modem( dev );
	} else {
		scc->connect_printer( dev );
	}
	init_async(*scc, bit, true, parity, parity_even, reg );
	scc->write( reg2, data );
	if( expected.empty() ) {
		BOOST_CHECK( dev->w.empty() );
	} else {
		BOOST_CHECK_EQUAL_COLLECTIONS( dev->w.begin(), dev->w.end(), expected.begin(), expected.end() );
	}
}


BOOST_DATA_TEST_CASE( no_parity, bdata::xrange(2), port ) {
	send_and_test( port, 5, false, false, 0xf1, { true } );
}

BOOST_DATA_TEST_CASE( parity_odd, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, false, 0xf1, { true, false } );
}

BOOST_DATA_TEST_CASE( parity_1bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, true, 0xf1, { true, true } );
}

BOOST_DATA_TEST_CASE( parity_2bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, true, 0xE1, { true, false, true } );
}

BOOST_DATA_TEST_CASE( parity_3bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, true, 0xC5, { true, false, true, false } );
}

BOOST_DATA_TEST_CASE( parity_4bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, true, 0x8A, { false, true, false, true, false } );
}

BOOST_DATA_TEST_CASE( parity_5bit, bdata::xrange(2), port ) {
	send_and_test( port, 5, true, true, 0x1A,
				   { false, true, false, true, true, true } );
}

BOOST_DATA_TEST_CASE( parity_6bit, bdata::xrange(2), port ) {
	send_and_test( port, 6, true, true, 0xFF,
				   { true, true, true, true, true, true, false } );
}

BOOST_DATA_TEST_CASE( parity_7bit, bdata::xrange(2), port ) {
	send_and_test( port, 7, true, true, 0xFF,
				   { true, true, true, true, true, true, true, true } );
}

BOOST_DATA_TEST_CASE( parity_8bit, bdata::xrange(2), port ) {
	send_and_test( port, 8, true, true, 0xFF,
				   { true, true, true, true, true, true, true, true, false } );
}

BOOST_DATA_TEST_CASE( transmit_done, bdata::xrange(2), port ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	SCC_REG::SCC_REG_t reg = port ? SCC_REG::A_CMD : SCC_REG::B_CMD;
	SCC_REG::SCC_REG_t reg2 = port ? SCC_REG::A_DATA : SCC_REG::B_DATA;
	if( port ) {
		scc->connect_modem( dev );
	} else {
		scc->connect_printer( dev );
	}
	init_async(*scc, 5, true, false, false, reg );
	machine->via1->scc_wr_req = false;
	scc->write( reg, 1 );
	scc->write( reg, 0x80 );
	scc->write( reg2, 0xf1 );
	BOOST_CHECK( ! machine->via1->scc_wr_req );
	scc->write( reg, 1 );
	BOOST_TEST( scc->bit( reg, 0 ) );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE( async_recv ) 
BOOST_AUTO_TEST_CASE( abort ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, false, false );
	dev_run(*scc, *dev, {  } );
	scc->read( SCC_REG::A_DATA ); // data read
	BOOST_TEST( scc->bit(SCC_REG::A_CMD, 7) );
}


BOOST_AUTO_TEST_CASE( framing_error ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, false, false );
	dev_run(*scc, *dev, { false } );
	dev_run(*scc, *dev, { false, false, false, false, false} );
	scc->write( SCC_REG::A_CMD, 1 );
	scc->read( SCC_REG::A_DATA ); // data read
	BOOST_TEST( scc->bit(SCC_REG::A_CMD, 6) );
	scc->read( SCC_REG::A_DATA ); // data read
	scc->write( SCC_REG::A_CMD, 1 );
	BOOST_TEST( ! scc->bit(SCC_REG::A_CMD, 6) );
}

BOOST_AUTO_TEST_CASE( parity_error ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, true, true );
	dev_run(*scc, *dev, { false, false, false, false, false, true } );
	dev_run(*scc, *dev, { false, false, false, false, false, false } );
	scc->read( SCC_REG::A_DATA ); // data read
	scc->write( SCC_REG::A_CMD, 1 );
	BOOST_TEST( scc->bit(SCC_REG::A_CMD, 4) );
	scc->read( SCC_REG::A_DATA ); // data read
	scc->write( SCC_REG::A_CMD, 1 );
	BOOST_TEST( scc->bit(SCC_REG::A_CMD, 4) );
	scc->write( SCC_REG::A_CMD, 0x30 ); // error reset
 	dev_run(*scc, *dev, { false, false, false, false, false, false } );
	scc->read( SCC_REG::A_DATA ); // data read
	scc->write( SCC_REG::A_CMD, 1 );
	BOOST_TEST( ! scc->bit(SCC_REG::A_CMD, 4) );
}

BOOST_AUTO_TEST_CASE( no_parity ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, false, false );
	dev_run(*scc, *dev, { true, false, false, false, false } );
	BOOST_CHECK( scc->bit(SCC_REG::A_CMD, 0) );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::A_DATA ), 0xE1);
}

BOOST_AUTO_TEST_CASE( parity_odd ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, true, false );
	dev_run(*scc, *dev, { false, false, false, false, false, true } );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::A_DATA ), 0xE0);
}

BOOST_AUTO_TEST_CASE( parity_5bit ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 5, true, true, true );
	dev_run(*scc, *dev, { true, false, false, false, false, true } );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::A_DATA ), 0xE1);
}

BOOST_AUTO_TEST_CASE( parity_6bit ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 6, true, true, true );
	dev_run(*scc, *dev, { true, true, false, false, false, false, false } );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::A_DATA ), 0xC3);
}

BOOST_AUTO_TEST_CASE( parity_7bit ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 7, true, true, true );
	dev_run(*scc, *dev, { true, true, true, false, false, false, false, true } );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::A_DATA ), 0x87);
}

BOOST_AUTO_TEST_CASE( parity_8bit ) {
	auto scc = newZ8530();
 	auto dev = std::make_shared<TestDevice>();
	scc->connect_modem( dev );
	init_async(*scc, 8, true, true, true );
	dev_run(*scc, *dev, { true, true, true, true, false, false, false, false, false } );
	BOOST_CHECK_EQUAL( scc->read( SCC_REG::A_DATA ), 0xF);
}


BOOST_AUTO_TEST_SUITE_END()



