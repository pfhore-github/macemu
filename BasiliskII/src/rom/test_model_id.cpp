#include <stdint.h>
#include "data.hpp"
#include "prototype.hpp"
namespace ROM {
// $4B6C
static constexpr uint16_t MODEL_ID_IS_IN_VIA = 1 << 11;
bool test_model_id() {
	INIT_FLAGS.set( INIT_FLAG_T::FAULT_SKIP );
	auto id = get_model_id(); // $04BE4
	if( ! id ) {
		return false;
	}
	for( const auto& m : new_machines ) {
		model = &m;
		uint16_t mid = model->machine_id;
		if( mid == *id ) {
			if( *id & MODEL_ID_IS_IN_VIA ) {
				motherboard = model->motherboard;
				INIT_HW_FLG = motherboard->init_flg;
				uint32_t model_id = read_id_from_via(); // $047CC
				if( ( model_id & model->model_mask ) == model->model_id ) {
					return true;
				}				
			} else {
				// 4BDC
				return true;
			}
		}
	}
	// HALT
	for(;;) {
		SDL_Delay(10000);
	}
}
}
namespace ROMWrapper {
void run_04B6C() {
	using namespace ROM;
	INIT_FLAGS = DR(7) >> 16;
	if( test_model_id() ) {
		cpu.PC = rom_base | TEST_MODEL_ID_SUCCESS;
	} else {
		cpu.PC = rom_base | TEST_MODEL_ID_FAILURE;
	}
	if( motherboard ) {
		AR(0) = rom_base | motherboard->BASE;
	}
	if( model ) {
		AR(1) = rom_base | model->BASE;
	}
	DR(0) = INIT_HW_FLG.to_ulong();
	DR(7) = ROM::INIT_FLAGS.to_ulong() << 16 | (DR(7) & 0xffff);
}
}
