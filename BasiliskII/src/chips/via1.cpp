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

VIA1::VIA1(int i) :VIA(i), sync(false), fd_head(false) {
}
bool VIA1::readA(int n) {
	switch(n) {
	case 0 : return true;
	case 1 : return machine->model_map[0];
	case 2 : return machine->model_map[1];
	case 3 : return sync;
	case 4 : return machine->model_map[2];
	case 5 : return fd_head;
	case 6 : return machine->model_map[3];
	case 7 : return scc_wr_req.exchange(true);
	}
	return false;
}
bool VIA1::readB(int n) {
	switch(n) {
	case 0 : return rtc.read();
		// ADB is always data in
	case 3 : return bool(adb_bus);
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
constexpr uint8_t ADB_CMD = 0;
constexpr uint8_t ADB_DATA_EVEN = 1;
constexpr uint8_t ADB_DATA_ODD = 2;
constexpr uint8_t ADB_IDLE = 3;
void VIA1::cb2_out( bool v) {	
	out_buf = (out_buf << 1 | v);
	if( out_pos++ == 8 ) {
		adb_bus->send_to(out_buf);
		out_pos = 0;
		out_buf = 0;
	}
}
#if 0
bool VIA1::cb2_in_pull() {
	bool v = in_buf & 1;
	in_buf >>= 1;
	if( in_pos == 8 ) {
		in_buf = adb_bus->get_from();
		in_pos = 0;
	}
	return v;
}
#endif
void VIA1::adb_set_state(int b) {
	switch(b) {
	case ADB_CMD: 
		adb_bus->cmd(sr);
		break;
	case ADB_IDLE :
		adb_bus->state = b;
		// resend
		adb_bus->cmd(sr);
		break;
	case ADB_DATA_EVEN:
	case ADB_DATA_ODD:
		if( adb_bus->state != b ) {
			if( a_ctl.SR_OUT ) {
				// send to ADB
				adb_bus->send_to(sr);
			} else {
				// recieve from ADB
				sr = adb_bus->get_from();
			}
		}
		adb_bus->state = b;
		break;
	}
}
void VIA1::writeB(int n, bool v) {
	static uint8_t adb_state = 0;
	switch(n) {
	case 0 : rtc.write(v); return ;
	case 1 : rtc.clock(v); return ;
	case 2 : rtc_enable = v; return ;
	case 4 : 
		if( adb_bus ) {
			// ST0
			adb_state = ((adb_state &~ 1) | v);
			adb_set_state(adb_state);
		}
		return;
	case 5 :
		if( adb_bus ) {
			// ST1
			adb_set_state(adb_state | v<<1);
			adb_set_state(adb_state);
		}		
		return;
	default : return;
	}
}



