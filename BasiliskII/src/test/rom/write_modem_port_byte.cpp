#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "scc.hpp"
#include "scc_impl.hpp"
using namespace ROM;
namespace ROM {
}
void prepare() {
	fixture f;
	rom_base = 0x40800000;
}
BOOST_AUTO_TEST_CASE( exec) {
	prepare();
	motherboard = &motherboards[ 0 ];
	auto scc = std::make_shared<IO_TEST<SCC>>(std::make_shared<Z8530>());
	scc->set_read_data(SCC_REG::A_CMD, { 0, 1 } );
	machine->scc = scc;
	AR(3) = 0x50F04000;
	DR(3) = 0;
	DR(0) = 0x1f;
	if( rom ) {
		test_rom( 0x408B9FDA );
	} else {
		ROMWrapper::run_B9FDA();
	}
	scc->verify( SCC_REG::A_CMD, { 0x30, 1, 1 } );
	scc->verify( SCC_REG::A_DATA, { 0x1f } );
}

