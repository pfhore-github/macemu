#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

namespace ROM {
bool test_scsi_cmd(uint32_t a) { return MOCK::invoke<bool>("test_scsi_cmd", a); }
bool test_iifx_exp1(uint32_t b) { return MOCK::invoke<bool>("test_iifx_exp1", b); }
}
using namespace ROM;
void prepare(MB_TYPE m) {
	fixture f(m);
	set_sr( 0x2700 );
	DEFINE_ROM( 04672 );
	DEFINE_ROM( 04708 );
	MOCK::reset_all();
	MOCK::make<bool(uint32_t)>("test_scsi_cmd");
	MOCK::make<bool(uint32_t)>("test_iifx_exp1");
	cpu.VBR = 0x40803DD8;
	DR(7) = 1 << ( INIT_FLAG_T::FAULT_SKIP + 16 );
}

BOOST_AUTO_TEST_CASE( none )  {
	prepare(MB_TYPE::GLU);
	AR(1) = 0x40803688;
	DR(0) = 1 << INIT_HW_FLG_T::SCSI_DMA |
		1 << 8 |
		1 << 9 |
		1 << 10 |
		1 << INIT_HW_FLG_T::SCSI_040_EXT;
	TEST_ROM2( 04A96, 03096 );
	INIT_HW_FLG = DR(0);
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_INT ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::IIFX_EXP1 ) );
	BOOST_CHECK( INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_DMA ) );
	BOOST_CHECK( INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_EXT ) );
	BOOST_CHECK( INIT_HW_FLG.test( 8 ) );
	BOOST_CHECK( INIT_HW_FLG.test( 9 ) );
	BOOST_CHECK( INIT_HW_FLG.test( 10 ) );
}

BOOST_AUTO_TEST_CASE( scsi_040 )  {
	prepare(MB_TYPE::MCU);
	AR(1) = 0x40803910;
	DR(0) |= 1 << INIT_HW_FLG_T::SCSI_040_INT |
		1 << INIT_HW_FLG_T::SCSI_DMA |
		1 << 8 |
		1 << 9 |
		1 << 10;
	MOCK::get<bool(uint32_t)>("test_scsi_cmd")->always( true, 0x50F0F000 );
	TEST_ROM2( 04A96, 03096 );
	INIT_HW_FLG = DR(0);
	BOOST_CHECK( INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_INT ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::IIFX_EXP1 ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_DMA ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_EXT ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( 8 ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( 9 ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( 10 ) );
}

BOOST_AUTO_TEST_CASE( scsi_040_failure )  {
	prepare(MB_TYPE::MCU);
	AR(1) = 0x40803910;
	DR(0) |= 1 << INIT_HW_FLG_T::SCSI_040_INT |
		1 << INIT_HW_FLG_T::SCSI_DMA |
		1 << 8 |
		1 << 9 |
		1 << 10;
	MOCK::get<bool(uint32_t)>("test_scsi_cmd")->always( false, 0x50F0F000 );
	TEST_ROM2( 04A96, 03096 );
	INIT_HW_FLG = DR(0);
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_INT ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::IIFX_EXP1 ) );
	BOOST_CHECK( INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_DMA ) );
	BOOST_CHECK( INIT_HW_FLG.test( 8 ) );
	BOOST_CHECK( INIT_HW_FLG.test( 9 ) );
	BOOST_CHECK( INIT_HW_FLG.test( 10 ) );
}

BOOST_AUTO_TEST_CASE( scsi_040_2 )  {
	prepare(MB_TYPE::MCU);
	AR(1) = 0x40803910;
	DR(0) |= 1 << INIT_HW_FLG_T::SCSI_040_INT |
		1 << INIT_HW_FLG_T::SCSI_DMA |
		1 << 8 |
		1 << 9 |
		1 << 10 |
		1 << INIT_HW_FLG_T::SCSI_040_EXT ;
	MOCK::get<bool(uint32_t)>("test_scsi_cmd")
		->nth_is(1, true, 0x50F0F000 )
		.nth_is(2, true, 0x50F0F402 );
	TEST_ROM2( 04A96, 03096 );
	INIT_HW_FLG = DR(0);
	BOOST_CHECK( INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_INT ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::IIFX_EXP1 ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_DMA ) );
	BOOST_CHECK( INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_EXT ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( 8 ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( 9 ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( 10 ) );
}

BOOST_AUTO_TEST_CASE( scsi_040_2_failure )  {
	prepare(MB_TYPE::MCU);
	AR(1) = 0x40803910;
	DR(0) |= 1 << INIT_HW_FLG_T::SCSI_040_INT |
		1 << INIT_HW_FLG_T::SCSI_DMA |
		1 << 8 |
		1 << 9 |
		1 << 10 |
		1 << INIT_HW_FLG_T::SCSI_040_EXT ;
	MOCK::get<bool(uint32_t)>("test_scsi_cmd")
		->nth_is(1, true, 0x50F0F000 )
		.nth_is(2, false, 0x50F0F402 );
	TEST_ROM2( 04A96, 03096 );
	INIT_HW_FLG = DR(0);
	BOOST_CHECK( INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_INT ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::IIFX_EXP1 ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_DMA ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_EXT ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( 8 ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( 9 ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( 10 ) );
}

BOOST_AUTO_TEST_CASE( iifx )  {
	prepare(MB_TYPE::OSS);
	AR(1) = 0x408037F0;
	DR(0) = 1 << INIT_HW_FLG_T::IIFX_EXP1 ;
	MOCK::get<bool(uint32_t)>("test_iifx_exp1")->always(true, 0x50F1E000 );
	TEST_ROM2( 04A96, 03096 );
	INIT_HW_FLG = DR(0);
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_INT ) );
	BOOST_CHECK( INIT_HW_FLG.test( INIT_HW_FLG_T::IIFX_EXP1 ) );
}

BOOST_AUTO_TEST_CASE( iifx_failure )  {
	prepare(MB_TYPE::OSS);
	AR(1) = 0x408037F0;
	DR(0) = 1 << INIT_HW_FLG_T::IIFX_EXP1 ;
	MOCK::get<bool(uint32_t)>("test_iifx_exp1")->always(false, 0x50F1E000 );
	TEST_ROM2( 04A96, 03096 );
	INIT_HW_FLG = DR(0);
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::SCSI_040_INT ) );
	BOOST_CHECK( ! INIT_HW_FLG.test( INIT_HW_FLG_T::IIFX_EXP1 ) );
}


