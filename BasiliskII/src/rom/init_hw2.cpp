#include "data.hpp"
#include "prototype.hpp"
namespace ROM {
// $4A5E
void init_hw2(uint32_t offset) {
	AR(7) += offset;
	for( unsigned int i = 0; i < 4; ++i ) {
		if( rom_base == ( vbr_table[i] & 0xffff0000 ) ) {
			cpu.VBR = vbr_table[i] - 4*i + 4;
		}
	}
	getHWInfo(0);			// $02F64
	SDL_assert( INIT_HW_FLG.test( INIT_HW_FLG_T::MASTER ) );
	reset_via();				// $02E8C
}
}

namespace ROMWrapper {
void run_04A5E() {
	using namespace ROM;
	init_hw2( DR(3) );
	AR(0) = rom_base | motherboard->BASE;
	AR(1) = rom_base | model->BASE;
	DR(0) = INIT_HW_FLG.to_ulong();
	DR(1) = ROM_FLG.to_ulong();
	DR(2) = GET_D2();
	DR(7) = INIT_FLAGS.to_ulong() << 16 | EXCEPTION_STATUS;
	cpu.PC = rom_base | 0x02E46;
}
}
