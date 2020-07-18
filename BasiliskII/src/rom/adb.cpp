#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "mac_via.h"
#include "via.hpp"
#include "via1.hpp"
#include "asc.hpp"
#include "prototype.hpp"
#include "assert.h"
#include "adb.h"
#include "scc.hpp"
#include "machine.hpp"
constexpr uint8_t ADB_CMD = 0;
constexpr uint8_t ADB_DATA_EVEN = 1;
constexpr uint8_t ADB_DATA_ODD = 2;
constexpr uint8_t ADB_IDLE = 3;
extern std::atomic<bool> adb_interrupt;
extern std::atomic<uint8_t> state ;
void adb_set_state(int b);

namespace ROM {
// $AA0E0
void sad_mac_x() {
	machine->via1->adb_set_state( 3 );
	EXCEPTION_STATUS = 0x30;
	sad_mac( 1 );
}
// $85714
void init_adb() {
	machine->via1->adb_set_state( 3 );
	machine->via1->read( VIA_REG::SR );
	machine->via1->adb_set_state( 2 );
	if( machine->via1->test( VIA_REG::RB, VIA1::ADB_INT ) )
		return sad_mac_x();
	SDL_Delay(0);
	machine->via1->adb_set_state( 3 );
	if( ! machine->via1->test( VIA_REG::RB, VIA1::ADB_INT ) )
		return sad_mac_x();
	machine->via1->read( VIA_REG::SR );
	if( machine->via1->test( VIA_REG::IFR, IRQ_FLAG::SREG ) )
		return sad_mac_x();
	machine->via1->adb_set_state( 3 );
}

// $A9FFC
void init_adb2(uint16_t r0_1, uint16_t r0_2, int r1, int r2) {
	uint16_t d4[] = { 0x2000, 0x0100 };
	if( machine->via1->test( VIA_REG::RB, 5 ) ) { // state != 3
		// initialize ADB state
		machine->via1->adb_set_state( 3 );
	}
	// $AA01C
	cpu.R[4] = 0x01002000;
	talk_adb(r0_1);
	uint32_t r1_x = r1;
	if( r2 <= 4 ) {
		for(int i = 0; i < r2; ++i ) {
			talk_adb(r1 >> (8*i));
		}
	}
	
	uint16_t low = read_adb1();
	uint16_t high = read_adb1();
	uint32_t tp = high << 16 | low;
	read_adb1();
	cpu.R[0] = tp;
	if( cpu.R[0] != 2 ) {
		cpu.R[0] = 0;
	}
	return;				
}
}

}
