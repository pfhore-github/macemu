#include <stdint.h>
#include <stdio.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#include "data.hpp"
#include "prototype.hpp"
#include "mac_via.h"
#include "via.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "rbv.hpp"
#include "io.hpp"
#include "asc.hpp"
#include "machine.hpp"
#include "scc.hpp"
extern std::atomic<uint8_t> state;
void adb_set_state(int b);
void adb_write(uint8_t v);
uint8_t adb_read();
namespace ROM {





void _BD3EC();
void _474BA() {
	getHWInfo2(0);
	cpu.R[3] = model->unknown_48;
	if( cpu.R[3] ) {
		uint32_t a2 = /* model.data(cpu.R[3]) */ 0;
		cpu.R[3] = read_l(a2);
		if( cpu.R[3] ) {
			cpu.R[8+2] = a2;
			run_rom(a2+cpu.R[3]);
		}
	}
	
}

void _478A4() {
	uint16_t r1;
	do {
		r1 = cpu.R[3];
		_4B53C();
		cpu.R[4] = cpu.R[4] >> 8 | cpu.R[4] << 24;
		cpu.R[4] = (cpu.R[4] & 0xffffff00) |( r1 & 0xff);
		cpu.R[3] = (cpu.R[3] & 0xffff0000) | (( cpu.R[3]+1) & 0xffff);
		r1 = cpu.R[3] & 3;
	} while( r1 );
	return;
}

// $47C40
void check_checksum() {}


void _B358C() {}
void _B375E() {}
void _B381E() {}
void write_adb1(uint8_t r2) {
	adb_write(r2);
}
void _B3BBA() {
	write_adb1(cpu.R[2]);
}
uint8_t read_adb1() {
	return adb_read();
}
void _B3BDA() {
	cpu.R[2] = (cpu.R[2] & 0xffff00ff) | read_adb1();
	adb_set_state(0);
}
void _B3AB8() {
	if( cpu.R[15] < 0x8000 || read_w(0xde0) != 0 || cpu.VBR ) {
		// $B3B12
		machine->via1->set(vACR, 2 );
		machine->via1->set(vACR, 3 );
		machine->via1->set(vACR, 4 );
		write_adb1(1);
		read_adb1();
		write_adb1(7);
		write_adb1(cpu.R[1] >> 8);
		write_adb1(cpu.R[1]);
		read_adb1();

		adb_set_state(1);
		read_adb1();
		read_adb1();
		read_adb1();
		cpu.R[1] = read_adb1();
		adb_set_state(3);
		machine->via1->read(vSR);
	} else {
		// $B3AD0
	}
}

void _4B53C() {
	uint32_t a3 = model->unknown_52;
	uint32_t d0 = /* model->data(a3) */ 0;
	if( d0 == 0 )
		return;
	a3 += d0;
	run_rom(a3);
	return;
	
}

}
