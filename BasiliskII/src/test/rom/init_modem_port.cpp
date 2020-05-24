#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "machine.hpp"
#include "scc.hpp"
#include "glu.hpp"
#include "scc_impl.hpp"
#include <memory>
using namespace ROM;
static bool fw_load = false; 
namespace ROM {
uint16_t init_scc_iop_fw() { fw_load = true; return 0; }
}
void prepare(MB_TYPE m) {
	fixture f(m);
}
BOOST_AUTO_TEST_CASE( glu ) {
	prepare(MB_TYPE::GLU);
	AR(6) = 0;
	DR(3) = 0 ;
	AR(2) = 0x50F00000 ;
	AR(3) = 0x50F04000 ;
	auto scc_mock = std::make_shared<IO_TEST<SCC>>(std::make_shared<Z8530>());
	rom_routines[ 0x47EF6 ] = []() { fw_load = true; };
	scc_mock->set_read_data(SCC_REG::B_CMD, { 0 } );
 	scc_mock->set_read_data(SCC_REG::A_DATA, { 0 } );
	machine->scc = scc_mock;
	if( rom ) {		
		test_rom( 0x408B9F78 );
	} else {
		ROMWrapper::run_B9F78();
	}
	scc_mock->verify( SCC_REG::A_CMD, {
			0x09, 0xC0, 0x0F, 0x00, 0x04, 0x4C, 0x0B, 0x50, 0x0E, 0x00,
			0x0C, 0x0A, 0x0D, 0x00, 0x0E, 0x01, 0x0A, 0x00, 0x03, 0xC1,
			0x05, 0xEA, 0x01, 0x00 } );
}

BOOST_AUTO_TEST_CASE( iop ) {
	prepare(MB_TYPE::OSS);
	AR(6) = 0;
	AR(2) = 0x50F00000 ;
	AR(3) = 0x50F04020 ;
	DR(3) = 0 ;
	INIT_HW_FLG.set( INIT_HW_FLG_T::IOP_SCC );
	auto scc_mock = std::make_shared<IO_TEST<SCC_IOP>>(std::make_shared<Z85C80>());
	rom_routines[ 0x47EF6 ] = []() { fw_load = true; };
	scc_mock->set_read_data(0x10+SCC_REG::B_CMD, { 0 } );
	scc_mock->set_read_data(0x10+SCC_REG::A_DATA, { 0 } );
	scc_mock->set_read_data(4, { 0 } );
	machine->scc = scc_mock;
	if( rom ) {		
		test_rom( 0x408B9F78 );
	} else {
		ROMWrapper::run_B9F78();
	}
	scc_mock->verify( 0x10+SCC_REG::A_CMD, {
			0x09, 0xC0, 0x0F, 0x00, 0x04, 0x4C, 0x0B, 0x50, 0x0E, 0x00,
			0x0C, 0x0A, 0x0D, 0x00, 0x0E, 0x01, 0x0A, 0x00, 0x03, 0xC1,
			0x05, 0xEA, 0x01, 0x00 } );
}

