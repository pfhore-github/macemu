#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "machine.hpp"
#include "asc.hpp"

using namespace ROM;
#include <time.h>
namespace ROM {
bool easc_is_sonora() { return MOCK::invoke<bool>("easc_is_sonora"); }
void wait_until_easc_fifo_half() { MOCK::invoke<void>("wait_until_easc_fifo_half"); }
int check_easc_fifo_done() { return MOCK::invoke<int>("check_easc_fifo_done"); } // 4
void easc_beep(uint16_t v1, uint16_t v2, uint16_t v3,
			   uint16_t v4, int16_t v5) {
	MOCK::invoke<void>("easc_beep", v1, v2, v3, v4, v5);
}
}
void prepare(MB_TYPE m) {
	fixture f(m);
	MOCK::reset_all();
	MOCK::make<bool()>("easc_is_sonora");
	MOCK::make<void()>("wait_until_easc_fifo_half")->always_do( []() {});
	MOCK::make<int()>("check_easc_fifo_done")
		->nth_is(1, 2)
		.nth_is(2, 4);
	MOCK::make<void(uint16_t, uint16_t, uint16_t, uint16_t, int16_t)>("easc_beep");
	DEFINE_ROM( 45E3C );		// wait_until_easc_fifo_half
	DEFINE_ROM( 45EA2 );		// easc_beep
	DEFINE_ROM( BD43A );		// easc_is_sonora#1
	DEFINE_ROM( BD44C );		// easc_is_sonora#2
	DEFINE_ROM( BD45E );		// easc_is_sonora#3
	DEFINE_ROM( BD470 );		// easc_is_sonora#4
	DEFINE_ROM( BD482 );		// easc_is_sonora#5
	DEFINE_ROM( BD494 );		// check_easc_fifo_done
}
BOOST_AUTO_TEST_CASE( sonora_beep0 ) {
	prepare(MB_TYPE::SONORA);
	MOCK::get<bool()>("easc_is_sonora")->always( true );
	auto easc_beep_mock = MOCK::get<void(uint16_t, uint16_t, uint16_t, uint16_t, int16_t)>("easc_beep");
	for(int i = 0; i < 0x21; ++i ) {
		easc_beep_mock->nth_is(i+1, 0xC000, 0x8000, 0xC080, 0,  ( 0x4000 - i * 0x200 ) );
	}
	int a = 0;
	auto asc = std::make_shared<IO_TEST<ASC>>(0xc0);
	machine->asc = asc;
	asc->set_read_data( ASC_REG::VERSION, { 0xc0 } );
	asc->set_read_data( ASC_REG::FIFO_MODE, { 0x7f, 0xff } );
	AR(5) = 0x50f00000;
	DR(0) = a;	
	AR(3) = 0x50F14000;
	TEST_ROM( 45C64 );
	asc->verify( ASC_REG::A_IRQ_CTL, { 1, 0 } );
	asc->verify( ASC_REG::B_IRQ_CTL, { 1, 0 } );
	asc->verify( ASC_REG::REC, { 2 } );
	asc->verify( ASC_REG::FIFO_MODE, { 0xff, 0x7f } );
	asc->verify( ASC_REG::VOLUME, { 0xe0, 0} );
	asc->verify( ASC_REG::MODE, { 1, 0} );
	asc->verify( ASC_REG::A_FIFO_CTL, { 0, 0x80} );
	asc->verify( ASC_REG::B_FIFO_CTL, { 0, 0x80} );
	asc->verify( ASC_REG::A_SAMPLE_RATE_INCREMENT, { 0x81} );
	asc->verify( ASC_REG::A_SAMPLE_RATE_INCREMENT+1, { 0x2F} );
	asc->verify( ASC_REG::B_SAMPLE_RATE_INCREMENT, { 0x81} );
	asc->verify( ASC_REG::B_SAMPLE_RATE_INCREMENT+1, { 0x2F} );
	asc->verify( ASC_REG::A_CDXA_DECOMP, { 0} );
	asc->verify( ASC_REG::A_CDXA_DECOMP+1, { 0} );
	asc->verify( ASC_REG::A_CDXA_DECOMP+2, { 0} );
	asc->verify( ASC_REG::A_CDXA_DECOMP+3, { 0x3c} );
	asc->verify( ASC_REG::A_CDXA_DECOMP+4, { 0xcc} );
	asc->verify( ASC_REG::A_CDXA_DECOMP+5, { 0x73} );
	asc->verify( ASC_REG::A_CDXA_DECOMP+6, { 0xc9} );
	asc->verify( ASC_REG::A_CDXA_DECOMP+7, { 0x62} );
	asc->verify( ASC_REG::B_CDXA_DECOMP, { 0} );
	asc->verify( ASC_REG::B_CDXA_DECOMP+1, { 0} );
	asc->verify( ASC_REG::B_CDXA_DECOMP+2, { 0} );
	asc->verify( ASC_REG::B_CDXA_DECOMP+3, { 0x3c} );
	asc->verify( ASC_REG::B_CDXA_DECOMP+4, { 0xcc} );
	asc->verify( ASC_REG::B_CDXA_DECOMP+5, { 0x73} );
	asc->verify( ASC_REG::B_CDXA_DECOMP+6, { 0xc9} );
	asc->verify( ASC_REG::B_CDXA_DECOMP+7, { 0x62} );
	asc->verify( ASC_REG::A_LEFT_VOLUME, { 0x7f} );
	asc->verify( ASC_REG::A_RIGHT_VOLUME, { 0} );
	asc->verify( ASC_REG::B_LEFT_VOLUME, { 0} );
	asc->verify( ASC_REG::B_RIGHT_VOLUME, { 0x7f} );
	std::vector<uint8_t> vb(128, 0x80);
	asc->verify( 0, vb );
	asc->verify( 1, vb );
	asc->verify( 2, vb );
	asc->verify( 3, vb );
	asc->verify( 0x400, vb );
	asc->verify( 0x401, vb );
	asc->verify( 0x402, vb );
	asc->verify( 0x403, vb );
}
BOOST_AUTO_TEST_CASE( non_sonora ) {
	prepare(MB_TYPE::V8);
	MOCK::get<bool()>("easc_is_sonora")->always( false );
	auto easc_beep_mock = MOCK::get<void(uint16_t, uint16_t, uint16_t, uint16_t, int16_t)>("easc_beep");
	for(int i = 0; i < 0x21; ++i ) {
		easc_beep_mock->nth_is(i+1, 0xC000, 0x8000, 0xC080, 0,  ( 0x4000 - i * 0x200 ) );
	}
	int a = 0;
	auto asc = std::make_shared<IO_TEST<ASC>>(0x80);
	machine->asc = asc;
	asc->set_read_data( ASC_REG::VERSION, { 0x80 } );
	asc->set_read_data( ASC_REG::FIFO_MODE, { 0x7f, 0xff } );
	AR(5) = 0x50f00000;
	DR(0) = a;	
	AR(3) = 0x50F14000;
	TEST_ROM( 45C64 );
	asc->verify( ASC_REG::FIFO_MODE, { 0xff, 0x7f } );
	asc->verify( ASC_REG::VOLUME, { 0xe0, 0} );
	asc->verify( ASC_REG::MODE, { 1, 0} );
	asc->verify( ASC_REG::CLOCK, { 0 } );
	asc->verify( ASC_REG::CONTROL, { 2} );
}


BOOST_AUTO_TEST_CASE( beep1 ) {
	prepare(MB_TYPE::SONORA);
	MOCK::get<bool()>("easc_is_sonora")->always( true );
	auto easc_beep_mock = MOCK::get<void(uint16_t, uint16_t, uint16_t, uint16_t, int16_t)>("easc_beep");
	for(int i = 0; i < 8; ++i ) {
		easc_beep_mock->nth_is(i+1, 1, 0x8000, 1, 1,  0x4000 );
	}
	for(int i = 0; i < 8; ++i ) {
		easc_beep_mock->nth_is(i+8+1, 0xA145, 0x8000, 1, 1,  0x4000 );
	}
	for(int i = 0; i < 8; ++i ) {
		easc_beep_mock->nth_is(i+8+8+1, 0xA145, 0x8000, 1, 0xBFC9,  0x4000 );
	}
	for(int i = 0; i < 0x21; ++i ) {
		easc_beep_mock->nth_is(i+8+8+8+1, 0xA145, 0x8000, 0, 0xBFC9,  (0x4000-i*0x200) );
	}
	int a = 1;
	auto asc = std::make_shared<IO_TEST<ASC>>(0xc0);
	machine->asc = asc;
	asc->set_read_data( ASC_REG::VERSION, { 0xc0 } );
	asc->set_read_data( ASC_REG::FIFO_MODE, { 0x7f, 0xff } );
	AR(5) = 0x50f00000;
	DR(0) = a;	
	AR(3) = 0x50F14000;
	TEST_ROM( 45C64 );
}
