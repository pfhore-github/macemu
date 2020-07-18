#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include <atomic>
#include "via.hpp"
#include "via_test.hpp"


namespace bdata = boost::unit_test::data;


BOOST_AUTO_TEST_CASE( orb_read_no_latch )  {
	VIA_TEST v;
	v.set_orb( 0x1 );
	v.set_irb( 0x10 );
	v.b[2].set_read_data( { true } );
	v.b[3].set_read_data( { false } );
	v.b[4].set_read_data( { false } );
	v.b[5].set_read_data( { false } );
	v.write( VIA_REG::DDRB, 0xC3 );
	BOOST_TEST( v.read(VIA_REG::RB) == 0x05 );
}

BOOST_AUTO_TEST_CASE( orb_read_latch )  {
	VIA_TEST v;
	v.set_orb( 0x1 );
	v.set_irb( 0x10 );
	v.b[0].set_read_data( { false } );
	v.b[1].set_read_data( { false } );
	v.b[2].set_read_data( { true } );
	v.b[3].set_read_data( { false } );
	v.b[4].set_read_data( { false } );
	v.b[5].set_read_data( { false } );
	v.b[6].set_read_data( { true } );
	v.b[7].set_read_data( { true } );
	v.write( VIA_REG::DDRB, 0xC3 );
	v.write( VIA_REG::ACR, 2 );
	BOOST_TEST( v.read(VIA_REG::RB) == 0x11 );
	v.cb1_in(true);
	v.cb1_in(false);
	BOOST_TEST( v.read(VIA_REG::RB) == 0x05 );
}


BOOST_AUTO_TEST_CASE( orb_write )  {
	VIA_TEST v;
	v.write( VIA_REG::DDRB, 0xf0);
	v.reset_written();
	v.write( VIA_REG::RB, 0xff);
	for(int i = 0; i < 8; ++i ) {
		if( i >= 4 ) {
			v.b[i].verify( { true } );
		} else {
			v.b[i].verify( { } );
		}
	}
}

BOOST_AUTO_TEST_CASE( ora_read_no_latch )  {
	VIA_TEST v;
	v.set_ora( 0x2 );
	v.set_ira( 0x20 );
	v.a[0].set_read_data( { true } );
	v.a[1].set_read_data( { false } );
	v.a[2].set_read_data( { true } );
	v.a[3].set_read_data( { false } );
	v.a[4].set_read_data( { true } );
	v.a[5].set_read_data( { false } );
	v.a[6].set_read_data( { true } );
	v.a[7].set_read_data( { false } );
	v.write( VIA_REG::DDRA, 0xf0);
	BOOST_TEST( v.read(VIA_REG::RA) == 0x55 );
}

BOOST_AUTO_TEST_CASE( ora_read_latch )  {
	VIA_TEST v;
	v.set_ora( 0x1 );
	v.set_ira( 0xAA );
	v.a[0].set_read_data( { true } );
	v.a[1].set_read_data( { false } );
	v.a[2].set_read_data( { true } );
	v.a[3].set_read_data( { false } );
	v.a[4].set_read_data( { true } );
	v.a[5].set_read_data( { false } );
	v.a[6].set_read_data( { true } );
	v.a[7].set_read_data( { false } );
	v.write( VIA_REG::DDRA, 0xf0 );
	v.write( VIA_REG::ACR, 1 );
	BOOST_TEST( v.read(VIA_REG::RA) == 0xAA );
	v.ca1_in(true);
	v.ca1_in(false);
	BOOST_TEST( v.read(VIA_REG::RA) == 0x55 );
}

BOOST_AUTO_TEST_CASE( ora_write )  {
	VIA_TEST v;
	v.write( VIA_REG::DDRA, 0xf0);
	v.reset_written();
	v.write( VIA_REG::RA, 0xff);
	for(int i = 0; i < 8; ++i ) {
		if( i >= 4 ) {
			v.a[i].verify( { true } );
		} else {
			v.a[i].verify( { } );
		}
	}
}


BOOST_AUTO_TEST_CASE( orb_dir )  {
	VIA_TEST v;
	v.set_orb( 0xff );
	v.write( VIA_REG::DDRB, 0xf0); 
	for(int i = 0; i < 8; ++i ) {
		if( i >= 4 ) {
			v.b[i].verify( { true } );
		} else {
			v.b[i].verify( { } );
		}
	}
}

BOOST_AUTO_TEST_CASE( ora_dir )  {
	VIA_TEST v;
	v.set_ora( 0xff );
	v.write( VIA_REG::DDRA, 0x0f); 
	for(int i = 0; i < 8; ++i ) {
		if( i < 4 ) {
			v.a[i].verify( { true } );
		} else {
			v.a[i].verify( { } );
		}
	}
}

BOOST_AUTO_TEST_CASE( timer1 )  {
	VIA_TEST v;
	v.write(VIA_REG::ACR, 0x20);
	v.write(VIA_REG::IER, 0x80 | IRQ_FLAG::TIMER1 );
	// about 40ms
	v.write(VIA_REG::TIMER1_L, 0);
	v.write(VIA_REG::TIMER1_H, 0x80 );
	BOOST_CHECK( !( v.read(VIA_REG::IFR) & IRQ_FLAG::TIMER1 ));
	SDL_Delay(45);
	BOOST_TEST( v.read(VIA_REG::IFR) & IRQ_FLAG::TIMER1 );
}

BOOST_AUTO_TEST_CASE( timer1_pb7)  {
	VIA_TEST v;
	v.write(VIA_REG::ACR, 0x80);
	// about 40ms
	v.write(VIA_REG::TIMER1_L, 0);
	v.write(VIA_REG::TIMER1_H, 0x80 );
	SDL_Delay(45);
	v.b[7].verify( { false, true } );
}

BOOST_AUTO_TEST_CASE( timer1_freerun )  {
	VIA_TEST v;
	v.write(VIA_REG::ACR, 0x60);
	v.write(VIA_REG::IER, IRQ_FLAG::TIMER1 | 0x80 );
	// about 4ms
	v.write(VIA_REG::TIMER1_L, 0x40);
	v.write(VIA_REG::TIMER1_H, 0xC );
	SDL_Delay(100);
	BOOST_TEST( v.irq_cnt > 22);
	BOOST_TEST( v.irq_cnt < 26);
	BOOST_TEST( v.read(13) & IRQ_FLAG::TIMER1 );
	v.write(VIA_REG::ACR, 0x20);
}

BOOST_AUTO_TEST_CASE( timer1_freerun_pb7)  {
	VIA_TEST v;
	v.write(VIA_REG::ACR, 0xc0);
	// about 40ms
	v.write(VIA_REG::TIMER1_L, 0);
	v.write(VIA_REG::TIMER1_H, 0x80 );
	SDL_Delay(130);
	v.b[7].verify( { false, true, false, true } );
	v.write(VIA_REG::ACR, 0x20);
}


BOOST_DATA_TEST_CASE( timer2, bdata::xrange(2), irq)  {
	VIA_TEST v;
	v.write(VIA_REG::ACR, 0x0);
	v.write(VIA_REG::IER, 0x20 | (irq << 7) );
	// about 40ms
	v.write(VIA_REG::TIMER2_L, 0);
	v.write(VIA_REG::TIMER2_H, 0x80 );
	SDL_Delay(45);
	BOOST_TEST( v.irq_cnt == (irq ? 1 : 0));
	BOOST_TEST( v.read(13) & IRQ_FLAG::TIMER2 );
}

BOOST_DATA_TEST_CASE( timer2_counter, bdata::xrange(2), irq)  {
	VIA_TEST v;
	v.write(VIA_REG::ACR, 0x20);
	v.write(VIA_REG::IER, 0x20 | (irq << 7) );
	v.write(VIA_REG::TIMER2_L, 0x80);
	v.write(VIA_REG::TIMER2_H, 0 );
	v.write(VIA_REG::DDRB, 0xff);

	for(int i = 0; i < 128; ++i ) {
		v.write(VIA_REG::RB, 1 << 6);
	}
	if( irq ) {
		BOOST_TEST( v.irq_cnt == 1);
	} else {
		BOOST_TEST( v.irq_cnt == 0);
	}
	BOOST_TEST( v.read(13) & IRQ_FLAG::TIMER2 );
}

BOOST_AUTO_TEST_CASE( sr_in ) {
	VIA_TEST v;
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::SREG );
	v.write(VIA_REG::ACR, 0xC);
	v.cb2_in_push_byte( 0xAA );
	BOOST_CHECK( v.read( VIA_REG::IFR) & IRQ_FLAG::SREG );
	BOOST_CHECK_EQUAL( v.read(VIA_REG::SR), 0xAA );
	
}

BOOST_AUTO_TEST_CASE( sr_out ) {
	VIA_TEST v;
	v.write(VIA_REG::ACR, 0x1C);
	v.write(VIA_REG::SR, 0xAA );
	v.cb2.verify( { 0xAA } );
}


BOOST_AUTO_TEST_CASE( pcr_ca1_negative ) {
	VIA_TEST v;
	v.write( VIA_REG::PCR, 0x8 );
	v.write( VIA_REG::RA_H, 0 );
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::CA1 );
	v.reset_written();
	v.ca1_in( false );
	v.ca1_in( false );
	BOOST_CHECK( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA1 ) );
	v.ca1_in( true );
	BOOST_CHECK( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA1 ) );
	v.ca1_in( true );
	BOOST_CHECK( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA1 ) );
	v.ca1_in( false );
	BOOST_CHECK( ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA1 ) );
}

BOOST_AUTO_TEST_CASE( pcr_ca1_positive ) {
	VIA_TEST v;
	v.ca1_in( true );
	v.write( VIA_REG::PCR, 0x9 );
	v.write( VIA_REG::RA_H, 0 );
	v.reset_written();
	v.ca1_in( true );
	BOOST_CHECK( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA1 ) );
	v.ca1_in( false );
	BOOST_CHECK( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA1 ) );
	v.ca1_in( false );
	BOOST_CHECK( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA1 ) );
	v.ca1_in( true );
	BOOST_CHECK(  v.read( VIA_REG::IFR) & IRQ_FLAG::CA1 );
}

BOOST_AUTO_TEST_CASE( pcr_ca2_input_negative ) {
	VIA_TEST v;
	v.write( VIA_REG::PCR, 0 );
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::CA2 );
	for(int i = 0; i < 8; ++i ) {
		v.a[i].set_read_data( { false } );
	}
	v.reset_written();
	v.ca2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( false );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 );
	v.write( VIA_REG::RA_H, 0 );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
}

BOOST_AUTO_TEST_CASE( pcr_ca2_independent_negative ) {
	VIA_TEST v;
	v.write( VIA_REG::PCR, 2 );
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::CA2 );
	for(int i = 0; i < 8; ++i ) {
		v.a[i].set_read_data( { false } );
	}
	v.reset_written();
	v.ca2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( false );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 );
	v.write( VIA_REG::RA_H, 0 );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 );
}

BOOST_AUTO_TEST_CASE( pcr_ca2_input_positive ) {
	VIA_TEST v;
	v.ca2_in_push( true );
	v.write( VIA_REG::PCR, 4 );
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::CA2 );
	for(int i = 0; i < 8; ++i ) {
		v.a[i].set_read_data( { false } );
	}
	v.reset_written();
	v.ca2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( true );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 );
	v.write( VIA_REG::RA_H, 0 );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
}

BOOST_AUTO_TEST_CASE( pcr_ca2_independent_positive ) {
	VIA_TEST v;
	v.ca2_in_push( true );
	v.write( VIA_REG::PCR, 6 );
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::CA2 );
	for(int i = 0; i < 8; ++i ) {
		v.a[i].set_read_data( { false } );
	}
	v.reset_written();
	v.ca2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 ) );
	v.ca2_in_push( true );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 );
	v.write( VIA_REG::RA_H, 0 );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CA2 );
}


BOOST_AUTO_TEST_CASE( pcr_cb1_negative ) {
	VIA_TEST v;
	v.write( VIA_REG::PCR, 0x80 );
	v.write( VIA_REG::RB, 0 );
	v.reset_written();
	v.cb1_in( false );
	v.cb1_in( false );
	BOOST_CHECK_EQUAL( v.irq_cnt, 0 );
	v.cb1_in( true );
	BOOST_CHECK_EQUAL( v.irq_cnt, 0 );
	v.cb1_in( true );
	BOOST_CHECK_EQUAL( v.irq_cnt, 0 );
	v.cb1_in( false );
	BOOST_CHECK_EQUAL( v.irq_cnt, 1 );
}
BOOST_AUTO_TEST_CASE( pcr_cb1_positive ) {
	VIA_TEST v;
	v.cb1_in( true );
	v.write( VIA_REG::PCR, 0x90 );
	v.write( VIA_REG::RB, 0 );
	v.reset_written();
	v.cb1_in( true );
	BOOST_CHECK_EQUAL( v.irq_cnt, 0 );
	v.cb1_in( false );
	BOOST_CHECK_EQUAL( v.irq_cnt, 0 );
	v.cb1_in( false );
	BOOST_CHECK_EQUAL( v.irq_cnt, 0 );
	v.cb1_in( true );
	BOOST_CHECK_EQUAL( v.irq_cnt, 1 );
}

BOOST_AUTO_TEST_CASE( pcr_cb2_input_negative ) {
	VIA_TEST v;
	v.write( VIA_REG::PCR, 0x0 );
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::CB2 );
	for(int i = 0; i < 8; ++i ) {
		v.b[i].set_read_data( { false } );
	}
	v.reset_written();
	v.cb2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( false );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 );
	v.write( VIA_REG::RB, 0 );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
}

BOOST_AUTO_TEST_CASE( pcr_cb2_independent_negative ) {
	VIA_TEST v;
	v.write( VIA_REG::PCR, 0x20 );
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::CB2 );
	for(int i = 0; i < 8; ++i ) {
		v.b[i].set_read_data( { false } );
	}
	v.reset_written();
	v.cb2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( false );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 );
	v.write( VIA_REG::RB, 0 );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 );
}

BOOST_AUTO_TEST_CASE( pcr_cb2_input_positive ) {
	VIA_TEST v;
	v.cb2_in_push( true );
	v.write( VIA_REG::PCR, 0x40 );
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::CB2 );
	for(int i = 0; i < 8; ++i ) {
		v.b[i].set_read_data( { false } );
	}
	v.reset_written();
	v.cb2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( true );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 );
	v.write( VIA_REG::RB, 0 );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
}



BOOST_AUTO_TEST_CASE( pcr_cb2_independent_positive ) {
	VIA_TEST v;
	v.cb2_in_push( true );
	v.write( VIA_REG::PCR, 0x60 );
	v.write( VIA_REG::IER, 0x80 | IRQ_FLAG::CB2 );
	for(int i = 0; i < 8; ++i ) {
		v.b[i].set_read_data( { false } );
	}
	v.reset_written();
	v.cb2_in_push( true );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( false );
	BOOST_TEST( ! ( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 ) );
	v.cb2_in_push( true );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 );
	v.write( VIA_REG::RB, 0 );
	BOOST_TEST( v.read( VIA_REG::IFR) & IRQ_FLAG::CB2 );
}


// taken from ROM$46AA
BOOST_AUTO_TEST_CASE( works )  {
	VIA_TEST v;
	uint8_t x = v.read(VIA_REG::IER);
	uint8_t l = 0xff;
	v.write(VIA_REG::IER, l);
	do {
		l = ~l + 1;
		v.write(VIA_REG::IER, l);
		l *= 2;
		if( l == 0 ) break;
		l = ~l + 1;
	} while( v.read(VIA_REG::IER) == l );
	v.write(VIA_REG::IER, 0x7f);
	v.write(VIA_REG::IER, x);
	BOOST_TEST( l == 0 );
}
