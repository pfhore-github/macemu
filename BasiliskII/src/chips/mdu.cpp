#include "io.hpp"
#include "registers.hpp"
#include <stdint.h>
#include <assert.h>
#include <atomic>
#include "mdu.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "scc.hpp"
#include "machine.hpp"
#include "ncr5380.hpp"
#include "asc.hpp"
#include "z8530.hpp"
#include "rbv.hpp"
MDU::MDU(MODEL model, SIMM30_SIZE bankA, SIMM30_SIZE bankB) {
	via1 = std::make_shared<VIA1>(1);
	if( model == MDU::MODEL::UNKNOWN ) {
		via2 = std::make_shared<VIA2>(2);
		rbv = std::make_shared<RBV>();
	} else {
		via2 = rbv = std::make_shared<RBV>();
	}
	asc = newPlaneASC();
	scc = newZ85C80(true);
	vdac = std::make_shared<VDAC>(); 
	scsi = std::make_shared<Ncr5380>();

	bank_size[0] = simm30_table[ int(bankA) ];
	bank_size[1] = simm30_table[ int(bankB) ];

	switch(model) {
	case MODEL::IIci :
		model_map[0] = true;
		model_map[1] = true;
		model_map[2] = false;
		model_map[3] = true;
		break;
	case MODEL::IIci_PGC :
		model_map[0] = true;
		model_map[1] = true;
		model_map[2] = true;
		model_map[3] = true;
		break;
	case MODEL::IIsi :
		model_map[0] = true;
		model_map[1] = true;
		model_map[2] = true;
		model_map[3] = false;
		break;
	case MODEL::UNKNOWN :
		break;
	}
}

std::shared_ptr<IO_BASE> MDU::get_io(uint32_t base) {
	switch((base>>13) & 0x1f) {
	case 0 : return via1;
	case 1 :
		if( via2 != IO_NONE::SELF ) {
			return via2;
		} else {
			return {};
		}
	case 2 : return scc;
	case 6 : return scc;
	case 8 : return scsi;
	case 10 : return asc;
	case 12 : return scsi; // DMA ?
	case 18 : return vdac;
	case 19 : return rbv;
	default : return {};
	}
}
uint8_t MDU::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 1 :
		if( via2 != IO_NONE::SELF ) {
			return via2->read(addr >> 9 & 0xf);
		} else {
			bus_error(addr, attr);
		}
	case 2  : return scc->read(addr>>1&3);
	case 6  : return scc->read(addr>>1&0x1ff);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	case 12  : return scsi->read(addr>>4&7); // DMA?
	case 18 : return vdac->read(addr>>2&7);
	case 19 : return rbv->read((addr&3)|(addr>>2&4));
	default : bus_error(addr, attr);
	}
}

void MDU::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 1 :
		if( via2 != IO_NONE::SELF ) {
			return via2->write(addr >> 9 & 0xf, v);
		} else {
			bus_error(addr, attr);
		}
	case 2  : return scc->write(addr>>1&3, v);
	case 6  : return scc->write(addr>>1&0xff, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	case 12  : return scsi->write(addr>>4&7, v); // DMA ?
	case 18 : return vdac->write(addr>>2&7, v);
	case 19 : return rbv->write((addr&3)|(addr>>2&4), v);
	default : bus_error(addr, attr);
	}
}

void* MDU::get_ram_addr(uint32_t addr, int attr) {
	if( addr < bank_size[0] ) {
		return &RAMBaseHost[addr];
	} else if( addr & 0x04000000 && ( addr & 0x03ffffff) < bank_size[1] ) {
		return &RAMBaseHost[ bank_size[0] + (addr& 0x03ffffff)];
	} else {
		return nullptr;
	}
}
