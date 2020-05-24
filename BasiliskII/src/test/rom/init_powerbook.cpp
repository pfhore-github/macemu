#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "msc.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "via_test.hpp"
#include <memory>
using namespace ROM;
struct MockMSC : public MSC {
	std::vector<uint8_t> executed_cmd;
	uint8_t cmd(uint8_t c) override {
		executed_cmd.push_back( c );
		return 0;
	}
};
BOOST_AUTO_TEST_CASE( non_pb ) {
	HWCFG_FLAGS.reset( HWCFG_T::POWERBOOK );
	if( rom ) {
		test_rom( 0x4088AC00, 0x4088AEB0 );
	} else {
		init_powerbook();
	}
}

BOOST_AUTO_TEST_CASE( pb_has_lowmem ) {
	rom_routines[ 0x4000 ] = []() { cpu.PC = AR(6); };
	rom_routines[ 0xB29C4 ] = []() {  };
	rom_routines[ 0x2F64 ] =
		[]() {			
			model = &old_machines[12]; // PB 180
			motherboard = model->motherboard;
			INIT_HW_FLG = model->init_flg;
			HWCFG_FLAGS = model->hwcfg;
			GID = model->gestalt;
			GEN = model->generation;
		};

	rom_routines[ 0x4B53C ] = []() {  };
	HWCFG_FLAGS.set( HWCFG_T::POWERBOOK );
	write_l(0xD18, 0x3000 );
	write_l(0x3112, 0x53415241 );
	write_l(0x3012, 0x4000 );
	if( rom ) {
		test_rom( 0x4088AC00, 0x4088AC56 );
	} else {
		init_powerbook();
	}
}
