#include <stdint.h>
#include "io.hpp"
#include "registers.hpp"
#include "sonic.hpp"
#include "ncr5380.hpp"
#include "rbv.hpp"
#include "scc.hpp"
#include "z8530.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "asc.hpp"
#include "mcu.hpp"
#include "machine.hpp"
#include <string.h>
void QuitEmulator();
extern uint8_t* RAMBaseHost;
MCU::MCU(MODEL model)  {
	via1 = std::make_shared<VIA1>(6);
	via2 = std::make_shared<VIA2>();
	asc = newPlaneASC();
	scc = newZ85C80(true);
	vdac = std::make_shared<VDAC>();
	mcu = std::make_shared<MCU_REG>();
	// TODO actually 53c96 
	scsi = std::make_shared<Ncr5380>();
	ether = std::make_shared<SONIC>();
	model_id = 0;
	switch( model ) {
	case MODEL::Q700 :
		model_map[0] = false;
		model_map[1] = false;
		model_map[2] = false;
		model_map[3] = true;
		break;
	case MODEL::Q900 :
		model_map[0] = false;
		model_map[1] = false;
		model_map[2] = true;
		model_map[3] = true;
		RAMBaseHost[0x78] = 'r';
		RAMBaseHost[0x79] = 'o';
		RAMBaseHost[0x7A] = 'm';
		RAMBaseHost[0x7B] = 'p';
		break;
	case MODEL::Q950 :
		model_map[0] = false;
		model_map[1] = false;
		model_map[2] = true;
		model_map[3] = false;
		break;
	}
}

std::shared_ptr<IO_BASE> MCU::get_io(uint32_t base) {
	switch((base>>13) & 0x1f) {
	case 0 : return via1;
	case 1 : return via2;
	case 5 : return ether;
	case 6 : return scc;
	case 7 : return mcu;
	case 8 : return scsi;
	case 10 : return asc;
	case 18 : return vdac;
	default : return {};
	}
}

uint8_t MCU::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 1 : return via2->read(addr >> 9 & 0xf);
	case 5  : return ether->read(addr);
	case 6  : return scc->read((addr & 0x3f)>>1);
	case 7  : return mcu->read(addr & 0xff);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	case 18 : return vdac->read(addr);
	default : bus_error(addr, attr);
	}
}

void MCU::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 1 : return via2->write(addr >> 9 & 0xf, v);
	case 5  : return ether->write(addr, v);
	case 6  : return scc->write((addr & 0x3f) >> 1, v);
	case 7  : return mcu->write(addr & 0xff, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	case 18 : return vdac->write(addr, v);
	default : bus_error(addr, attr);
	}
}
MCU_REG::MCU_REG() {
	memset(reg, 0, 0x100);
}
uint8_t MCU_REG::read(int addr) {
	return 0;
}

void MCU_REG::write(int addr, uint8_t v) {
	if(addr != 0xa0 ) {
		switch(addr&3) {
		case 0 : reg[addr>>2] = (reg[addr>>2] & 0x00ffffff) | (v<<24); break;
		case 1 : reg[addr>>2] = (reg[addr>>2] & 0xff00ffff) | (v<<16); break;
		case 2 : reg[addr>>2] = (reg[addr>>2] & 0xffff00ff) | (v<<8); break;
		case 3 : reg[addr>>2] = (reg[addr>>2] & 0xffffff00) | (v<<0); break;
		}
	} else {
	}
}
void* MCU::get_ram_addr(uint32_t addr, int attr) {
	return Machine::get_ram_addr(addr, attr);
}
