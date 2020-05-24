#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

using namespace ROM;
void prepare() {
	fixture f;
	set_sr( 0x2700 );
	rom_base = 0x40800000;
}	
BOOST_AUTO_TEST_CASE( specific )  {
	prepare();
	model = &old_machines[0];
	motherboard = &motherboards[0];
	INIT_FLAGS.set( INIT_FLAG_T::FAULT_SKIP );
	if( rom ) {
		test_rom( 0x4080309C );
	} else {
		ROMWrapper::run_0309C();
	}
	ROM_FLG = DR(1);
	INIT_FLAGS = DR(7);
	BOOST_TEST( DR(1) == model->rom_flg.to_ulong() );
	BOOST_TEST( ! INIT_FLAGS.test( INIT_FLAG_T::FAULT_SKIP ) );
}

BOOST_AUTO_TEST_CASE( generic )  {
	prepare();
	model = &old_machines[16];
	motherboard = &motherboards[3];
	if( rom ) {
		test_rom( 0x4080309C );
	} else {
		ROMWrapper::run_0309C();
	}
	BOOST_TEST( DR(1) == motherboard->rom_flg.to_ulong() );
	BOOST_TEST( ! INIT_FLAGS.test( INIT_FLAG_T::FAULT_SKIP ) );
}
