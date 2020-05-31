#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "msc.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "via_test.hpp"
#include <memory>
using namespace ROM;
struct MockMSC : public MSC_REG {
	std::vector<uint8_t> executed_cmd;
	uint8_t cmd(uint8_t c) override {
		executed_cmd.push_back( c );
		return 0;
	}
};
void UT_get_sawsVia2(const std::shared_ptr<IO_BASE>& via, uint8_t v);
void prepare(MB_TYPE m) {
	fixture f(m);
	rom_base = 0x40800000;
}	
BOOST_AUTO_TEST_CASE( msc_ok ) {
	prepare(MB_TYPE::MSC);
	machine->msc->DEBUG_SET_C_OUT(0xab );
	machine->via1->write( VIA_REG::ACR, 0x1c ); 
	machine->via1->write( VIA_REG::PCR, 2 ); 
	ROM_FLG.set( ROM_FLG_T::MSC );
	AR(0) = 0x408A8234;
	AR(2) = 0x50f00000;
	AR(3) = 0x50F26000;
	AR(5) = 0;
	if( rom ) {
		test_rom( 0x40847928 );
	} else {
		ROMWrapper::run_47928();
	}
	BOOST_TEST( DR(3) == 0xab );
	BOOST_TEST( DR(6) == 0 );
	BOOST_TEST( DR(4) & 0x80000000 );
}


BOOST_AUTO_TEST_CASE( jaws ) {
	prepare(MB_TYPE::JAWS);
	machine->via1->write( VIA_REG::DDRA, 0x28 ); 
	machine->via1->write( VIA_REG::RA, 0x21 ); 
	machine->via1->write( VIA_REG::DDRB, 0x87 ); 
	machine->via1->write( VIA_REG::RB, 0x87 ); 
	machine->via2->write( VIA_REG::DDRA, 0x00 ); 
	machine->via2->write( VIA_REG::RA, 0x00 ); 
	machine->via2->write( VIA_REG::DDRB, 0xBD ); 
	machine->via2->write( VIA_REG::RB, 0x66 ); 
	UT_get_sawsVia2( machine->via2, 0xab );
	machine->via2->clear( VIA_REG::RB, 2);
	AR(0) = 0x40803608;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	if( rom ) {
		test_rom( 0x40847928 );
	} else {
		ROMWrapper::run_47928();
	}
	BOOST_TEST( DR(3) == 0xab );
	BOOST_TEST( DR(6) == 0 );
	BOOST_TEST( !( DR(4) & 0x80000000) );
}

