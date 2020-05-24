#include "data.hpp"
#include "prototype.hpp"
#include "scc.hpp"
#include <assert.h>
namespace ROM {
// $47EF6
uint16_t init_scc_iop_fw() {
	struct IOP_FW {
		uint16_t sz;
		uint16_t addr;
		const char* data;
	} iop_fw = {
		17, 0x7FEE, "\xa9\x81\x8d\x30\xf0\xa9\x44\x8d\x31\xf0\x80\xfe\xee\x7f\xee\x7f\xee\x7f"
		/*
		  LDA #$81				; A9 81
		  STA $F030				; 8D 30 F0
		  LDA #$44				; A9 44
		  STA $F031				; 8D 31 F0
		  BRA -2	   			; 80 FE 
		  ???					; EE 7F EE 7F EE 7F
		*/
		  
	  
	};
	machine->scc->write(IOP_REG::CTRL, 0x32 );
	uint16_t d0 = iop_fw.sz;
	machine->scc->write(IOP_REG::ADDR_HI, iop_fw.addr >> 8);
	machine->scc->write(IOP_REG::ADDR_LO, iop_fw.addr);
	for(int i = 0; i <= iop_fw.sz; ++i ) {
		machine->scc->write(IOP_REG::RAM, iop_fw.data[i]);
	}
	machine->scc->write(IOP_REG::ADDR_HI, iop_fw.addr >> 8);
	machine->scc->write(IOP_REG::ADDR_LO, iop_fw.addr);
	for(int i = 0; i <= iop_fw.sz; ++i, --d0 ) {
		if( machine->scc->read(IOP_REG::RAM) != (uint8_t)iop_fw.data[i] ) {
			return d0+1;
		}
	}
	machine->scc->write(IOP_REG::CTRL, 0x36 );
	return d0+1;
}

}
namespace ROMWrapper {
void run_47EF6() {
	using namespace ROM;
	assert( test_machine_register(machine->scc, AR(3)));
	assert( test_machine_register(machine->scc, AR(3)+DR(3)));
	uint16_t v = ROM::init_scc_iop_fw();
	DR(0) = (DR(0) & 0xffff0000) | (v & 0xffff);
	
}
}
