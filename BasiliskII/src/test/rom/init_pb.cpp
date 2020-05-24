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
BOOST_FIXTURE_TEST_SUITE(rom_47826, fixture);
BOOST_AUTO_TEST_CASE( success1 ) {
	auto mock = std::make_shared<MockMSC>();
	machine = mock;
	machine->via1->write(VIA_REG::PCR, 0x02);
	machine->via1->write(VIA_REG::ACR, 0x1C);
	rom_routines[ 0x478C2 ] = send_to_xxx;
	AR(0) = 0x408A8234;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f26000;
	DR(4) = 0x0;
	ROM_FLG.set( ROM_FLG_T::MSC );
	if( rom ) {		
		test_rom( 0x40847826 );
	} else {
		send_to_xxx_key();
	}
	BOOST_TEST( cpu.Z );
	std::vector<uint8_t> expected = { 0x10, 0xB3 };
	BOOST_TEST( mock->executed_cmd == expected );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(rom_47840, fixture);

BOOST_AUTO_TEST_SUITE_END();
uint8_t UT_get_JawsVia2(const std::shared_ptr<IO_BASE>& via);
uint8_t UT_get_SR_MSC(const std::shared_ptr<VIA>& via);
BOOST_FIXTURE_TEST_SUITE(rom_478C2, fixture);
BOOST_AUTO_TEST_CASE( non_msc ) {
	init_machine( MB_TYPE::JAWS );
	AR(0) = 0x408032D4;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xff;
	machine->via2->write(VIA_REG::DDRB, 0xbd);
	machine->via2->write(VIA_REG::ORB, 0x66);
	if( rom ) {		
		test_rom( 0x408478C2 );
	} else {
		send_to_xxx() ;
	}
	BOOST_TEST( UT_get_JawsVia2(machine->via2) == 0xff );
	BOOST_TEST( DR(2) == 0 );
	BOOST_TEST( cpu.Z );

}
BOOST_AUTO_TEST_CASE( msc ) {
	
	auto msc = std::make_shared<MockMSC>();
	machine = msc;
	AR(0) = 0x408A8234;
	AR(2) = 0x50f00000;
	AR(3) = 0x50F26000;
	ROM_FLG.set( ROM_FLG_T::MSC );
	AR(5) = 0;
	DR(3) = 0xff;
	if( rom ) {		
		test_rom( 0x408478C2 );
	} else {
		send_to_xxx() ;
	}
	BOOST_TEST( DR(4) & 0x80000000 );
	std::vector<uint8_t> expected = { 0xff };
	BOOST_TEST( msc->executed_cmd  == expected );
	BOOST_TEST( DR(2) == 0 );
	BOOST_TEST( cpu.Z );
}

BOOST_AUTO_TEST_CASE( not_ready ) {
	init_machine( MB_TYPE::JAWS );
	auto via2 = std::make_shared<VIA_TEST>();
	via2->write(VIA_REG::DDRB, 0xbd);
	via2->write(VIA_REG::ORB, 0x66);
	via2->reset_written();
	via2->b[1].set_read_data({ true } );
	via2->b[6].set_read_data({ false } );
	machine->via2 = via2;
	AR(0) = 0x408032D4;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xff;
	if( rom ) {		
		test_rom( 0x408478C2 );
	} else {
		send_to_xxx() ;
	}
	BOOST_TEST( DR(6) == 2 );
	BOOST_TEST( ! cpu.Z );
	via2->b[2].verify( { false, true } );
}

BOOST_AUTO_TEST_CASE( not_ready2 ) {
	init_machine( MB_TYPE::JAWS );
	auto via2 = std::make_shared<VIA_TEST>();
	via2->write(VIA_REG::DDRB, 0xbd);
	via2->write(VIA_REG::ORB, 0x66);
	via2->reset_written();
	
	via2->b[1].set_read_data({ true, false } );
	via2->b[6].set_read_data({ false } );
	machine->via2 = via2;
	AR(0) = 0x408032D4;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xff;
	if( rom ) {		
		test_rom( 0x408478C2 );
	} else {
		send_to_xxx() ;
	}
	BOOST_TEST( ! cpu.Z );
	via2->b[2].verify( { false, true, true } );
}

BOOST_AUTO_TEST_CASE( ready ) {
	init_machine( MB_TYPE::JAWS );
	auto via2 = std::make_shared<VIA_TEST>();
	via2->write(VIA_REG::DDRB, 0xbd);
	via2->write(VIA_REG::ORB, 0x66);
	via2->reset_written();
	
	via2->b[1].set_read_data({ true, false, true } );
	via2->b[6].set_read_data({ false } );
	machine->via2 = via2;
	AR(0) = 0x408032D4;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	DR(3) = 0xff;
	if( rom ) {		
		test_rom( 0x408478C2 );
	} else {
		send_to_xxx() ;
	}
	BOOST_TEST( cpu.Z );
}


BOOST_AUTO_TEST_SUITE_END();
BOOST_FIXTURE_TEST_SUITE(rom_8AC00, fixture);
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
BOOST_AUTO_TEST_SUITE_END();
void UT_get_sawsVia2(const std::shared_ptr<IO_BASE>& via, uint8_t v);
BOOST_FIXTURE_TEST_SUITE(rom_47928, fixture);
BOOST_AUTO_TEST_CASE( msc ) {
	init_machine( MB_TYPE::MSC );
	auto msc = std::dynamic_pointer_cast<MSC>(machine);
	msc->DEBUG_SET_C_OUT(0xab );
	machine->via1->write( VIA_REG::ACR, 0x1c ); 
	machine->via1->write( VIA_REG::PCR, 2 ); 
	ROM_FLG.set( ROM_FLG_T::MSC );
	AR(0) = 0x408A8234;
	AR(2) = 0x50f00000;
	AR(3) = 0;
	AR(5) = 0;
	if( rom ) {		
		test_rom( 0x40847928 );
	} else {
		recv_from_xxx();
	}
	BOOST_TEST( DR(3) == 0xab );
	BOOST_TEST( DR(6) == 0 );
	BOOST_TEST( DR(4) & 0x80000000 );
}

BOOST_AUTO_TEST_CASE( jaws ) {
	init_machine( MB_TYPE::JAWS );
	machine->via1->write( VIA_REG::DDRA, 0x28 ); 
	machine->via1->write( VIA_REG::ORA, 0x21 ); 
	machine->via1->write( VIA_REG::DDRB, 0x87 ); 
	machine->via1->write( VIA_REG::ORB, 0x87 ); 
	machine->via2->write( VIA_REG::DDRA, 0x00 ); 
	machine->via2->write( VIA_REG::ORA, 0x00 ); 
	machine->via2->write( VIA_REG::DDRB, 0xBD ); 
	machine->via2->write( VIA_REG::ORB, 0x66 ); 
	UT_get_sawsVia2( machine->via2, 0xab );
	machine->via2->clear( VIA_REG::ORB, 2);
	AR(0) = 0x408032D4;
	AR(2) = 0x50f00000;
	AR(3) = 0x50f02000;
	AR(5) = 0;
	if( rom ) {		
		test_rom( 0x40847928 );
	} else {
		recv_from_xxx();
	}
	BOOST_TEST( DR(3) == 0xab );
	BOOST_TEST( DR(6) == 0 );
	BOOST_TEST( !( DR(4) & 0x80000000) );
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(rom_B29C4, fixture);
BOOST_AUTO_TEST_CASE( msc ) {
	init_machine( MB_TYPE::MSC );
	rom_routines[ 0x2f64 ] =
		[]() {
			AR(0) = 0x408A8234;
			AR(1) = 0x408A7AC0;
			DR(2) = 0x0A;
			ROM_FLG.set( ROM_FLG_T::MSC );
		};
	rom_routines[ 0x47840] = send_to_xxx_dword;
	rom_routines[ 0x478C2] = send_to_xxx;
	rom_routines[ 0x47928] = recv_from_xxx;
	
	if( rom ) {		
		test_rom( 0x408B29C4 );
	} else {
		init_pb_duo();
	}
}

BOOST_AUTO_TEST_SUITE_END();

BOOST_FIXTURE_TEST_SUITE(rom_B98BC, fixture);
BOOST_AUTO_TEST_CASE( test ) {
	init_machine( MB_TYPE::MSC );
	rom_routines[ 0x47752 ] =
		[]() {
			AR(0) = 0x408A8234;
			AR(1) = 0x408A7AC0;
			DR(2) = 0x0A;
			INIT_FLAGS.set( INIT_FLAG_T::UNK );
		};
	rom_routines[ 0x47826] = []() {};
	rom_routines[ 0xB9F46] = []() {};
	rom_routines[ 0xB9F78] = []() {};
	rom_routines[ 0xBA138] = []() {  };
	rom_routines[ 0xBAA1A] = []() { DR(0) = 0; };
	rom_routines[ 0xBA0EA] = []() { DR(0) = 0x30; };
	
	if( rom ) {		
		test_rom( 0x408B98BC );
	} else {
//		init_pb_duo();
	}
}
BOOST_AUTO_TEST_SUITE_END();
