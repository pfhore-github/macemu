#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
using namespace ROM;
BOOST_AUTO_TEST_CASE( not_found )  {
	fixture f;
	machine->model_id = 0;
	if( rom ) {
		test_rom( 0x40804BE4);
	} else {
		ROMWrapper::run_04BE4();
	}
	BOOST_TEST( cpu.Z == false );
}

BOOST_AUTO_TEST_CASE( found )  {
	fixture f;
	machine->model_id = 0xA55A2252;
	if( rom ) {
		test_rom( 0x40804BE4 );
	} else {
		ROMWrapper::run_04BE4();
	}
	BOOST_TEST( cpu.Z == true );
	BOOST_TEST( DR(0) == 0xA55A2252 );
}

