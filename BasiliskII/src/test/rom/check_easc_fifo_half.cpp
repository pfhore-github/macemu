#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "asc.hpp"
using namespace ROM;
std::shared_ptr<IO_TEST<ASC>> prepare() {
	fixture f(MB_TYPE::SONORA);
	rom_base = 0x40800000;
	AR(6) = 0x50F14000;
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbc);
	machine->asc = asc;
	return asc;
}
BOOST_AUTO_TEST_CASE( dual ) {
	auto asc = prepare();
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 4 } );
	if( rom ) {
		test_rom( 0x408BD41E, 0x40845E46 );
	} else {
		ROMWrapper::run_BD41E();
	}
	BOOST_CHECK_EQUAL( cpu.PC ,  0x40845E46 );
}

BOOST_AUTO_TEST_CASE( half ) {
	auto asc = prepare();
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 1 } );
	if( rom ) {
		test_rom( 0x408BD41E, 0x40845E44 );
	} else {
		ROMWrapper::run_BD41E();
	}
	BOOST_CHECK_EQUAL( cpu.PC ,  0x40845E44 );
	BOOST_CHECK( ! cpu.Z );
}

BOOST_AUTO_TEST_CASE( none ) {
	auto asc = prepare();
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 0 } );
	if( rom ) {
		test_rom( 0x408BD41E, 0x40845E44 );
	} else {
		ROMWrapper::run_BD41E();
	}
	BOOST_CHECK_EQUAL( cpu.PC ,  0x40845E44 );
	BOOST_CHECK( cpu.Z );
}



