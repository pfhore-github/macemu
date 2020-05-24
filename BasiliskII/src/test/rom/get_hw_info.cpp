#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"

using namespace ROM;
namespace bdata = boost::unit_test::data;
namespace ROM {
void test_scsi() { MOCK::invoke<void>("test_scsi"); }
bool has_rbv(uint32_t v) { return MOCK::invoke<bool>("has_rbv", v); }
bool has_via(uint32_t v1, uint32_t v2) { return MOCK::invoke<bool>("has_via", v1, v2); }
bool test_iop(uint32_t v) { return MOCK::invoke<bool>("test_iop", v); }
bool test_scsi_io(uint32_t v) { return MOCK::invoke<bool>("test_scsi_io", v); }
bool test_model_id() { return MOCK::invoke<bool>("test_model_id"); }
void check_flag() { return MOCK::invoke<void>("check_flag"); }
uint32_t read_id_from_via() { return MOCK::invoke<uint32_t>("read_id_from_via"); }
void run_rom(uint32_t) {}
bool check_glu() { return MOCK::invoke<bool>("check_glu"); }
bool check_mdu() { return MOCK::invoke<bool>("check_mdu"); }
bool check_oss() { return MOCK::invoke<bool>("check_oss"); }
bool check_V8() { return MOCK::invoke<bool>("check_V8"); }
bool check_mcu() { return MOCK::invoke<bool>("check_mcu"); }
bool check_jaws() { return MOCK::invoke<bool>("check_jaws"); }
bool check_niagara() { return MOCK::invoke<bool>("check_niagara"); }
bool check_null() { return MOCK::invoke<bool>("check_null"); }
}

void prepare(MB_TYPE m = MB_TYPE::GLU) {
	clear_global();
	fixture f(m);
	set_sr( 0x2700 );
	DEFINE_ROM( 030AC );		// check_glu
	DEFINE_ROM( 030CA );		// check_mdu
	DEFINE_ROM( 0310A );		// check_oss
	DEFINE_ROM( 03166 );		// check_V8
	DEFINE_ROM( 031A0 );		// check_mcu
	DEFINE_ROM( 031AE );		// check_jaws
	DEFINE_ROM( 031F2 );		// check_niagara
	DEFINE_ROM( 0323A );		// check_null
	DEFINE_ROM( 046AA );		// has_via
	DEFINE_ROM( 046D8 );		// has_rbv
	DEFINE_ROM( 04700 );		// test_scsi_io
	DEFINE_ROM( 0477C );		// test_iop
	DEFINE_ROM( 047CC );		// read_id_from_via
	DEFINE_ROM( 04A96 );		// test_scsi
	DEFINE_ROM( 04B0C );		// check_flag
	DEFINE_ROM( 04B6C );		// test_model_id
	MOCK::reset_all();
	MOCK::make<void()>("test_scsi")->always_do_nothing( );
	MOCK::make<bool(uint32_t)>("has_rbv")->always( false );
	MOCK::make<bool(uint32_t, uint32_t)>("has_via")->always( false );
	MOCK::make<bool(uint32_t)>("test_iop")->always( false );
	MOCK::make<bool(uint32_t)>("test_scsi_io")->always( false );
	MOCK::make<bool()>("test_model_id")->always( false );
	MOCK::make<void()>("check_flag")->always_do_nothing( );
	MOCK::make<uint32_t()>("read_id_from_via")->always( 0 );
	MOCK::make<bool()>("check_glu")->always( false );
	MOCK::make<bool()>("check_mdu")->always( false );
	MOCK::make<bool()>("check_oss")->always( false );
	MOCK::make<bool()>("check_V8")->always( false );
	MOCK::make<bool()>("check_mcu")->always( false );
	MOCK::make<bool()>("check_jaws")->always( false );
	MOCK::make<bool()>("check_niagara")->always( false );
	MOCK::make<bool()>("check_null")->always( false );
	AR(4) = 0;
}
void compare(const MACHINE_TABLE_T* mt) {
	BOOST_CHECK_EQUAL( AR(0) , rom_base | mt->motherboard->BASE );
	BOOST_CHECK_EQUAL( AR(1) , rom_base | mt->BASE );
	if( mt->init_flg.any() ) {
		BOOST_CHECK_EQUAL( DR(0) ,  mt->init_flg.to_ulong() );
	} 
	if( mt->rom_flg.any() ) {
		BOOST_CHECK_EQUAL( DR(1) ,  mt->rom_flg.to_ulong() );
	} else {
		BOOST_CHECK_EQUAL( DR(1) ,  mt->motherboard->rom_flg.to_ulong() );
	}
	BOOST_CHECK_EQUAL( DR(2) ,  mt->hwcfg.to_ulong() << 16 | mt->gestalt_machine_id() );
	BOOST_CHECK( ! ( DR(7) & 1 << INIT_FLAG_T::FAULT_SKIP ) );
}

BOOST_DATA_TEST_CASE( newer, bdata::xrange( 26 ), id )  {
	prepare();
	MOCK::get<bool()>("test_model_id")
		->always_do ( 
			[ id ]() {
				motherboard =  hw_test_table[0];
				model = &new_machines[id];
				return true;
			} );
	TEST_ROM( 02F64 );
	compare( &new_machines[id] );
}

BOOST_DATA_TEST_CASE( IIx, bdata::xrange( 3 ), id )  {
	prepare();
	DR(2) = 0;
	MOCK::get<bool()>("test_model_id")->always ( false );
	MOCK::get<uint32_t()>( "read_id_from_via" )->always( old_machines[id].model_id );
	MOCK::get<bool()>("check_glu")->always( true );
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_DATA_TEST_CASE( IIci, bdata::xrange( 3, 7 ), id )  {
	if( id == 5 ) {
		return;
	}
	prepare(MB_TYPE::MDU);
	DR(2) = 0;
	MOCK::get<uint32_t()>( "read_id_from_via" )->always( old_machines[id].model_id );
	MOCK::get<bool()>("check_mdu")->always( true );
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_AUTO_TEST_CASE( IIfx )  {
	int id = 5;
	prepare(MB_TYPE::OSS);
	MOCK::get<uint32_t()>( "read_id_from_via" )->always( old_machines[id].model_id );
	MOCK::get<bool()>("check_oss")->always( true );
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_AUTO_TEST_CASE( LC )  {
	int id = 7;
	prepare(MB_TYPE::V8);
	MOCK::get<uint32_t()>( "read_id_from_via" )->always( old_machines[id].model_id );
	MOCK::get<bool()>("check_V8")->always( true );
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_DATA_TEST_CASE( Quadra, bdata::xrange( 8, 11 ), id )  {
	prepare(MB_TYPE::MCU);
	MOCK::get<uint32_t()>( "read_id_from_via" )->always( old_machines[id].model_id );
	MOCK::get<bool()>("check_mcu")->always( true );
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_AUTO_TEST_CASE( PB170 )  {
	int id = 11;
	prepare(MB_TYPE::JAWS);
	MOCK::get<uint32_t()>( "read_id_from_via" )->always( old_machines[id].model_id );
	MOCK::get<bool()>("check_jaws")->always( true );
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}
BOOST_AUTO_TEST_CASE( PB180 )  {
	int id = 12;
	prepare(MB_TYPE::NIAGARA);
	MOCK::get<uint32_t()>( "read_id_from_via" )->always( old_machines[id].model_id );
	MOCK::get<bool()>("check_niagara")->always( true );
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_DATA_TEST_CASE( generic, bdata::xrange(14, 18), id ) {
	prepare();
	MOCK::get<bool()>("check_mdu")->always( id == 14 );
	MOCK::get<bool()>("check_oss")->always( id == 15 );
	MOCK::get<bool()>("check_V8")->always( id == 16 );
	MOCK::get<bool()>("check_jaws")->always( id == 17 );
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_DATA_TEST_CASE( generic_rbv, bdata::xrange(2), rbv ) {
	prepare();
	MOCK::get<bool()>("check_mdu")
		->always_do(
			[]() {
				INIT_HW_FLG.set( INIT_HW_FLG_T::RBV );
				return true;
			});
	MOCK::get<bool(uint32_t)>("has_rbv")->always( rbv, 0x50f26013u );
	TEST_ROM( 02F64 );
	BOOST_CHECK_EQUAL( DR(0) >> INIT_HW_FLG_T::RBV & 1 ,rbv );
	BOOST_CHECK_EQUAL( DR(0) >> INIT_HW_FLG_T::VDAC & 1 ,rbv );
}

BOOST_DATA_TEST_CASE( generic_iop_swim, bdata::xrange(2), v ) {
	prepare();
	MOCK::get<bool()>("check_mdu")
		->always_do(
			[]() {
				INIT_HW_FLG.set( INIT_HW_FLG_T::IOP_SWIM );
				INIT_HW_FLG.set( INIT_HW_FLG_T::NO_ISM );
				return true;
			});
	MOCK::get<bool(uint32_t)>("test_iop")->nth_is( 1, v, 0x50f1e020u );
	TEST_ROM ( 02F64 );
	BOOST_CHECK_EQUAL( DR(0) >> INIT_HW_FLG_T::IOP_SWIM & 1 , v );
	BOOST_CHECK_NE( DR(0) >> INIT_HW_FLG_T::NO_ISM & 1, v );
}

BOOST_DATA_TEST_CASE( generic_iop_scc, bdata::xrange(2), v ) {
	prepare();
	MOCK::get<bool()>("check_mdu")
		->always_do(
			[]() {
				INIT_HW_FLG.set( INIT_HW_FLG_T::IOP_SCC );
				INIT_HW_FLG.set( INIT_HW_FLG_T::IIFX_1 );
				INIT_HW_FLG.set( INIT_HW_FLG_T::IIFX_2 );
				return true;
			});
	MOCK::get<bool(uint32_t)>("test_iop")->nth_is(2, v, 0x50f0c020u );
	TEST_ROM ( 02F64 );
	BOOST_CHECK_EQUAL( DR(0) >> INIT_HW_FLG_T::IOP_SCC & 1 , v );
	BOOST_CHECK_NE( DR(0) >> INIT_HW_FLG_T::IIFX_1 & 1, v );
	BOOST_CHECK_NE( DR(0) >> INIT_HW_FLG_T::IIFX_2 & 1, v );
}

BOOST_DATA_TEST_CASE( generic_scsi_dma, bdata::xrange(2), v ) {
	prepare();
	MOCK::get<bool()>("check_mdu")
		->always_do(
			[]() {
				INIT_HW_FLG.set( INIT_HW_FLG_T::SCSI_DMA);
				INIT_HW_FLG.set( 8 );
				INIT_HW_FLG.set( 9 );
				INIT_HW_FLG.set( 10 );
				return true;
			});
	MOCK::get<bool(uint32_t)>("test_scsi_io")->always( v, 0x50f18000u );
	TEST_ROM ( 02F64 );
	BOOST_CHECK_EQUAL( DR(0) >> INIT_HW_FLG_T::SCSI_DMA & 1, v );
	BOOST_CHECK_NE( DR(0) >> 8 & 1, v );
	BOOST_CHECK_NE( DR(0) >> 9 & 1, v );
	BOOST_CHECK_NE( DR(0) >> 10 & 1, v );
}
