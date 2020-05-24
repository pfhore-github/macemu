#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "asc.hpp"
using namespace ROM;
void prepare() {
	fixture f(MB_TYPE::SONORA);
	rom_base = 0x40800000;
	AR(6) = 0x50F14000;
}
BOOST_AUTO_TEST_CASE( sonora_ok ) {
	prepare();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbc);
	machine->asc = asc;
	asc->set_read_data( ASC_REG::VERSION, { 0xbc } );
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 8 } );
	TEST_ROM2( BD3EC, 45F3A );
}

BOOST_AUTO_TEST_CASE( sonora_ng ) {
	prepare();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbc);
	asc->set_read_data( ASC_REG::VERSION, { 0xbc } );
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 0 } );
	TEST_ROM2( BD3EC, 45EAC );
}


BOOST_AUTO_TEST_CASE( ardbeg_ok ) {
	prepare();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbb);
	machine->asc = asc;
	asc->set_read_data( ASC_REG::VERSION, { 0xbb } );
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 8 } );
	TEST_ROM2( BD3EC, 45F3A );
}

BOOST_AUTO_TEST_CASE( ardbeg_ng ) {
	prepare();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbb);
	machine->asc = asc;
	asc->set_read_data( ASC_REG::VERSION, { 0xbb } );
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 0 } );
	TEST_ROM2( BD3EC, 45EAC );
}

BOOST_AUTO_TEST_CASE( other_ok ) {
	prepare();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xbb);
	machine->asc = asc;
	asc->set_read_data( ASC_REG::VERSION, { 0xb0 } );
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 2 } );
	TEST_ROM2( BD3EC, 45F36 );
	BOOST_CHECK( ! cpu.Z  );
}

BOOST_AUTO_TEST_CASE( other_ng ) {
	prepare();
	auto asc = std::make_shared<IO_TEST<ASC>>(0xb0);
	machine->asc = asc;
	asc->set_read_data( ASC_REG::VERSION, { 0xb0 } );
	asc->set_read_data( ASC_REG::FIFO_IRQ_STATUS, { 0 } );
	TEST_ROM2( BD3EC, 45F36 );
	BOOST_CHECK( cpu.Z );
}


