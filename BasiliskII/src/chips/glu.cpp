#include "io.hpp"
#include "registers.hpp"
#include <stdint.h>
#include <assert.h>
#include <atomic>
#include "glu.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "scc.hpp"
#include "machine.hpp"
#include "ncr5380.hpp"
#include "asc.hpp"
#include "z8530.hpp"
#include "mem.hpp"
const uint32_t simm30_table[5] = { 0, 1*1024*1024, 4*1024*1024, 16*1024*1024, 64*1024*1024 };

GLU::GLU(MODEL model, SIMM30_SIZE bankA, SIMM30_SIZE bankB) {
	
	via1 = std::make_shared<VIA1>();
	via2 = std::make_shared<VIA2>();
	asc = newPlaneASC();
	scc = newZ8530();
	scsi = std::make_shared<Ncr5380>();
	model_map[0] = model_map[1] = model_map[2] = false;
	model_id = 0;

	bank_size[0] = simm30_table[ int(bankA) ] ;
	bank_size[1] = simm30_table[ int(bankB) ] ;

	switch( model ) {
	case MODEL::IIx :
		model_map[3] = false;
		model_map2 = false;
		break;
	case MODEL::SE30 :
		model_map[3] = true;
		model_map2 = false;
		break;		
	case MODEL::IIcx :
		model_map[3] = true;
		model_map2 = true;
		break;
	}
	bankA_size = 0;
}
std::shared_ptr<IO_BASE> GLU::get_io(uint32_t base) {
	switch((base>>13) & 0xf) {
	case 0  : return via1;
	case 1  : return via2;
	case 2  : return scc;
	case 8  : return scsi;
	case 10 : return asc;
	default : return {};
	}
}

uint8_t GLU::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0xf) {
	case 0  : return via1->read(addr>>9&0xf);
	case 1  : return via2->read(addr>>9&0xf);
	case 2  : return scc->read(addr>>1&3);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	default : bus_error(addr, attr); return 0;
	}
}
#include <stdio.h>
void GLU::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0xf) {
	case 0  : return via1->write(addr>>9&0xf, v);
	case 1  : return via2->write(addr>>9&0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	default :	bus_error(addr, attr);
	}
}
	
void* GLU::get_ram_addr(uint32_t addr, int attr) {
	uint32_t bankA_sz = simm30_table[ bankA_size+1 ];
	if( addr < bankA_sz + bank_size[1]) {
		return &RAMBaseHost[addr];
	} else {
		return nullptr;
	}
}
