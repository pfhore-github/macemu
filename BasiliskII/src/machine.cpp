#include "machine.hpp"
#include "adb.h"
#include "memcjr.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "asc.hpp"
#include "glu.hpp"
#include "scc.hpp"
#include "rbv.hpp"
#include "oss.hpp"
#include "mcu.hpp"
#include "jaws.hpp"
#include "msc.hpp"
#include "ncr5380.hpp"
#include "sonic.hpp"
#include "SDL_endian.h"
#include "scc_impl.hpp"
std::shared_ptr<IO_BASE> Machine::get_via2() {
	if( via2 )
		return via2;
	if( rbv )
		return rbv;
	return {};
}
std::shared_ptr<IO_BASE> IO_NONE::SELF = std::make_shared<IO_NONE>();
Machine::Machine() {
	via2 = IO_NONE::SELF;
	iop = IO_NONE::SELF;
	rbv = IO_NONE::SELF;
	vdac = IO_NONE::SELF;
	ether = IO_NONE::SELF;
	// machine specific
	oss  = IO_NONE::SELF;
	exp0  = IO_NONE::SELF;
	exp1   = IO_NONE::SELF;
	pb  = IO_NONE::SELF;
	pb2  = IO_NONE::SELF;
	mcu  = IO_NONE::SELF;
	reg1  = IO_NONE::SELF;
	reg2  = IO_NONE::SELF;
}
// TODO

class V8 : public Machine {
public:
	V8() {
		auto via1_ = std::make_shared<VIA1>(1);
		via1 = via1_;
		// TODO
		auto adb_bus_via = std::make_shared<ADB_VIA>();
		adb_bus = adb_bus_via;
		
		rbv = std::make_shared<RBV>();
		asc = newV8EASC(0xE8);

		scc = std::make_shared<SCC>(std::make_shared<Z85C80>());		
		scsi = std::make_shared<Ncr5380>();
		vdac = std::make_shared<VDAC>(); 
	}
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override {
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
	uint8_t io_read_b(uint32_t addr, int attr) override {
		switch((addr>>13) & 0x1f) {
		case 0 : return via1->read(addr >> 9 & 0xf);
		case 2  : return scc->read(addr>>1&3);
		case 8  : return scsi->read(addr>>4&7);
		case 10 : return asc->read(addr&0xfff);
		case 18 : return vdac->read(addr>>2&7);
		case 19 : return rbv->read((addr&3)|(addr>>2&4));
		default : bus_error(addr, attr);
		}
	}
	void io_write_b(uint32_t addr, uint8_t v, int attr) override {
		switch((addr>>13) & 0x1f) {
		case 0 : return via1->write(addr >> 9 & 0xf, v);
		case 2  : return scc->write(addr>>1&3, v);
		case 8  : return scsi->write(addr>>4&7, v);
		case 10 : return asc->write(addr&0xfff, v);
		case 18 : return vdac->write(addr>>2&7, v);
		case 19 : return rbv->write((addr&3)|(addr>>2&4), v);
		default : bus_error(addr, attr);
		}
	}
};

MDU::MDU() {
	auto adb_via_bus = std::make_shared<ADB_VIA>();
	adb_bus = adb_via_bus;
	via1 = std::make_shared<VIA1>(1);
	via2 = rbv = std::make_shared<RBV>();
	asc = newPlaneASC();
	scc = std::make_shared<SCC_IOP>(std::make_shared<Z85C80>());
	vdac = std::make_shared<VDAC>(); 
	scsi = std::make_shared<Ncr5380>();
	// connect VIA1 <-> ADB
	
	via1->adb_bus = adb_via_bus ;
	adb_via_bus->via = via1;
}

std::shared_ptr<IO_BASE> MDU::get_io(uint32_t base) {
	switch((base>>13) & 0x1f) {
	case 0 : return via1;
	case 6 : return scc;
	case 8 : return scsi;
	case 10 : return asc;
	case 18 : return vdac;
	case 19 : return rbv;
	default : return {};
	}
}
uint8_t MDU::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 6  : return scc->read(addr>>1&3);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	case 18 : return vdac->read(addr>>2&7);
	case 19 : return rbv->read((addr&3)|(addr>>2&4));
	default : bus_error(addr, attr);
	}
}

void MDU::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 6  : return scc->write(addr>>1&3, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	case 18 : return vdac->write(addr>>2&7, v);
	case 19 : return rbv->write((addr&3)|(addr>>2&4), v);
	default : bus_error(addr, attr);
	}
}

Sonora::Sonora() {
	via1 = std::make_shared<VIA1>(1);
	auto adb_via_bus = std::make_shared<ADB_VIA>();
	adb_bus = adb_via_bus;
	rbv = std::make_shared<RBV>();
	asc = newSonoraEASC();
	scc = std::make_shared<SCC>(std::make_shared<Z85C80>());
	vdac = std::make_shared<VDAC>(); 
	scsi = std::make_shared<Ncr5380>();
	// connect VIA1 <-> ADB
	auto via1_tmp = std::dynamic_pointer_cast<VIA1>(via1);
	via1_tmp->adb_bus = adb_via_bus ;
	adb_via_bus->via = via1;
}

std::shared_ptr<IO_BASE> Sonora::get_io(uint32_t base) {
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
uint8_t Sonora::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 2  : return scc->read(addr>>1&3);
	case 8  : return scsi->read(addr>>4&7);
	case 10 : return asc->read(addr&0xfff);
	case 18 : return vdac->read(addr>>2&7);
	case 19 : return rbv->read(addr&0xf);
	default : bus_error(addr, attr);
	}
}

void Sonora::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x1f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 2  : return scc->write(addr>>1&3, v);
	case 8  : return scsi->write(addr>>4&7, v);
	case 10 : return asc->write(addr&0xfff, v);
	case 18 : return vdac->write(addr>>2&7, v);
	case 19 : return rbv->write(addr&0xf, v);
	default : bus_error(addr, attr);
	}
}



void init_machine(MB_TYPE type) {
		
	switch(type) {
	case MB_TYPE::GLU : // SE/30, II, IIx, IIcx
		machine = std::make_unique<GLU>();
		break;
	case MB_TYPE::MDU : // IIci/ IIsi
		machine = std::make_unique<MDU>();
		break;
	case MB_TYPE::OSS : // IIfx
		machine = std::make_unique<OSS>();
		break;
	case MB_TYPE::V8 : // LC/IIvx/IIvi
		machine = std::make_unique<V8>();
		break;
	case MB_TYPE::MCU:			// Early Quadra 
		machine = std::make_unique<MCU>();
		break;
	case MB_TYPE::JAWS:			// PB 14x/170
		machine = std::make_unique<JAWS>();
		break;
	case MB_TYPE::MSC:			// PB 150/Duo 2x0
		machine = std::make_unique<MSC>();
		break;
	case MB_TYPE::NIAGARA:		// PB 16x/180
		machine = std::make_unique<Niagara>();
		break;
	case MB_TYPE::MEMCjr:		// Quadra 605/LC475/575
		machine = std::make_unique<MEMCjr>();
		break;
	case MB_TYPE::SONORA:		// LCIII/LC5x0
		machine = std::make_unique<Sonora>();
		break;
		
	}
	machine->model_map[0] = machine->model_map[1] = machine->model_map[2] = machine->model_map[3] = false;
}

std::unique_ptr<Machine> machine;
