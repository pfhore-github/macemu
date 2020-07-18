#include <stdint.h>
#include "io.hpp"
#include "registers.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "scc.hpp"
#include "machine.hpp"
#include "ncr5380.hpp"
#include "asc.hpp"
#include "z8530.hpp"
#include "swim.hpp"
#include "intel.h"
#include "ardbeg.hpp"
#include "rbv.hpp"
#include "sonic.hpp"
Ardbeg::Ardbeg(MODEL model) {
	via1 = std::make_shared<VIA1>(1);
	rbv = std::make_shared<RBV>();
	asc = newSonoraEASC();
	scc = newZ85C80();
	vdac = std::make_shared<VDAC>(); 
	scsi = std::make_shared<Ncr5380>();
	ether = std::make_shared<SONIC>();

	switch(model) {
	case MODEL::LC520 : model_id = 0xA55A0100; break;
	case MODEL::LC550 : model_id = 0xA55A0101; break;
	}

}

std::shared_ptr<IO_BASE> Ardbeg::get_io(uint32_t base) {
	switch((base>>13) & 0x1f) {
	case 0 : return via1;
	case 2 : return scc;
	case 5 : return ether;
	case 8 : return scsi;
	case 10 : return asc;
	case 18 : return vdac;
	case 19 : return rbv;
	default : return {};
	}
}
uint8_t Ardbeg::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 2  : return scc->read(addr>>1&3);
	case 5  : return ether->read(addr);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	case 18 : return vdac->read(addr>>2&7);
	case 19 : return rbv->read((addr&0x3)|(addr & 0x10)>>2|((addr & 0xc) << 1));
	default : bus_error(addr, attr);
	}
}

void Ardbeg::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 5  : return ether->write(addr, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	case 18 : return vdac->write(addr>>2&7, v);
	case 19 : return rbv->write((addr&0x3)|(addr & 0x10)>>2|((addr & 0xc) << 1), v);
	default : bus_error(addr, attr);
	}
}
