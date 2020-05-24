#include "data.hpp"
#include "prototype.hpp"
// death crime?
namespace ROM {
// $4AA98
void sad_mac( uint32_t d6) {
	AR(7) = 0x2600;
	getHWInfo2(0);				// $47752
	if( ! INIT_FLAGS.test( INIT_FLAG_T::FAULT_SKIP) ) {
		// fault excpetion
		play_chime_1();		// $45C1C
		comm_modem_port(d6);	// $B98BC
		return;
	}
	DR(6) = d6;
	// $4AAC8
	if( INIT_HW_FLG.test( INIT_HW_FLG_T::MASTER2) ) {
		try {
			// TODO
			uint32_t a1 = motherboard->secret;
			uint32_t d3 = read_l(a1);
			uint32_t a0 = read_l(a1+4);
			a1 += 8;
			if( d3 == 0xaaaa5555 ) {
				AR(1) = 0x470da;
				AR(3) = 0xaaa0c;
				run_rom(a0);
			}
		} catch(Exception::AccessFault&) {
		}
	}
	INIT_FLAGS.set(INIT_FLAG_T::MODEM_PORT_UNSEND);
	INIT_FLAGS.set(INIT_FLAG_T::MODEM_PORT_WAIT);
	reset_VIA1_timer2();		// $BA090
	if( dynamic_cast<MC68040_MMU*>(mmu) ) {
		mmu->exec( 0xF498, 3, 0 ); // CINVA IC
	} else {
		// 68030?
		mmu->movec_to( 2, mmu->movec_from(2) | 1 << 3 ); // CLEAR Instruction Cache
		mmu->movec_to( 2, mmu->movec_from(2) | 0x11 );	 // Enable Instruction Cache & Instruction Burst Enable
	}
	comm_modem_port(d6);	// $B98BC
}
}

#include "data.hpp"
#include "prototype.hpp"
namespace ROMWrapper {
void run_4AA98() {
	ROM::INIT_FLAGS = DR(7) >> 16;
	ROM::EXCEPTION_STATUS = DR(7);
	ROM::sad_mac( DR(6) );
	DR(7) = ROM::INIT_FLAGS.to_ulong() << 16 | ROM::EXCEPTION_STATUS;
}
}
