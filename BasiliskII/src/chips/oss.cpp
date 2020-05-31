#include <stdint.h>
#include "io.hpp"
#include "registers.hpp"
#include "oss.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "scc.hpp"
#include "machine.hpp"
#include "ncr5380.hpp"
#include "asc.hpp"
#include "z8530.hpp"
#include "intel.h"
void QuitEmulator();
namespace OSS_IRQ {
enum OSS_IRQ_t {
	NuBus = 0,
	IOP_ISM = 6,
	IOP_SCC,
	SOUND,
	SCSI,
	VBlank,
	VIA1,
	PARITY = 14,
};
}
std::shared_ptr<IO_BASE> OSS::get_io(uint32_t base) {
	switch((base>>13) & 0x1f) {
	case 0 : return via1;
	case 2 : return scc;
	case 4 :
	case 5 :
	case 6 :
	case 7 : return scsi;
	case 8 : return asc;
	case 13 : return oss;
	case 14 : return exp0;
	case 15 : return exp1;
	default : return {};
	}
}

uint8_t OSS::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 2  : return scc->read(addr>>1&0x1f);
	case 4 :
	case 5 :
	case 6 :
	case 7 : return scsi->read(addr);
	case 8 : return asc->read(addr&0xffff);
	case 13 : return oss->read((addr&0xf)|(addr>>5&0x10));
	case 14 : return exp0->read(addr & 0x1fff);
	case 15 : return exp1->read(addr & 0x1fff);
	default : bus_error(addr, attr);
	}
}

OSS::OSS() {
	via1 = std::make_shared<VIA1>(1);
	// TODO
	oss = std::make_shared<OSS_REG>();
	asc = newPlaneASC();
	scc = newZ8530( true );
	scsi = std::make_shared<Ncr5380Ext>();
	exp0 = std::make_shared<IIFX_exp0>();
	exp1 = std::make_shared<IIFX_exp1>();
}
void OSS::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : via1->write(addr >> 9 & 7,0xf); break;
	case 2  : scc->write(addr>>1&0x1f,v); break;
	case 4 :
	case 5 :
	case 6 :
	case 7 : scsi->write(addr,v); break;
	case 8 : asc->write(addr&0xffff,v); break;
	case 13 : oss->write((addr&0xf)|(addr>>5&0x10),v); break;
	case 14 : exp0->write(addr & 0x1fff,v); break;
	case 15 : exp1->write(addr & 0x1fff,v); break;
	default : bus_error(addr, attr);
	}		
}	
uint8_t OSS_REG::read(int addr) {
	switch( addr & 0x1f ) {
	case OSS_IRQ::NuBus :
	case OSS_IRQ::NuBus+1 :
	case OSS_IRQ::NuBus+2 :
	case OSS_IRQ::NuBus+3 :
	case OSS_IRQ::NuBus+4 :
	case OSS_IRQ::NuBus+5 :
	case OSS_IRQ::IOP_ISM :
	case OSS_IRQ::IOP_SCC :
	case OSS_IRQ::SOUND :
	case OSS_IRQ::SCSI :
	case OSS_IRQ::VBlank :
	case OSS_IRQ::VIA1 :
	case 12:
	case 13:
	case OSS_IRQ::PARITY :
	case 15:
		return irq_level[addr & 0x0f];
	case 0x12:
		return irq_pending >> 8;
	case 0x13:
		return irq_pending & 0xff;
	case 0x14 :
		return rom_ctrl;
	case 0x17 :
		return ack_60hz;
	default:
		return 0;
	}
}

void OSS_REG::write(int addr, uint8_t v) {
	switch( addr & 0x1f ) {
	case OSS_IRQ::NuBus :
	case OSS_IRQ::NuBus+1 :
	case OSS_IRQ::NuBus+2 :
	case OSS_IRQ::NuBus+3 :
	case OSS_IRQ::NuBus+4 :
	case OSS_IRQ::NuBus+5 :
	case OSS_IRQ::IOP_ISM :
	case OSS_IRQ::IOP_SCC :
	case OSS_IRQ::SOUND :
	case OSS_IRQ::SCSI :
	case OSS_IRQ::VBlank :
	case OSS_IRQ::VIA1 :
	case 12:
	case 13:
	case OSS_IRQ::PARITY :
	case 15:
		irq_level[addr & 0x0f] = v;
		break;
	case 0x12:
		irq_pending = v << 8 | (irq_pending & 0xff);
		break;
	case 0x13:
		irq_pending = v  | (irq_pending & 0xff00);
		break;
	case 0x14 :
		if( v & 0x80 ) {
			QuitEmulator();
		}
		rom_ctrl = v;
		break;
	case 0x17 :
		ack_60hz = v;
		break;
	}
}

uint16_t OSS::io_read_w(uint32_t addr, int attr) {
	if( is_scsi_addr(addr) ) {
		return scsi->read16(addr);
	} else {
		return Machine::io_read_w(addr, attr);
	}
}
uint32_t OSS::io_read_l(uint32_t addr, int attr) {
	if( is_scsi_addr(addr) ) {
		return scsi->read32(addr);
	} else {
		return Machine::io_read_l(addr, attr);
	}
}
void OSS::io_write_w(uint32_t addr, uint16_t v, int attr) {
	if( is_scsi_addr(addr) ) {
		scsi->write16(addr, v);
	} else {
		Machine::io_write_w(addr, v, attr);
	}
}
void OSS::io_write_l(uint32_t addr, uint32_t v, int attr) {
	if( is_scsi_addr(addr) ) {
		scsi->write32(addr, v);
	} else {
		Machine::io_write_l(addr, v, attr);
	}
}
uint8_t IIFX_exp0::read(int addr) {
	bool k = (addr>>4)&1;
	if( k ) {
		return 1;
	} else {
		return 0;
	}
	
}
void IIFX_exp0::write(int addr, uint8_t v) {
	bool k = (addr>>4)&1;
	if( k ) {
		reg2[v] = val;
		val = 0;
	} else {
		val = (val >> 1) | ( v & 1) << 15;
	}
}

uint8_t IIFX_exp1::read(int addr) {
	switch( addr ) {
	case 0 :
		return k[ptr];
	default :
		return 0;
	}
}
void IIFX_exp1::write(int addr, uint8_t v) {
	switch( addr ) {
	case 0 :
		k[ptr] = v;
		break;			
	case 0x10 :
		ptr = v == 0xff;
		break;
	default :
		break;
	}
}	
