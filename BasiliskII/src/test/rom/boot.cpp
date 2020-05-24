#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "test_common.hpp"
#include "prototype.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "machine.hpp"

// MOCK
namespace ROM {
void init_mmu() { MOCK::invoke<void>("init_mmu"); }
void init_hw() { MOCK::invoke<void>("init_hw"); }
void init_powerbook() { MOCK::invoke<void>("init_powerbook"); }
void run_rom(uint32_t) {}
}
using namespace ROMWrapper;
BOOST_AUTO_TEST_CASE( exec )  {
	fixture f;
	clear_global();
	MOCK::objects.clear();
	MOCK::make<void()>("init_mmu")->always_do( []() { } );
	MOCK::make<void()>("init_hw")
		->always_do( 
			[]() {
				ROM::motherboard = &ROM::motherboards[0];
				ROM::model = &ROM::old_machines[0];
			} );
	MOCK::make<void()>( "init_powerbook")->always_do( []() {} );
	DEFINE_ROM(04052);
	DEFINE_ROM(02E00);
	DEFINE_ROM(8AC00);
	if( rom) {
		machine->rom_mirror = true;
		test_rom( 0x8c, 0x8E0 );
	} else {
		run_0008C();
	}	
	MOCK::verify("init_mmu");
	MOCK::verify("init_hw");
	MOCK::verify("init_powerbook");
	BOOST_CHECK_EQUAL( get_sr() >> 8, 0x27 );
}

