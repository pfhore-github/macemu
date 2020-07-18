#include "io.hpp"
#include "msc.hpp"
#include "asc.hpp"
#include "ncr5380.hpp"
#include "powerbook.hpp"

#include "rbv.hpp"
#include "mem.hpp"
#include "scc.hpp"
#include "iop.hpp"
#include "z8530.hpp"
#include "fpu/fpu.hpp"
#include "fpu/68881.hpp"
void MSC_VIA1::cb2_out(uint8_t v) {
	machine->pb_ex->c_in = v;
}
// no ADB
uint8_t MSC_VIA1::read(int n) {
	return VIA::read(n);
}

MSC_VIA1::MSC_VIA1(MSC* parent) :VIA1(1), base(parent) {}
static NuBus msc_nu_e = {
	msc_nu_e_read_b,
	nullptr,
	msc_nu_e_read_l,
	msc_nu_e_write_b,
	nullptr,
	nullptr
};
MSC::MSC(MODEL model) {	
	via1 = std::make_shared<MSC_VIA1>(this);
	via2 = rbv = std::make_shared<MSCRbv>(this);
	// TODO
	asc = newEASC();
	pb_ex = std::make_shared<PB_EX_REG>();
	scc = newZ85C80();
	scsi = std::make_shared<Ncr5380>();
	vdac = std::make_shared<VDAC>();
	switch( model ) {
	case MODEL::PB_Duo210 : model_id = 0xA55A1004; break;
	case MODEL::PB_Duo230 : model_id = 0xA55A1005; break;
	case MODEL::PB_Duo270c : model_id = 0xA55A1002; break;
	}
	if( fpu ) {
		delete fpu;
	}
	if( model == MODEL::PB_Duo270c ) {
		// TODO: actually 68882
		fpu = new M68040_FPU();
	} else {
		fpu = new NoFPU();
	}
	nubus[5] = &msc_nu_e;
	
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
	case 19 : return rbv->read((addr&3)|(addr>>2&0xC));
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
	case 19 : return rbv->write((addr&3)|(addr>>2&0xC) , v);
	default : bus_error(addr, attr);
	}
}

bool MSCRbv::readB(int n) {
	switch(n) {
	case PB_TRANS_READY : // transport complete
		return base->ready;
	default:
		return RBV::readB(n);
	}
}
void MSCRbv::writeB(int n, bool v) {
	switch( n ) {
	case PB_TRANS_READ_MODE : // transport mode
		if( v ) {
			if( ! std::exchange(tran_mode, v) ) {
				// read
				if( auto v2 = base->pb_ex->pop_out() ) {
					machine->via1->cb2_in_push_byte(*v2);
				}
			} 
			base->ready = true;
		} else {
			tran_mode = v;
			base->ready = false;
			// write
			if( base->pb_ex->c_in ) {
				base->pb_ex->cmd( *base->pb_ex->c_in ) ;
				base->pb_ex->c_in = {};
			}
		}
		break;
	default:
		RBV::writeB(n, v);
		break;
	}
}

void MSCRbv::write(int addr, uint8_t v) {
	switch(addr) {
	case 0x10 : return; // TODO memory address?
	default : RBV::write(addr, v); return;
	}
}

uint8_t MSCRbv::read(int addr) {
	switch(addr) {
	case 0x10 : return 0; // TODO memory address?
	default : return RBV::read(addr); 
	}
}	

uint32_t msc_nu_e_read_l(uint32_t v)
{
	switch( v ) {
	case 0xFFFFE4 : return 0x52757373; // "Russ"
	case 0xFFFFE8 : return 0x53574321; // "SWC2"
	}
	return 0;	
}
uint8_t msc_nu_e_read_b(uint32_t v) {
	switch( v ) {
	case 0xE00021 : return 0;
	case 0xE08000 : return 0;
	}
	return 0;
}
void msc_nu_e_write_b(uint32_t v, uint8_t c) {
	switch( v ) {
	case 0xE08000 :
		break;
	}
}
		
