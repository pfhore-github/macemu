#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

#include "oss.hpp"
namespace ROM {
void getHWInfo(uint8_t v) { MOCK::invoke<void>("getHWInfo", v); }
uint32_t remap_rom(uint32_t d) { return MOCK::invoke<uint32_t>("remap_rom", d); }
void init_hw2(uint32_t v) { MOCK::invoke<void>("init_hw2", v); }
void init_hw3() { MOCK::invoke<void>("init_hw3"); }
}
void prepare(MB_TYPE m) {
	fixture f(m);
	DEFINE_ROM( 02F64 );		// getHWinfo
	DEFINE_ROM( 04866 );		// init_hw3
	DEFINE_ROM( 04A5E );		// remap_rom
	DEFINE_ROM( 04AFE );		// init_hw2
	MOCK::reset_all();
	MOCK::make<void(uint8_t)>("getHWInfo")->always( 0 ) ;
	MOCK::make<uint32_t(uint32_t)>("remap_rom")->always(0, 0x40800000 );
	MOCK::make<void(uint32_t)>("init_hw2")->always( 0 );
	MOCK::make<void()>("init_hw3")->always_do([]() {});
	clear_global();
}
using namespace ROM;
BOOST_AUTO_TEST_CASE( exec ) {
	prepare(MB_TYPE::MCU);
	MOCK::get<void(uint8_t)>( "getHWInfo" )
		->always_do(
			[](uint8_t v) {
				BOOST_CHECK_EQUAL( v, 0 );
				// Quadra 950
				model = &old_machines[10];	
				motherboard = model->motherboard;
				INIT_HW_FLG = model->init_flg;
				ROM_FLG = model->rom_flg;			
				HWCFG_FLAGS = GET_D2();
			} );
	
	TEST_ROM( 02E00 );
	MOCK::verify( "getHWInfo" );
	MOCK::verify( "remap_rom" );
	MOCK::verify( "init_hw2" );
	MOCK::verify( "init_hw3" );
	BOOST_CHECK_EQUAL( cpu.VBR ,  0x40803DD8 );
	BOOST_CHECK_EQUAL( AR(1) ,  0x40803910 );
	BOOST_CHECK_EQUAL( AR(0) ,  0x40803564 );
	BOOST_CHECK_EQUAL( DR(0) ,  0x07A31807);
	BOOST_CHECK_EQUAL( DR(1) ,  0x43924 );
}
void getHWInfo_iifx(bool d) {
	// IIfx
	model = &old_machines[5];	
	motherboard = model->motherboard;
	INIT_HW_FLG = motherboard->init_flg;
	ROM_FLG = motherboard->rom_flg;
				
	HWCFG_FLAGS = GET_D2();
	INIT_HW_FLG[ INIT_HW_FLG_T::IIFX_EXP0 ] = d;
}
BOOST_AUTO_TEST_CASE( oss ) {
	prepare(MB_TYPE::OSS);
	
	MOCK::get<void(uint8_t)>( "getHWInfo" )
		->always_do( [](uint8_t v) {
						 BOOST_CHECK_EQUAL( v, 0 );
						 getHWInfo_iifx(false); }
			);
	auto oss = std::make_shared<IO_TEST<OSS_REG>>();
	machine->oss = oss;
	TEST_ROM( 02E00 );
	oss->verify(0x14, { 0xD } );
}

BOOST_AUTO_TEST_CASE( exp0 ) {
	prepare(MB_TYPE::OSS);
	MOCK::get<void(uint8_t)>( "getHWInfo" )
		->always_do( [](uint8_t v) {
						 BOOST_CHECK_EQUAL( v, 0 );
						 getHWInfo_iifx(true);} );
	auto iifx = std::make_shared<IO_TEST<IIFX_exp0>>();
	machine->exp0 = iifx;
	TEST_ROM( 02E00 );
	iifx->verify( 0, { 0xff, 0xff, 0xff, 0x7f,
					   0x3f, 0x9f, 0xcf, 0xe7,
					   0xf3, 0x79, 0x3c, 0x1e,
					   0xf, 0x7, 0x3, 0x1 });
	iifx->verify( 0x10, { 0 } );
}

