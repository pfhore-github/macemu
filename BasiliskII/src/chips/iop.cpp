#include "iop.hpp"
#include "machine.hpp"

uint8_t IOP::read(int addr) {
	switch(addr) {
	case IOP_REG::ADDR_HI : return (ram_addr >> 8);
	case IOP_REG::ADDR_LO : return ram_addr & 0xff;
	case IOP_REG::CTRL : return ctrl;
	case IOP_REG::RAM :
	case IOP_REG::RAM+1 : {
		uint8_t v = data[ram_addr];
		if( ctrl & IOP_AUTOINC ) {
			++ram_addr;
		}
		return v;
	}
	default : return read_hw(addr-0x10);
	}
}
void IOP::write(int addr, uint8_t v) {
	switch(addr) {
	case 0 : ram_addr = (ram_addr & 0xff) | v << 8; break; 
	case 1 : ram_addr = (ram_addr & 0xff00) | v; break; 
	case 2 :
		ctrl = v;
		if( !(v & IOP_RUN) ) {
			ram_addr = 0;
		}
		break;
	case 4 :
	case 5 :
		data[ram_addr] = v;
		if( ctrl & IOP_AUTOINC ) {
			++ram_addr;
		}
		break;
	default : write_hw(addr-16, v); break;
	}
}
