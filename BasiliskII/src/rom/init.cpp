#include <stdint.h>
#include "sysdeps.h"
#include "main.h"
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "data.hpp"
#include "prototype.hpp"
#include "mac_via.h"
#include <unordered_map>
#include <functional>
#include "via1.hpp"
#include "machine.hpp"
#include "adb.h"
#include "rbv.hpp"
#include "oss.hpp"
#include "jaws.hpp"
#include "prefs.h"
#include "inst_cmn.hpp"
MB_TYPE mb_type;
// LC630 ROM
extern std::atomic<uint8_t> state;
void adb_write(uint8_t v);
uint8_t adb_read();

void adb_set_state(int b);
void set_machine(const std::string& s);
void emul_reset(uint32_t* d, uint32_t* a);
extern "C" void Execute68k(uint32_t addr, struct M68kRegisters *r);
namespace ROM {
void run_rom(uint32_t addr) {
	M68kRegisters r;
	memcpy(&r, cpu.R, sizeof(uint32_t)*16);
	r.sr = get_sr();
	Execute68k(rom_base | addr, &r);
	memcpy(cpu.R, &r, sizeof(uint32_t)*16);
	set_sr(r.sr);		
}





// $8E0
void goto_boot2() {
	return boot_second();
}


void halt() {
	for(;;) {
		SDL_Delay(10000);
	}
}
void update_init_flag();
void mb_check();

// $3DE8
const uint8_t vbr_handler[] = {
	0x08, 0x07, 0x00, 0x1b, 	// BTST #27, %D7
	0x2e, 0x4d,					// MOVEAL %A5, %SP
	0x4e, 0xd6					// JMP (%FP)
};





bool reset_rbv_irq(uint32_t addr);


// $706A

void play_asc_4() {
	cpu.R[8+4] = 0x408071C4;
//	play_asc();
}






// $14E9C
void read_and_wait_vbufB() {
	machine->via1->read(vBufB);
}

// $14E4E
void _14E4E() {
	// SHIFT OUT MODE
	machine->via1->set(vACR, 4);
	machine->via1->write(vSR, cpu.R[0]);
	adb_set_state(state | 1);
	machine->via1->read(vBufB);
	machine->via1->read(vSR);
	adb_set_state(state & 2);
	return;
}
// $14DAC 
void adb_write_14xxx() {
	// SHIFT OUT MODE
	machine->via1->set(vACR, 4);
	if( machine->via1->bit(vBufB, 3) ) {
		adb_write(cpu.R[0]);
		adb_set_state(state | 1);
	}
	machine->via1->read(vSR);
	adb_set_state(state & 2);
	machine->via1->read(vBufB);
	cpu.Z = ! machine->via1->bit(vBufB, 3);
	return;
}
// $14E6E
void adb_read_14xx() {
	adb_set_state(state | 1);
	cpu.R[0] = adb_read();
	machine->via1->read(vBufB);
	adb_set_state(state & 2);
}

void _14E8E() {
	machine->via1->read(vSR);
}
void _14CCC() {
	for(;;) {
		cpu.R[4] = 0x20000100;
		read_and_wait_vbufB();
		if( state & 2 ){
			adb_set_state(0);
			read_and_wait_vbufB();
			read_and_wait_vbufB();
		}
		cpu.R[4] = cpu.R[4] << 16 | cpu.R[4] >> 16;
		if( machine->via1->bit(vBufB, 3) ) {
			adb_set_state(state | 2); 
			adb_write_14xxx();
			if( ! cpu.Z ) {
				_14E4E();
				read_and_wait_vbufB();
				cpu.R[0] = cpu.R[1];
				cpu.R[2]--;
				if( (int16_t)cpu.R[2] < 3 ) {
					do {
						_14E4E();
						read_and_wait_vbufB();
						cpu.R[0] >>= 8;
					} while( (int16_t)cpu.R[2]-- >= 0 );
				}
				// SHIFT IN MODE
				machine->via1->clear(vACR, 4);
				adb_set_state( state & 1);
				read_and_wait_vbufB();
				_14E8E();
				read_and_wait_vbufB();
				adb_set_state( state | 2);
				adb_read_14xx();
				cpu.R[0] &= 0xffff00ff;
				cpu.R[0] = cpu.R[0] >> 16 | cpu.R[0] << 16;
				adb_read_14xx();
				cpu.R[0] &= 0xffff00ff;
				cpu.R[0] = cpu.R[0] >> 16 | cpu.R[0] << 16;
				uint32_t v = cpu.R[1];
				cpu.R[1] = cpu.R[0];
				cpu.R[0] = v;
				adb_read_14xx();
				adb_set_state( state & 1 );
				read_and_wait_vbufB();
				cpu.R[0] = cpu.R[1];
				if( (cpu.R[0] & 0xff) != 2 ) {
					cpu.R[0] = 0;
				}
				cpu.Z = true;
				return ;
			}
			machine->via1->clear(vBufB, 5);
		}
		read_and_wait_vbufB();
		machine->via1->clear(vACR, 4);
		adb_read();
		adb_set_state(2);
		adb_set_state(3);
		adb_read();
		machine->via1->read(vBufB);
		adb_set_state(0);
		read_and_wait_vbufB();
	}
}
void _8C40A() {
	cpu.R[0] = 0x0E0001;
	cpu.R[2] = 1;
	_14CCC();
	cpu.Z = (cpu.R[0] & 0xffff) == 0;
}
void _8C400() {
	cpu.R[1] = 0;
	return _8C40A();
}


void switch_hwtest(uint32_t addr) {}
uint32_t get_hw_info_addr;
}

uint32_t find_rom_data(uint32_t start, uint32_t end, const uint8_t *data, uint32_t data_len);

