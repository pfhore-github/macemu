#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#include "mac_via.h"
#include "data.hpp"
namespace ROM {
#if 0
// $b631a
uint8_t via_rtc_send(uint8_t d1) {	
	for(int i = 7; i >= 0 ; --i ) {
		uint8_t d0 = read_b(GEN->via_base1+vBufB);
		uint8_t dx = d0 & (~VIA1B_vRTCData) | (d1 >> i)&1;
		write_b(GEN->via_base1+vBufB, dx);
		write_b(GEN->via_base1+vBufB, dx &~ VIA1B_vRTCClk);
		write_b(GEN->via_base1+vBufB, dx | VIA1B_vRTCClk);
	}
	return;	
}
// $b3638
uint8_t via_rtc_recv() {
	uint8_t v = read_b(GEN->via_base1+vDirB);	
	write_b(GEN->via_base1+vDirB, v &~ VIA1B_vRTCData);
	int d1 = 1;
	int vx = read_b(GEN->via_base1+vBufB) &~  VIA1B_vRTCClk;
	for(int i = 0; i < 8; ++i ) {
		write_b(GEN->via_base1+vBufB, vx);
		write_b(GEN->via_base1+vBufB, vx |  VIA1B_vRTCClk);
		d1 = d1 << 1 | (read_b(GEN->via_base1+vBufB) & VIA1B_vRTCData);
	} while( d0 < 0x100 );
	write_b(GEN->via_base1+vDirB, v | VIA1B_vRTCData);
	return v1;
}
// $b35c6; A2=$50F00000
void clk_no_mem(uint32_t a2, uint8_t d1) {
	uint16_t d2_h = get_sr();
	// disable interrupt
	cpu.IX = 7;
	// enable rtc
	int vx = read_b(GEN->via_base1+vBufB) &~ VIA1B_vRTCEnb;
	write_b(GEN->via_base1+vBufB, vx);
	if( (0x78 & d1) == 0x38 ) {
		// XPRAM
		via_rtc_send(d1);
	}
	if((int8_t)d1 >= 0 ) {
		via_rtc_send(d1);
		via_rtc_send(cpu.D[2]);
	} else {
		via_rtc_send();
		cpu.D[2] = vir_rtc_recv();
	}
	uint8_t v = read_b(VIA1_BASE+vBufB);
		
	write_b(VIA1_BASE+vBufB, v | VIA1B_vRTCEnb);
	cpu.set_sr(d2_h);
	return;
}

// $B9886
void _B9886() {
	getHWInfo2(0);
	a2 = GEN_MAP->via_base1;
	if( HWCLG_FLAGS._24 ) {
		_4736E();
	}
	if( ! INIT_FLAGS._26 ) {
		getHWInfo2(0);
		if( ADDR_FLG._12 ) {
			_48C52(GEN_MAP->asc_base, GEN_MAP->via_base1);
		}				
	}
	d5 = 0;
	_B9F04();
	_B9F36();
	uint16_t d0 = _BA0A8();
	if( d0 >= 0 ) {
		if( (d0 &0x7f) != 0x2a ) {
			if( 
		}
	}
}
// $B9F04
uint32_t set_scc_address(uint32_t* scc_addr) {
	getHWInfo2(0);
	if( ADDR_FLG.scc ) {
		*scc_addr = GEN_MAP->scc_iop_base;
		return 0;
	} else {
		*scc_addr = GEN_MAP->scc_base;
		return GEN_MAP->scc_base2 - a3;
	}
}
#endif
}
