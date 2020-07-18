#define BOOST_TEST_DYN_LINK
#include "test_common.hpp"
#include "machine.hpp"
#include "glu.hpp"
#include "mdu.hpp"
#include "mcu.hpp"
#include "oss.hpp"
#include "v8.hpp"
#include "jaws.hpp"
#include "msc.hpp"
#include "sonora.hpp"
#include "memcjr.hpp"
#include "ardbeg.hpp"
#include "f108.hpp"
#include "swim.hpp"
#include "z8530.hpp"
#include "ncr5380.hpp"
using namespace ROM;
namespace bdata = boost::unit_test::data;
namespace ROM {
void run_rom(uint32_t) {}
}

void prepare(std::unique_ptr<Machine>&& m) {
	clear_global();
	fixture f(std::move(m));
	set_sr( 0x2700 );
	MOCK::reset_all();
	cpu.VBR = 0x40803DD8;
	DR(7) |= 1 << 27;
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


BOOST_DATA_TEST_CASE( msc_bw, bdata::xrange( 2 ), id )  {
	prepare(std::make_unique<MSC>(MSC::MODEL(id)));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	BOOST_CHECK_EQUAL( DR(2) & 1 << (int(HWCFG_T::HAS_FPU)+16), 0 );
	DR(2) |= 1 << (int(HWCFG_T::HAS_FPU)+16);
	compare( &new_machines[id] );

}

BOOST_AUTO_TEST_CASE( msc_cr )  {
	prepare(std::make_unique<MSC>(MSC::MODEL::PB_Duo270c));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &new_machines[2] );
}

BOOST_DATA_TEST_CASE( sonora, bdata::xrange( 7 ), id )  {
	prepare(std::make_unique<Sonora>(Sonora::MODEL(id)));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &new_machines[3+id] );
}

BOOST_DATA_TEST_CASE( memcjr, bdata::xrange( 5 ), id )  {
	prepare(std::make_unique<MEMCjr>(MEMCjr::MODEL(id)));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &new_machines[id > 2 ? id+12 : id+10] );
}


BOOST_DATA_TEST_CASE( memjr_extra, bdata::xrange(10), id )  {
	prepare(std::make_unique<MEMCjr>(MEMCjr::MODEL(5+id)));
	DR(2) = 0;
	if( rom ) {
		run_rom_as_650( 0x40802f34 );
	} else {
		ROMWrapper::run_02F64();
	}
	compare( &new_machines[26+id] );
}


BOOST_DATA_TEST_CASE( ardbeg, bdata::xrange( 2 ), id )  {
	prepare(std::make_unique<Ardbeg>(Ardbeg::MODEL(id)));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &new_machines[13+id] );
}

BOOST_DATA_TEST_CASE( f108, bdata::xrange( 9 ), id )  {
	prepare(std::make_unique<F108>(F108::MODEL(id)));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &new_machines[17+id] );
}


BOOST_DATA_TEST_CASE( GLUE, bdata::xrange( 3 ), id )  {
	prepare(std::make_unique<GLU>( GLU::MODEL(id) ));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_DATA_TEST_CASE( IIci, bdata::xrange( 3, 7 ), id )  {
	if( id == 5 ) {
		return;
	}
	prepare(std::make_unique<MDU>( MDU::MODEL(id > 5 ? id-4 : id-3)));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_AUTO_TEST_CASE( IIfx )  {
	int id = 5;
	prepare(std::make_unique<OSS>());
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_AUTO_TEST_CASE( LC )  {
	int id = 7;
	prepare(std::make_unique<V8>());
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_DATA_TEST_CASE( Quadra, bdata::xrange( 8, 11 ), id )  {
	prepare(std::make_unique<MCU>(MCU::MODEL(id-8)));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}

BOOST_AUTO_TEST_CASE( PB170 )  {
	int id = 11;
	prepare(std::make_unique<JAWS>());
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}
BOOST_AUTO_TEST_CASE( PB180 )  {
	int id = 12;
	prepare(std::make_unique<Niagara>());
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[id] );
}
BOOST_AUTO_TEST_SUITE( generic );
BOOST_AUTO_TEST_CASE( mdu ) {
	prepare( std::make_unique<MDU> (MDU::MODEL::UNKNOWN));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[14] );
}

BOOST_AUTO_TEST_CASE( oss ) {
	prepare( std::make_unique<OSS> (false));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[15] );
}

BOOST_AUTO_TEST_CASE( v8 ) {
	prepare( std::make_unique<V8> (V8::MODEL::UNKNOWN));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[16] );
}

BOOST_AUTO_TEST_CASE( jaws ) {
	prepare( std::make_unique<JAWS> (false));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	compare( &old_machines[17] );
}
BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_CASE( rbv_disable ) {
	prepare( std::make_unique<MDU> (MDU::MODEL::UNKNOWN));
	machine->rbv = IO_NONE::SELF;
	DR(2) = 0;
	TEST_ROM( 02F64 );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::RBV) ) );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::VDAC) ) );
}

BOOST_AUTO_TEST_CASE( via2_disable ) {
	prepare( std::make_unique<MDU> (MDU::MODEL::UNKNOWN));
	machine->via2 = IO_NONE::SELF;
	DR(2) = 0;
	TEST_ROM( 02F64 );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::VIA2) ) );
}

BOOST_AUTO_TEST_CASE( iop_swim_disable ) {
	prepare( std::make_unique<OSS> (false));
	machine->floppy = std::make_shared<SWIM>();
	DR(2) = 0;
	TEST_ROM( 02F64 );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::IOP_SWIM) ) );
}

BOOST_AUTO_TEST_CASE( iop_swim_enable ) {
	prepare( std::make_unique<OSS> (false));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	BOOST_CHECK( ( DR(0) & 1 << int(INIT_HW_FLG_T::IOP_SWIM) ) );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::NO_ISM) ) );
}

BOOST_AUTO_TEST_CASE( iop_scc_disable ) {
	prepare( std::make_unique<MDU> (MDU::MODEL::UNKNOWN));
	machine->scc = newZ8530( false );
	DR(2) = 0;
	TEST_ROM( 02F64 );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::IOP_SCC) ) );
	BOOST_CHECK( ( DR(0) & 1 << int(INIT_HW_FLG_T::NON_IIFX_1) ) );
	BOOST_CHECK( ( DR(0) & 1 << int(INIT_HW_FLG_T::NON_IIFX_2) ) );
}

BOOST_AUTO_TEST_CASE( iop_scc_enable ) {
	prepare( std::make_unique<MDU> (MDU::MODEL::UNKNOWN));
	machine->scc = newZ8530( true );
	DR(2) = 0;
	TEST_ROM( 02F64 );
	BOOST_CHECK( ( DR(0) & 1 << int(INIT_HW_FLG_T::IOP_SCC) ) );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::NON_IIFX_1) ) );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::NON_IIFX_2) ) );
}

BOOST_AUTO_TEST_CASE( dma_disable ) {
	prepare( std::make_unique<MDU> (MDU::MODEL::UNKNOWN));
	machine->scsi = IO_NONE::SELF;
	DR(2) = 0;
	TEST_ROM( 02F64 );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::SCSI_DMA) ) );
	BOOST_CHECK( ( DR(0) & 1 << int(INIT_HW_FLG_T::PSUEDO_DMA_1) ) );
	BOOST_CHECK( ( DR(0) & 1 << int(INIT_HW_FLG_T::PSUEDO_DMA_2) ) );
	BOOST_CHECK( ( DR(0) & 1 << int(INIT_HW_FLG_T::PSUEDO_DMA_3) ) );
}

BOOST_AUTO_TEST_CASE( dma_enable ) {
	prepare( std::make_unique<MDU> (MDU::MODEL::UNKNOWN));
	DR(2) = 0;
	TEST_ROM( 02F64 );
	BOOST_CHECK( ( DR(0) & 1 << int(INIT_HW_FLG_T::SCSI_DMA) ) );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::PSUEDO_DMA_1) ) );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::PSUEDO_DMA_2) ) );
	BOOST_CHECK( ! ( DR(0) & 1 << int(INIT_HW_FLG_T::PSUEDO_DMA_3) ) );
}


