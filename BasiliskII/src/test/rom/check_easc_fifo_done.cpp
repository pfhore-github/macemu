#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "asc.hpp"
#include "sonora.hpp"
#include "glu.hpp"
using namespace ROM;
template<class T>
void prepare() {
	fixture f(std::make_unique<T>());
	rom_base = 0x40800000;
}
BOOST_AUTO_TEST_CASE( sonora_ok ) {
	prepare<Sonora>();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbc);
	machine->asc = asc;
	AR(6) = 0x50F14000;
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 0xC } );
	asc->set_read_data( ASC_REG::VERSION, { 0xBC } );
	if( rom ) {
		test_rom( 0x408BD494, 0x40845E1E );
	} else {
		ROMWrapper::run_BD494();
	}
	BOOST_CHECK_EQUAL( DR(0) ,  0xC );
	BOOST_CHECK( cpu.Z );
	BOOST_CHECK_EQUAL( cpu.PC ,  0x40845E1E );
}

BOOST_AUTO_TEST_CASE( sonora_ng ) {
	prepare<Sonora>();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbc);
	machine->asc = asc;
	AR(6) = 0x50F14000;
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 0xB, 5 } );
	asc->set_read_data( ASC_REG::VERSION, { 0xBC } );
	if( rom ) {
		test_rom( 0x408BD494, 0x40845E18 );
	} else {
		ROMWrapper::run_BD494();
	}
	BOOST_CHECK_EQUAL( DR(0) ,  1 );
	BOOST_CHECK_EQUAL( cpu.PC ,  0x40845E18 );
}

BOOST_AUTO_TEST_CASE( ardbeg_ok ) {
//	prepare<Ardbeg>();
	prepare<Sonora>();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbb);
	machine->asc = asc;
	AR(6) = 0x50F14000;
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 0xC } );
	asc->set_read_data( ASC_REG::VERSION, { 0xBB } );
	if( rom ) {
		test_rom( 0x408BD494, 0x40845E1E );
	} else {
		ROMWrapper::run_BD494();
	}
	BOOST_CHECK_EQUAL( DR(0) ,  0xC );
	BOOST_CHECK( cpu.Z  );
	BOOST_CHECK_EQUAL( cpu.PC ,  0x40845E1E );
}

BOOST_AUTO_TEST_CASE( ardbeg_ng ) {
	prepare<Sonora>();
//	prepare<Ardbeg>();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbb);
	machine->asc = asc;
	AR(6) = 0x50F14000;
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 0xB, 5 } );
	asc->set_read_data( ASC_REG::VERSION, { 0xBB } );
	if( rom ) {
		test_rom( 0x408BD494, 0x40845E18 );
	} else {
		ROMWrapper::run_BD494();
	}
	BOOST_CHECK_EQUAL( DR(0) ,  1 );
	BOOST_CHECK_EQUAL( cpu.PC ,  0x40845E18 );
}

BOOST_AUTO_TEST_CASE( glu ) {
	prepare<GLU>();
	auto asc = std::make_shared<IO_TEST<ASC>>(0x80);
	machine->asc = asc;
	AR(6) = 0x50F14000;
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 0x5 } );
	asc->set_read_data( ASC_REG::VERSION, { 0x80 } );
	if( rom ) {
		test_rom( 0x408BD494, 0x40845E18 );
	} else {
		ROMWrapper::run_BD494();
	}
	BOOST_CHECK_EQUAL( DR(0) ,  0x1 );
	BOOST_CHECK_EQUAL( cpu.PC ,  0x40845E18 );
}




