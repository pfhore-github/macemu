#include "data.hpp"
#include "prototype.hpp"
namespace ROM {
// $4A5E
void init_hw2(uint32_t offset) {
	AR(7) += offset;
	for( uint32_t vbr : vbr_table ) {
		if( rom_base == ( vbr & 0xffff0000 ) ) {
			cpu.VBR = vbr;
		}
	}
	do { 
		getHWInfo(0);			// $02F64
	} while( ! INIT_HW_FLG.test( INIT_HW_FLG_T::MASTER ) );
	reset_via();				// $02E8C
}
}

namespace ROMWrapper {
void run_04A5E() {
	ROM::init_hw2( DR(3) );
	cpu.PC = rom_base | 0x02E46;
}
}
