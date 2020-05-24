#include "data.hpp"
#include "prototype.hpp"
#include "machine.hpp"
namespace ROM {
// $4B0C
void check_flag() {
	if( HWCFG_FLAGS.test( HWCFG_T::HAS_FPU ) ) {
		if( ! has_fpu() ) {	// $04640
			HWCFG_FLAGS.reset( HWCFG_T::HAS_FPU );
		}			
	}
	INIT_FLAGS.set( INIT_FLAG_T::FAULT_SKIP );
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::ETHER) ) {
		if( ! has_ether( motherboard->ether_addr ) ) { // $047B0
			INIT_HW_FLG.reset( INIT_HW_FLG_T::ETHER);
		}
	}
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::SPECIAL_LOWMEM ) ) {
		if( ! check_LMG1( motherboard->unknown116 ) ){ // $047BA
			INIT_HW_FLG.reset( INIT_HW_FLG_T::SPECIAL_LOWMEM );
		}
	}
}
}
namespace ROMWrapper {
void run_04B0C() {
	ROM::SET_D2( DR(2) );
	ROM::INIT_FLAGS = DR(7) >> 16;
	ROM::INIT_HW_FLG = DR(0);
	ROM::check_flag();
	DR(0) = ROM::INIT_HW_FLG.to_ulong();
	DR(2) = ROM::GET_D2();
	DR(7) = ROM::INIT_FLAGS.to_ulong() << 16 | (DR(7) & 0xffff);
	cpu.PC = rom_base | 0x309C;
}
}
