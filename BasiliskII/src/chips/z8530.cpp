#include "scc.hpp"
#include "devices/serial.hpp"
#include "scc_impl.hpp"
#include "registers.hpp"
uint8_t Z8530::read_reg_impl(int reg) {
	switch(reg) {
	case 0 :					// Buffer Status
	case 4 :
		return rr[0];
	case 1 :
	case 5 :
		return rr[1];
	case 8 :					// Recieve Data Register
		return read_data();
	case 9 :
	case 13 :
		return rr[13];
	default :
		return rr[reg & 15];
	}
}

void Z8530::write_reg0(uint8_t v) {
	reg_ptr = v & 7;
	switch( (v>>3) & 7 ) {
	case 0 : break;
	case 1 : reg_ptr |= 8; break;
	case 2 :
		// RESET Ext/Status Interrupts
		rr[0] |= 0x80;
		break;
	case 3 :
		// TODO: SEND Abort
		device->abort();
		rr[0] |= 0x40;
		break;
	case 4 :
		// TODO: Enable Int on Next Rx Character
		break;
	case 5 :
		// TODO: Reset Tx Int Pending
		break;
	case 6 :
		// TODO: ERROR reset
		rr[1] &= 0x70;
		break;
	case 7 :
		// TODO: RESET Highest IUS
		break;
	}
	switch( (v >> 6) & 3 ) {
	case 0 : break;
	case 1 :
		// TODO: RESET Rx CRC
		break;
	case 2 :
		// TODO: Reset Tx CRC
		break;
	case 3 :
		// TODO: Reset Tx Underrun
		break;
	}
}

void Z8530::write_reg9(uint8_t v) {
	switch( v >> 6 & 3 ) {
	case 0 :
		break;
	case 1 :
		// TODO channel B Reset
		break;
	case 2 :
		// TODO channel A Reset
		break;
	case 3 :
		// TODO force HW reset
		break;
	}
}


void Z8530::write_reg_impl(int n, uint8_t v) {
	switch(n) {
	case 0 :
		write_reg0(v);
		return;
	case 8 : // transmit buffer 
		write_data(v);
		return;
	case 9 :
		write_reg9(v);
		return;		
	}
}
