#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "oss.hpp"
#include "via.hpp"
#include "via_test.hpp"
#include "scc.hpp"
#include "scc_impl.hpp"
#include "iop.hpp"
#include <memory>
using namespace ROM;
void prepare() {
	fixture f( std::make_unique<OSS>() );
	rom_base = 0x40800000;
	motherboard = &motherboards[ 2 ];
}

BOOST_AUTO_TEST_CASE( ok ) {
	prepare();
	AR(6) = 0;
	AR(3) = 0x50F04020 ;
	DR(3) = 0 ;
	auto scc_mock = std::make_shared<IO_TEST<SCC_IOP>>(std::make_shared<Z85C80>());
	scc_mock->set_read_data(IOP_REG::RAM, { 0xa9, 0x81, 0x8d, 0x30, 0xf0, 0xa9, 0x44, 0x8d,
											0x31, 0xf0, 0x80, 0xfe, 0xee, 0x7f, 0xee, 0x7f, 0xee, 0x7f} );
	DR(0) = 0;
	machine->scc = scc_mock;
	if( rom ) {		
		test_rom( 0x40847EF6 );
	} else {
		ROMWrapper::run_47EF6();
	}
	scc_mock->verify( IOP_REG::CTRL, { 0x32, 0x36 });
	scc_mock->verify( IOP_REG::ADDR_HI, { 0x7F, 0x7F });
	scc_mock->verify( IOP_REG::ADDR_LO, { 0xEE, 0xEE });
	scc_mock->verify( IOP_REG::RAM, { 0xa9, 0x81, 0x8d, 0x30, 0xf0, 0xa9, 0x44, 0x8d,
									  0x31, 0xf0, 0x80, 0xfe, 0xee, 0x7f, 0xee, 0x7f, 0xee, 0x7f} );
	BOOST_TEST( DR(0) == 0 );
}

BOOST_AUTO_TEST_CASE( ng ) {
	machine = std::make_unique<OSS>();
	AR(6) = 0;
	AR(2) = 0x50F00000 ;
	AR(3) = 0x50F04020 ;
	DR(3) = 0 ;
	auto scc_mock = std::make_shared<IO_TEST<SCC_IOP>>(std::make_shared<Z85C80>());
	scc_mock->set_read_data(IOP_REG::RAM, { 0xa9, 0x81, 0x80 } );
	DR(0) = 0;
	machine->scc = scc_mock;
	if( rom ) {		
		test_rom( 0x40847EF6 );
	} else {
		ROMWrapper::run_47EF6();
	}
	scc_mock->verify( IOP_REG::CTRL, { 0x32 });
	scc_mock->verify( IOP_REG::ADDR_HI, { 0x7F, 0x7F });
	scc_mock->verify( IOP_REG::ADDR_LO, { 0xEE, 0xEE });
	scc_mock->verify( IOP_REG::RAM, { 0xa9, 0x81, 0x8d, 0x30, 0xf0, 0xa9, 0x44, 0x8d,
									  0x31, 0xf0, 0x80, 0xfe, 0xee, 0x7f, 0xee, 0x7f, 0xee, 0x7f} );
	BOOST_TEST( DR(0) == 0x10 );
}
