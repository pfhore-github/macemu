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
#include "f108.hpp"
#include "memcjr.hpp"
#include "sonic.hpp"
#include "rbv.hpp"
uint8_t F108_REG::read(int addr) {
	return 0;
}

void F108_REG::write(int addr, uint8_t v) {
}
F108::F108(MODEL model) {
	via1 = std::make_shared<VIA1>(1);
	via2 = std::make_shared<VIA2>(2);
	asc = newSonoraEASC();
	scc = newZ85C80();
	vdac = std::make_shared<VDAC>(); 
	scsi = std::make_shared<Ncr5380>();
	ether = std::make_shared<SONIC>();
	reg1 = std::make_shared<MEMCjr_REG1>();
	reg2 = std::make_shared<MEMCjr_REG2>();
	f108_reg = std::make_shared<F108_REG>();

	switch(model) {
	case MODEL::LC630_1: model_id = 0xa55a2252; break;
	case MODEL::LC630_2: model_id = 0xa55a2253; break;
	case MODEL::LC630_3: model_id = 0xa55a2250; break;
	case MODEL::LC630_4: model_id = 0xa55a2256; break;
	case MODEL::LC630_5: model_id = 0xa55a2257; break;
	case MODEL::LC630_6: model_id = 0xa55a2254; break;
	case MODEL::LC580_1: model_id = 0xa55a225A; break;
	case MODEL::LC580_2: model_id = 0xa55a225B; break;
	case MODEL::LC580_3: model_id = 0xa55a2258; break;
	}
	RAMBaseHost[0x78] = 'r';
	RAMBaseHost[0x79] = 'o';
	RAMBaseHost[0x7A] = 'm';
	RAMBaseHost[0x7B] = 'p';

}

std::shared_ptr<IO_BASE> F108::get_io(uint32_t base) {
	switch((base>>13) & 0x1f) {
	case 0 : return via1;
	case 1 : return via2;
	case 2 : return scc;
	case 5 : return ether;
	case 7 : return reg1;
	case 8 : return scsi;
	case 10 : return asc;
	case 12 : return reg1;
	case 13 : return f108_reg;
	case 18 : return vdac;
	default : return {};
	}
}
uint8_t F108::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 1 : return via2->read(addr >> 9 & 0xf);
	case 2  : return scc->read(addr>>1&3);
	case 5  : return ether->read(addr);
	case 7  : return reg1->read(addr & 0x1fff);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	case 12 : return reg2->read(addr & 0x1fff);
	case 13 : return f108_reg->read(addr & 0x1fff);
	case 18 : return vdac->read(addr>>2&7);
	default : bus_error(addr, attr);
	}
}

void F108::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 1 : return via2->write(addr >> 9 & 0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 5  : return ether->write(addr, v);
	case 7  : return reg1->write(addr & 0x1fff, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	case 12 : return reg2->write(addr & 0x1fff, v);
	case 13 : return f108_reg->write(addr & 0x1fff, v);
	case 18 : return vdac->write(addr>>2&7, v);
	default : bus_error(addr, attr);
	}
}
