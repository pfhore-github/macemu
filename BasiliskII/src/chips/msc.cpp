#include "io.hpp"
#include "msc.hpp"
#include "asc.hpp"
#include "ncr5380.hpp"
#include "rbv.hpp"
#include "mem.hpp"
#include "scc.hpp"
#include "iop.hpp"
#include "scc_impl.hpp"
// TODO
void MSC_VIA1::cb2_out(bool v) {
	base->msc->c_in = base->msc->c_in << 1 | v;
	++base->msc->c_in_c;
}

MSC_VIA1::MSC_VIA1(MSC* parent) :VIA1(1), base(parent) {}

MSC::MSC() {	
	via1 = std::make_shared<MSC_VIA1>(this);
	via2 = rbv = std::make_shared<MSCRbv>(this);
	// TODO
	asc = newEASC();
	msc = std::make_shared<MSC_REG>();
	scc = std::make_shared<SCC>(std::make_shared<Z85C80>());
	scsi = std::make_shared<Ncr5380>();
	vdac = std::make_shared<VDAC>(); 
}


std::shared_ptr<IO_BASE> MSC::get_io(uint32_t base) {
	switch((base>>13) & 0x1f) {
	case 0 : return via1;
	case 2 : return scc;
	case 8 : return scsi;
	case 10 : return asc;
	case 18 : return vdac;
	case 19 : return rbv;
	default : return {};
	}		
}

uint8_t MSC::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 2  : return scc->read(addr>>1&3);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xffff);
	case 18 : return vdac->read(addr>>2&7);
	case 19 : return rbv->read( addr & 0xff);
	default : bus_error(addr, attr);
	}
}

void MSC::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xffff, v);
	case 18 : return vdac->write(addr>>2&7, v);
	case 19 : return rbv->write(addr & 0xff, v);
	default : bus_error(addr, attr);
	}
}

bool MSCRbv::readB(int n) {
	switch(n) {
	case 1 : // transport complete
		return base->msc->ready;
	default:
		return RBV::readB(n);
	}
}
void MSCRbv::writeB(int n, bool v) {
	switch( n ) {
	case 2 : // transport mode
		if( v ) {
			// read
			for(int i = 7; i >= 0; --i ) {
				machine->via1->cb2_in_push(base->msc->c_out >> i & 1);
			}
			base->msc->ready = true;
		} else {
			// write
			base->msc->ready = false;
			if( base->msc->c_in_c == 8 ) {
				base->msc->c_out = base->msc->cmd(base->msc->c_in);
				base->msc->c_in = 0;
				base->msc->c_in_c = 0;
			}
		}
		break;
	default:
		RBV::writeB(n, v);
		break;
	}
}
#include <stdio.h>
uint8_t MSC_REG::cmd(uint8_t c) {
	if( firmware_pos != -1 ) {
		if( firmware_pos < 0x8003 ) {
			firmware[firmware_pos++] = c;
		} else {
			firmware_loaded = true;
			firmware_pos = -1;
		}
		return 0;
	}
	switch(c) {
	case 0xE1:
		// send firmware
		firmware_pos = 0;
		return 0;
	case 0xE2:
		// ROM VERSION?
		return firmware_loaded ? 0x62 : 0;
	default:
		printf("CMD: %02x\n", c);
		return 0;
	}
}

uint8_t MSCRbv::read(int addr) {
	switch(addr) {
	case 0x10 : return 0; // TODO memory address?
	default : return RBV::read((addr&3)|(addr>>2&4) | (addr&0x20)); 
	}
}	
void MSCRbv::write(int addr, uint8_t v) {
	switch(addr) {
	case 0x10 : return; // TODO memory address?
	default : RBV::write((addr&3)|(addr>>2&4) | (addr&0x20), v); return;
	}
}
