#include <stdint.h>
#include "io.hpp"
#include "via.hpp"
#include "registers.hpp"
#include "xpram.h"
#include "adb.h"
#include "rtc.hpp"
#include "via1.hpp"
#include "machine.hpp"
#include "scc.hpp"
#include "glu.hpp"
extern bool adb_interrupt;
constexpr uint8_t ADB_CMD = 0;
constexpr uint8_t ADB_DATA_EVEN = 1;
constexpr uint8_t ADB_DATA_ODD = 2;
constexpr uint8_t ADB_IDLE = 3;
uint8_t VIA1::read(int n) {
	if( n == VIA_REG::SR && adb->st == ADB_IDLE ) {
		adb_int.store(true);
		return 0;
	} else {
		return VIA::read(n);
	}
}
VIA1::VIA1(int i) :VIA(i), sync(false), fd_head(false) {
	adb = std::make_unique<ADBController>();
}
bool VIA1::readA(int n) {
	switch(n) {
	case 1 : return machine->model_map[0];
	case 2 : return machine->model_map[1];
	case 4 : return machine->model_map[2];
	case 6 : return machine->model_map[3];
	case 7 : return scc_wr_req.exchange(true);
	default : return false;
	}
	return false;
}
bool VIA1::readB(int n) {
	switch(n) {
		// RTC DATA
	case 0 : return rtc.read();
		// ADB INT
	case 3 :
		if( adb->st == ADB_IDLE ) {
			return true;
		} else {
			return adb_int.exchange( false );
		}
	default : return false;
	}
	return false;
}

void VIA1::writeA(int n, bool v) {
	switch(n) {
	case 3 : sync = v; return;
	case 4 :
		if( v && dynamic_cast<GLU*>(machine.get()) ) {
			machine->rom_mirror = true;
		}
		return;
	case 5 : fd_head = v; return;
	case 7 : scc_wr_req = v; return;
	default : return;
	}
}
void VIA1::cb2_out(uint8_t v) {
	adb_v = v;
}

void VIA1::adb_set_state(int b) {
	switch(b) {
	case ADB_CMD: 
		adb->cmd(adb_v);
		break;
	case ADB_IDLE :
		adb->st = b;
		break;
	case ADB_DATA_EVEN:
	case ADB_DATA_ODD:
		if( std::exchange(adb->st, b) != b ) {
			if( a_ctl.SR_OUT ) {
				// send to ADB
				adb->data(adb_v, b);
			} else {
				// recieve from ADB
				adb_v = adb->read();
				machine->via1->cb2_in_push_byte(adb_v);
			}
		}
		break;
	}
}

void VIA1::writeB(int n, bool v) {
	switch(n) {
	case 0 : rtc.write(v); return ;
	case 1 : rtc.clock(v); return ;
	case 2 : rtc_enable = v; return ;
	case 4 : 
		// ST0
		adb_state = ((adb_state &~ 1) | v);
		return;
	case 5 :
		// ST1
		adb_state = (adb_state&~2) | v<<1;
		adb_set_state(adb_state);
		return;
	default : return;
	}
}



