#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

#include "oss.hpp"
#include "mcu.hpp"
namespace bdata = boost::unit_test::data;

namespace ROM {
void init_hw3() { MOCK::invoke<void>("init_hw3"); }
}

void prepare(std::unique_ptr<Machine>&& m) {
	fixture f(std::move(m));
	DEFINE_ROM( 04866 );		// init_hw3
	MOCK::reset_all();
	MOCK::make<void()>("init_hw3")->always_do([]() {});
	clear_global();
}
using namespace ROM;
BOOST_DATA_TEST_CASE( exec,bdata::xrange(2), begin  ) {
	prepare(std::make_unique<MCU>(MCU::MODEL::Q950));
	if( begin ) {
		rom_base = 0;
		machine->rom_mirror = true;
	}
	if( rom ) {
		if( begin ) {
			test_rom( 0x2e00, 0x40800000 );
		} else {
			test_rom( 0x40802e00 );
		}
	} else { 
		ROMWrapper::run_02E00();
		BOOST_REQUIRE_EQUAL( cpu.PC, 0x40800000 ); 
	}
	MOCK::verify( "init_hw3" );
	BOOST_CHECK_EQUAL( cpu.VBR ,  0x40803DD8 );
	BOOST_CHECK_EQUAL( AR(1) ,  0x40803910 );
	BOOST_CHECK_EQUAL( AR(0) ,  0x40803564 );
	BOOST_CHECK_EQUAL( DR(0) ,  0x07A31807);
	BOOST_CHECK_EQUAL( DR(1) ,  0x43924 );
}


BOOST_AUTO_TEST_CASE( oss ) {
	prepare(std::make_unique<OSS>());
	rom_base = 0x40800000;
	
	auto oss = std::make_shared<IO_TEST<OSS_REG>>();
	machine->oss = oss;
	oss->watch( 0x14 );
	if( rom ) {
		test_rom( 0x40802e00 );
	} else { 
		ROMWrapper::run_02E00();
		BOOST_REQUIRE_EQUAL( cpu.PC, 0x40800000 ); 
	}
	oss->verify(0x14, { 0xD } );
}

BOOST_AUTO_TEST_CASE( exp0 ) {
	prepare(std::make_unique<OSS>());
	rom_base = 0x40800000;
	auto iifx = std::make_shared<IO_TEST<IIFX_exp0>>();
	machine->exp0 = iifx;
	iifx->watch( 0 );
	iifx->watch( 0x10 );
	if( rom ) {
		test_rom( 0x40802e00 );
	} else { 
		ROMWrapper::run_02E00();
		BOOST_REQUIRE_EQUAL( cpu.PC, 0x40800000 ); 
	}
	iifx->verify( 0, { 0xff, 0xff, 0xff, 0x7f,
					   0x3f, 0x9f, 0xcf, 0xe7,
					   0xf3, 0x79, 0x3c, 0x1e,
					   0xf, 0x7, 0x3, 0x1 });
	iifx->verify( 0x10, { 0 } );
}

