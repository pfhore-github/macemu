#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "glu.hpp"
#include "mcu.hpp"
#include "scc.hpp"
#include "scc_impl.hpp"
using namespace ROM;
namespace ROM {
}
template<class T>
void prepare() {
	fixture f(std::make_unique<T>());
	rom_base = 0x40800000;
}
struct NON_IOP_F {
	std::shared_ptr<IO_TEST<SCC>> scc = std::make_shared<IO_TEST<SCC>>(std::make_shared<Z8530>());
	NON_IOP_F() {
		prepare<GLU>();
		motherboard = &motherboards[ 0 ];
		AR(3) = 0x50F04000;
		DR(3) = 0;
		machine->scc = scc;
	}
};
BOOST_AUTO_TEST_CASE( nothing ) {
	NON_IOP_F f;
	f.scc->set_read_data(SCC_REG::A_CMD, { 0, 1 } );
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_VALID + 16);
	if( rom ) {
		test_rom( 0x408BA0EA );
	} else {
		ROMWrapper::run_BA0EA();
	}
	BOOST_TEST( DR(0) == 0x8000 );
}

BOOST_FIXTURE_TEST_SUITE( non_iop, NON_IOP_F )

BOOST_AUTO_TEST_CASE( not_found) {
	scc->set_read_data(SCC_REG::A_CMD, { 0 } );
	
	if( rom ) {
		test_rom( 0x408BA0EA );
	} else {
		ROMWrapper::run_BA0EA();
	}
	BOOST_TEST( DR(0) == 0x8000 );
}

BOOST_AUTO_TEST_CASE( err) {
	scc->set_read_data(SCC_REG::A_CMD, { 1, 0x70 } );
	scc->set_read_data(SCC_REG::A_DATA, { 0x22 } );
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_VALID +16);
	if( rom ) {
		test_rom( 0x408BA0EA );
	} else {
		ROMWrapper::run_BA0EA();
	}
	scc->verify( SCC_REG::A_CMD, { 1, 0x30 } );
	BOOST_TEST( DR(0) == 0x7022 );
}

BOOST_AUTO_TEST_CASE( ok) {
	scc->set_read_data(SCC_REG::A_CMD, { 1, 0x4 } );
	scc->set_read_data(SCC_REG::A_DATA, { 0x33 } );
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_VALID +16);
	if( rom ) {
		test_rom( 0x408BA0EA );
	} else {
		ROMWrapper::run_BA0EA();
	}
	scc->verify( SCC_REG::A_CMD, { 1 } );
	BOOST_TEST( DR(0) == 0x33 );
}

BOOST_AUTO_TEST_SUITE_END()
struct IOP_F {
	std::shared_ptr<IO_TEST<SCC_IOP>> scc = std::make_shared<IO_TEST<SCC_IOP>>(std::make_shared<Z8530>());
	IOP_F() {
		prepare<MCU>();
		motherboard = &motherboards[ 4 ];
		AR(3) = 0x50F0C020;
		DR(3) = 0;
		machine->scc = scc;
	}
};
BOOST_FIXTURE_TEST_SUITE( iop, IOP_F )

BOOST_AUTO_TEST_CASE( not_found) {
	scc->set_read_data(0x10+SCC_REG::A_CMD, { 0 } );
	if( rom ) {
		test_rom( 0x408BA0EA );
	} else {
		ROMWrapper::run_BA0EA();
	}
	BOOST_TEST( DR(0) == 0x8000 );
}

BOOST_AUTO_TEST_CASE( err) {
	scc->set_read_data(0x10+SCC_REG::A_CMD, { 1, 0x70 } );
	scc->set_read_data(0x10+SCC_REG::A_DATA, { 0x33 } );
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_VALID +16);
	if( rom ) {
		test_rom( 0x408BA0EA );
	} else {
		ROMWrapper::run_BA0EA();
	}
	scc->verify( 0x10 + SCC_REG::A_CMD, { 1, 0x30 } );
	BOOST_TEST( DR(0) == 0x7033 );
}

BOOST_AUTO_TEST_CASE( ok) {
	scc->set_read_data(0x10+SCC_REG::A_CMD, { 1, 0x4 } );
	scc->set_read_data(0x10+SCC_REG::A_DATA, { 0x22 } );
	DR(7) = 1 << ( INIT_FLAG_T::MODEM_PORT_VALID +16);
	if( rom ) {
		test_rom( 0x408BA0EA );
	} else {
		ROMWrapper::run_BA0EA();
	}
	scc->verify( 0x10+SCC_REG::A_CMD, { 1 } );
	BOOST_TEST( DR(0) == 0x22 );
}
BOOST_AUTO_TEST_SUITE_END()

