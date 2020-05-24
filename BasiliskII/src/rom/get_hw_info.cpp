#include <stdint.h>
#include "data.hpp"
#include "prototype.hpp"
namespace ROM {
static void mb_check();
static void update_init_flag();
// $2F64
void getHWInfo(uint8_t gestalt_id) {
	// 新しい機種用の判定をさきに行う
	if( ! test_model_id() ) {	// $04B6C
		// 引数としてGestaltIDを指定されたときは同じ機種データを取得
		if( gestalt_id ) {
			for(const auto& base : old_machines ) {
				if( base.gestalt_machine_id() == gestalt_id ) {
					model = &base;
				}
			}
		}
		// マザーボードチェックへ飛ぶ
		if( ! model ) {
			mb_check();
		}
	}
	// $02F82
	motherboard = model->motherboard;
	INIT_HW_FLG = model->init_flg;
	if( INIT_HW_FLG.count() == 0 ) {
		update_init_flag();
	} 
    HWCFG_FLAGS = model->hwcfg.to_ulong();
    GID = model->gestalt;
    GEN = model->generation;

	check_flag();				// $04B0C

	// $4080309C
	ROM_FLG = model->rom_flg;
	if( ROM_FLG.count() == 0 ) {
		ROM_FLG = motherboard->rom_flg;
	}
	INIT_FLAGS.reset( INIT_FLAG_T::FAULT_SKIP );
}

// $2F9E
static void mb_check() {
	for( auto mb : hw_test_table ) {
		motherboard = mb;
		if( ! mb->check() )
			continue;
		// $02FB0
		INIT_HW_FLG = motherboard->init_flg.to_ulong();
		uint32_t id = read_id_from_via();		// $047CC
		uint8_t gen = motherboard->generation;
		for(const auto& base : old_machines ) {
			model = &base;
			if ( model->generation != gen ) {
				continue;
			}
			if( ( id & model->model_mask ) != model->model_id ) {
				continue;
			}
			return;
		}
	}
	for(;;) {
		SDL_Delay(100000);
	}
}

// $2FE4
static void update_init_flag() {
	INIT_HW_FLG = motherboard->init_flg.to_ulong();
	const MOTHERBOARD_TABLE_T* mb = model->motherboard;
	if( INIT_HW_FLG.test(INIT_HW_FLG_T::RBV) ) {
		if( ! has_rbv(mb->rbv_addr + 0x13) ) {	// $046D8
			// not RBV/VDAC
			INIT_HW_FLG.reset( INIT_HW_FLG_T::RBV );
			INIT_HW_FLG.reset( INIT_HW_FLG_T::VDAC );
		}
	}
	if( INIT_HW_FLG.test(INIT_HW_FLG_T::VIA2 ) ) {	
		if( ! has_via( mb->via2_addr + 0x1c00 ) ) { // $046AA
			// no VIA2
			INIT_HW_FLG.reset( INIT_HW_FLG_T::VIA2 );
		}
	}
	if( INIT_HW_FLG.test(INIT_HW_FLG_T::IOP_SWIM ) ) {
		if( ! test_iop( mb->swim_iop_addr) ) { // $0477C
			// no SWIM IOP
			INIT_HW_FLG.reset( INIT_HW_FLG_T::IOP_SWIM );
		} else {
			// has SWIM IOP
			INIT_HW_FLG.reset( INIT_HW_FLG_T::NO_ISM );
		}
	}
	if( INIT_HW_FLG.test(INIT_HW_FLG_T::IOP_SCC ) ) {
		if( ! test_iop( mb->scc_iop_addr ) ) { // $0477C
			// no SCC IOP
			INIT_HW_FLG.reset( INIT_HW_FLG_T::IOP_SCC );
		} else {
			// has SCC IOP
			INIT_HW_FLG.reset( INIT_HW_FLG_T::IIFX_1 );
			INIT_HW_FLG.reset( INIT_HW_FLG_T::IIFX_2 );
		}
	}
	if( INIT_HW_FLG.test(INIT_HW_FLG_T::SCSI_DMA ) ) {
		if( !test_scsi_io(mb->scsi_dma_addr)) { // $04700
			// no "true" SCSI DMA
			INIT_HW_FLG.reset( INIT_HW_FLG_T::SCSI_DMA );
		} else {
			// has "true" SCSI DMA
			INIT_HW_FLG.reset( 8 );
			INIT_HW_FLG.reset( 9 );
			INIT_HW_FLG.reset( 10 );
		}
	}
	test_scsi();				// $04A96
	return;
}
}

namespace ROMWrapper {
void run_02F64() {
	using namespace ROM;
	uint32_t ret = AR(6);
	INIT_FLAGS = DR(7) >> 8;
	getHWInfo( DR(2) );
	AR(0) = rom_base | motherboard->BASE;
	AR(1) = rom_base | model->BASE;
	DR(0) = INIT_HW_FLG.to_ulong();
	DR(1) = ROM_FLG.to_ulong();
	DR(2) = GET_D2();
	DR(7) = INIT_FLAGS.to_ulong() << 16 | EXCEPTION_STATUS;
	cpu.PC = ret;
}
}
