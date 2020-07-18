#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "rbv.hpp"
#include "mcu.hpp"
#include "jaws.hpp"
#include "memcjr.hpp"
#include "sonora.hpp"
#include "f108.hpp"
#include <memory>
namespace bdata = boost::unit_test::data;
extern uint32_t mac_model_id;
using namespace ROM;

void prepare(std::unique_ptr<Machine>&& m) {
	clear_global();
	fixture f(std::move(m));
	set_sr( 0x2700 );
	DR(2) = 0;
	ROMWrapper::run_04A5E();
}
BOOST_AUTO_TEST_CASE( rbv_lciii_p )  {
	prepare(std::make_unique<Sonora>(Sonora::MODEL::LCIII_p));
	auto rbv = std::make_shared<IO_TEST<SonorRBV>>();
	machine->rbv = rbv;
	rbv->watch(RBV_REG::EXP);
	rbv->watch(RBV_REG::RBV_4);
	rbv->watch(RBV_REG::RBV_5);
	TEST_ROM( 04866 );
	rbv->verify(RBV_REG::EXP, { 0x40 });
	rbv->verify(RBV_REG::RBV_4, { 0 });
	rbv->verify(RBV_REG::RBV_5, { 3 });

}
BOOST_AUTO_TEST_CASE( rbv_lciii )  {
	prepare(std::make_unique<Sonora>(Sonora::MODEL::LCIII));
	machine->model_id = 0xA55A0002;
	auto rbv = std::make_shared<IO_TEST<SonorRBV>>();
	machine->rbv = rbv;
	rbv->watch(RBV_REG::EXP);
	rbv->watch(RBV_REG::RBV_4);
	rbv->watch(RBV_REG::RBV_5);
	TEST_ROM( 04866 );
	rbv->verify(RBV_REG::EXP, { 0x40 });
	rbv->verify(RBV_REG::RBV_4, { 0 });
	rbv->verify(RBV_REG::RBV_5, { 3, 2 });
}

BOOST_DATA_TEST_CASE( mcu, bdata::xrange(2), tf )  {
	prepare(std::make_unique<MCU>());   
	auto via2 = std::make_shared<IO_TEST<VIA2>>();
	via2->set_read_data(0, { uint8_t(tf ? 0x20 : 0) } );
	via2->set_read_data(2, { 0 } );
	auto mcu = std::make_shared<IO_TEST<MCU_REG>>();
	mcu->set_read_data(0x48, { 0 } );
	mcu->set_read_data(0x49, { 0 } );
	mcu->set_read_data(0x4A, { 0 } );
	mcu->set_read_data(0x4B, { 0 } );
	machine->via2 = via2;
	machine->mcu = mcu;
	for(int i = 0; i < 0xbc ; ++i ) {
		mcu->watch( i );
	}
	TEST_ROM( 04866 );
	uint32_t val = tf ? 0x138b0810 : 0x124f0810;
	for(int i = 0; i < 32 ; ++i ) {
		uint32_t x = val >> i;
		mcu->verify32( i*4, { x });
	}
	for( int i = 0x80; i < 0x87; ++i ) {
		mcu->verify( i , { 0 });
	}
	mcu->verify( 0x87, { 1 });
	for(int i = 0xa0; i < 0xbc; i += 4 ) { 
		mcu->verify32( i, { 0xffff });
	}
}


BOOST_AUTO_TEST_CASE( jaws )  {
	prepare(std::make_unique<JAWS>());

	machine->model_id = 0xA55A0002;
	auto pb = std::make_shared<IO_TEST<JAWS_REG>>();
	auto pb2 = std::make_shared<IO_TEST<JAWS_REG2>>();
	pb2->set_read_data(0, { 0xffU });
	pb->watch( 0 );
	machine->pb = pb;
	machine->pb2 = pb2;
	TEST_ROM( 04866 );
	pb->verify( 0, { 1 } );
}

BOOST_AUTO_TEST_SUITE( memcjr )

BOOST_AUTO_TEST_CASE( quadra605_20Mhz )  {
	prepare(std::make_unique<MEMCjr>( MEMCjr::MODEL::Q605_20));
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	machine->reg1 = k1;
	machine->reg2 = k2;
	for(int i = 0x30; i < 0x38; ++i ) {
		k1->watch( i );
	}
	for(int i = 0x7c; i < 0x80; ++i ) {
		k1->watch( i );
	}
	k2->watch( 0 );
	k2->watch( 0x600 );
	k2->watch( 0x601 );
	TEST_ROM( 04866 );
	k1->verify32(0x30, { 0xA55A0101 } );
	k1->verify32(0x34, { 0xA55A011D });
	k1->verify32(0x7C, { 0x06956804, 0x76956804 });
	k2->verify( 0, { 1 } );
	k2->verify16( 0x600, { 0x0280 } );
}



BOOST_AUTO_TEST_CASE( quadra605_25Mhz )  {
	prepare(std::make_unique<MEMCjr>( MEMCjr::MODEL::Q605_25));
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	machine->reg1 = k1;
	machine->reg2 = k2;
	for(int i = 0x30; i < 0x38; ++i ) {
		k1->watch( i );
	}
	for(int i = 0x7c; i < 0x80; ++i ) {
		k1->watch( i );
	}
	k2->watch( 0 );
	k2->watch( 0x600 );
	k2->watch( 0x601 );
	TEST_ROM( 04866 );
	k1->verify32( 0x30, { 0xA55A001A } );
	k1->verify32(0x34, { 0xA55A016B });
	k1->verify32(0x7C, { 0x6A956800, 0xAE956805 });
	k2->verify( 0, { 1 } );
	k2->verify16(0x600, { 0x01E0 });
}

BOOST_AUTO_TEST_CASE( quadra605_33Mhz )  {
	prepare(std::make_unique<MEMCjr>( MEMCjr::MODEL::Q605_33));
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	machine->reg1 = k1;
	machine->reg2 = k2;
	for(int i = 0x30; i < 0x38; ++i ) {
		k1->watch( i );
	}
	for(int i = 0x7c; i < 0x80; ++i ) {
		k1->watch( i );
	}
	k2->watch( 0 );
	k2->watch( 0x600 );
	k2->watch( 0x601 );
	TEST_ROM( 04866 );
	k1->verify32(0x30, { 0xA55A00DC });
	k1->verify32(0x34, { 0xA55A01E7 });
	k1->verify32(0x7C, { 0x72956803, 0x9E956807 });
	k2->verify( 0, { 0 } );
	k2->verify16(0x600, { 0x00D5 });
}

BOOST_AUTO_TEST_CASE( unreleased )  {
	prepare(std::make_unique<MEMCjr>(MEMCjr::MODEL::UNUSED1));
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	k2->set_read_data( 0, { 1 } );
	k2->set_read_data( 0x100, { 0 } );
	machine->reg1 = k1;
	machine->reg2 = k2;
	k2->watch( 0 );
	k2->watch( 0x100 );
	k2->watch( 0x600 );
	k2->watch( 0x601 );
	TEST_ROM( 04866 );
	k2->verify( 0, { 1, 0x11 } );
	k2->verify( 0x100, { 0x08 } );
	k2->verify16( 0x600, { 0x01E0 } );
}

struct DJ_MEMCR {
	MEMCjr::MODEL model;
	uint32_t v_30;
	uint32_t v_34;
	uint32_t v_0;
	uint32_t v_600;
};
std::ostream& operator<<(std::ostream& os, const DJ_MEMCR& i) {
	switch(i.model) {
	case MEMCjr::MODEL::UNUSED3 : return os << "unused 650"; 
	case MEMCjr::MODEL::C650 : return os << "Centris 650"; 
	case MEMCjr::MODEL::Q800 : return os << "Quadra 800"; 
	case MEMCjr::MODEL::Q650 : return os << "Quadra 650"; 
	case MEMCjr::MODEL::UNUSED4 : return os << "unknown #4";
	case MEMCjr::MODEL::UNUSED5 : return os << "unknown #5";
	case MEMCjr::MODEL::C610 : return os << "Centris 610";
	case MEMCjr::MODEL::Q610 : return os << "Quadra 610";
	case MEMCjr::MODEL::UNUSED6 : return os << "unknown #6";
	case MEMCjr::MODEL::UNUSED7 : return os << "unknown #7";
	default : return os;
	}
}
const std::vector<DJ_MEMCR> djmemc_tbl = {
	{ MEMCjr::MODEL::UNUSED3, 0x101, 0x11D, 1, 0x0280 }, // unused 650
	{ MEMCjr::MODEL::C650, 0x01A, 0x16B, 1, 0x01E0 }, // Centris 650
	{ MEMCjr::MODEL::Q800, 0x0FB, 0x1E7, 0, 0x00D5 }, // Quadra 800
	{ MEMCjr::MODEL::Q650, 0x0FB, 0x1E7, 0, 0x00D5 }, // Quadra 650
	{ MEMCjr::MODEL::UNUSED4, 0x2FC, 0x255, 0, 0x0000 }, // ???
	{ MEMCjr::MODEL::UNUSED5, 0x2FC, 0x255, 0, 0x0000 }, // ???
	{ MEMCjr::MODEL::C610, 0x0101, 0x011D, 1, 0x0280 }, // Centris 610
	{ MEMCjr::MODEL::Q610, 0x001A, 0x016B, 1, 0x01E0 }, // Quadra 610
	{ MEMCjr::MODEL::UNUSED6, 0x01A, 0x16B, 1, 0x01E0 }, // ???
	{ MEMCjr::MODEL::UNUSED7, 0x0FB, 0x1E7, 0, 0x00D5 }, // ???
};
BOOST_DATA_TEST_CASE( djMEMC, djmemc_tbl, model )  {
	clear_global();
	fixture f(std::make_unique<MEMCjr>(model.model));
	set_sr( 0x2700 );
	DR(2) = 0;
	ROMWrapper::run_04A5E();
	
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	k2->set_read_data(0, { 0xab } );
	k2->set_read_data(1, { 0xcd } );
	k2->set_read_data(2, { 0xef } );
	k2->set_read_data(3, { 0x98 } );
	for(int i = 0x30; i < 0x38; ++i ) {
		k1->watch( i );
	}
	for(int i = 0; i < 4; ++i ) {
		k2->watch( i );
		k2->watch( 0x600+i );
	}
	machine->reg1 = k1;
	machine->reg2 = k2;
	if( rom ) {
		run_rom_as_650( 0x40804862 );
	} else {
		ROMWrapper::run_04866();
	}
	k1->verify32(0x30, { model.v_30 });
	k1->verify32(0x34, { model.v_34 });
	k2->verify32(0x0, { 0xabcdef90 | model.v_0 });
	k2->verify32(0x600, { 0xabcd0000 | model.v_600 });
}


BOOST_AUTO_TEST_CASE( centris610)  {
	clear_global();
	fixture f(std::make_unique<MEMCjr>(MEMCjr::MODEL::C610));
	set_sr( 0x2700 );
	DR(2) = 0;
	ROMWrapper::run_04A5E();
	
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	k2->set_read_data(0, { 0xab } );
	k2->set_read_data(1, { 0xcd } );
	k2->set_read_data(2, { 0xef } );
	k2->set_read_data(3, { 0x98 } );
	for(int i = 0x30; i < 0x38; ++i ) {
		k1->watch( i );
	}
	for(int i = 0; i < 4; ++i ) {
		k2->watch( i );
		k2->watch( 0x600+i );
	}
	machine->reg1 = k1;
	machine->reg2 = k2;
	if( rom ) {
		run_rom_as_650( 0x40804862 );
	} else {
		ROMWrapper::run_04866();
	}
	k1->verify32(0x30, { 0x00000101 });
	k1->verify32(0x34, { 0x0000011d });
	k2->verify32(0x0, { 0xabcdef91 });
	k2->verify32(0x600, { 0xabcd0280 });
}

BOOST_AUTO_TEST_SUITE_END(  );
BOOST_AUTO_TEST_SUITE( f108 )

BOOST_AUTO_TEST_CASE( LC630_2)  {
	prepare(std::make_unique<F108>(F108::MODEL::LC630_2));
	auto k1 = std::make_shared<IO_TEST<MEMCjr_REG1>>();
	auto k2 = std::make_shared<IO_TEST<MEMCjr_REG2>>();
	machine->reg1 = k1;
	machine->reg2 = k2;
	for(int i = 0x30; i < 0x38; ++i ) {
		k1->watch( i );
	}
	for(int i = 0x7c; i < 0x80; ++i ) {
		k1->watch( i );
	}
	k2->watch( 0 );
	k2->watch( 0x500 );
	k2->watch( 0x501 );
	k2->watch( 0x600 );
	k2->watch( 0x601 );
	TEST_ROM( 04866 );
	k1->verify32(0x30, { 0xA55A02DC });
	k1->verify32(0x34, { 0xA55A0255 });
	k1->verify32(0x7C, { 0x7295680B, 0x56956809 });
	k2->verify( 0, { 0 } );
	k2->verify16(0x500, { 0x90B7 });
	k2->verify16(0x600, { 0x0000 });
}

BOOST_AUTO_TEST_SUITE_END(  );
