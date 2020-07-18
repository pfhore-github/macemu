#include <stdint.h>
#include "io.hpp"
#include "registers.hpp"
#include "lc.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "rbv.hpp"
#include "scc.hpp"
#include "machine.hpp"
#include "ncr5380.hpp"
#include "asc.hpp"
#include "z8530.hpp"
#include "intel.h"
#include "v8.hpp"
V8::V8(MODEL model) {
	via1 = std::make_shared<VIA1>(1);
		
	rbv = std::make_shared<RBV>();
	asc = newV8EASC(0xE8);

	scc = newZ85C80();		
	scsi = std::make_shared<Ncr5380>();
	vdac = std::make_shared<VDAC>();
	
	switch( model ) {
	case MODEL::LC :
	case MODEL::LC2 :
	case MODEL::IIvx :
	case MODEL::IIvi :
		model_map[0] = false;
		model_map[1] = true;
		model_map[2] = true;
		model_map[3] = true;
		break;
	case MODEL::ColorClassic :
		model_map[0] = true;
		model_map[1] = false;
		model_map[2] = false;
		model_map[3] = false;
		break;
	case MODEL::UNKNOWN :
		break;
	}
	model_id = 0;
}

std::shared_ptr<IO_BASE> V8::get_io(uint32_t base)  {
	switch((base>>13) & 0x3f) {
	case 0 : return via1;
	case 2 : return scc;
	case 8 : return scsi;
	case 10 : return asc;
	case 18 : return vdac;
	case 19 : return rbv;
	default : return {};
	}
}

uint8_t V8::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x3f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 2  : return scc->read(addr>>1&3);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	case 18 : return vdac->read(addr>>2&7);
	case 19 : return rbv->read((addr&3)|(addr>>2&4));
	default : bus_error(addr, attr);
	}
}

void V8::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x3f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	case 18 : return vdac->write(addr>>2&7, v);
	case 19 : return rbv->write((addr&3)|(addr>>2&4), v);
	default : bus_error(addr, attr);
	}
}
