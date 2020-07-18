#include <stdint.h>
#include "io.hpp"
#include "registers.hpp"
#include "machine.hpp"
#include "memcjr.hpp"
#include "via1.hpp"
#include "via2.hpp"
#include "asc.hpp"
#include "scc.hpp"
#include "sonic.hpp"
#include "z8530.hpp"
void QuitEmulator();
uint8_t MEMCjr_REG1::read(int addr) {
	return 0;
}

void MEMCjr_REG1::write(int addr, uint8_t v) {
}
uint8_t MEMCjr_REG2::read(int addr) {
	return 0;
}

void MEMCjr_REG2::write(int addr, uint8_t v) {
}

MEMCjr::MEMCjr(MODEL model) {
	via1 = std::make_shared<VIA1>();
	via2 = std::make_shared<VIA2>();
	asc = newPlaneASC();
	scc = newZ8530();
	reg1 = std::make_shared<MEMCjr_REG1>();
	reg2 = std::make_shared<MEMCjr_REG2>();
	ether = std::make_shared<SONIC>();
	switch(model) {
	case MODEL::Q605_20 : model_id = 0xA55A2224; break;
	case MODEL::Q605_25 : model_id = 0xA55A2225; break;
	case MODEL::Q605_33 : model_id = 0xA55A2226; break;
	case MODEL::UNUSED1 : model_id = 0xA55A2231; break;
	case MODEL::UNUSED2 : model_id = 0xA55A2232; break;
	case MODEL::UNUSED3 :
		model_id = 0xA55A2BAD;
		model_map[0] = true;
		model_map[1] = false;
		model_map[2] = false;
		model_map[3] = true;
		break;
	case MODEL::C650 :
		model_id = 0xA55A2BAD;
		model_map[0] = true;
		model_map[1] = true;
		model_map[2] = false;
		model_map[3] = true;
		break;
	case MODEL::Q800 :
		model_id = 0xA55A2BAD;
		model_map[0] = true;
		model_map[1] = false;
		model_map[2] = true;
		model_map[3] = false;
		break;
	case MODEL::Q650 :
		model_id = 0xA55A2BAD;
		model_map[0] = true;
		model_map[1] = false;
		model_map[2] = true;
		model_map[3] = true;
		break;
	case MODEL::UNUSED4 :
		model_id = 0xA55A2BAD;
		model_map[0] = true;
		model_map[1] = true;
		model_map[2] = true;
		model_map[3] = true;
		break;
	case MODEL::UNUSED5 :
		model_id = 0xA55A2BAD;
		model_map[0] = true;
		model_map[1] = true;
		model_map[2] = true;
		model_map[3] = false;
		break;
	case MODEL::C610 :
		model_id = 0xA55A2BAD;
		model_map[0] = false;
		model_map[1] = false;
		model_map[2] = false;
		model_map[3] = true;
		break;
	case MODEL::Q610 :
		model_id = 0xA55A2BAD;
		model_map[0] = false;
		model_map[1] = true;
		model_map[2] = false;
		model_map[3] = true;
		break;
	case MODEL::UNUSED6 :
		model_id = 0xA55A2BAD;
		model_map[0] = false;
		model_map[1] = true;
		model_map[2] = false;
		model_map[3] = false;
		break;
	case MODEL::UNUSED7 :
		model_id = 0xA55A2BAD;
		model_map[0] = false;
		model_map[1] = false;
		model_map[2] = true;
		model_map[3] = true;
		break;
	}
	RAMBaseHost[0x78] = 'r';
	RAMBaseHost[0x79] = 'o';
	RAMBaseHost[0x7A] = 'm';
	RAMBaseHost[0x7B] = 'p';

}

std::shared_ptr<IO_BASE> MEMCjr::get_io(uint32_t base) {
	switch((base>>13)&0x7f) {
	case 0 : return via1;
	case 1 : return via2;
	case 4 : return scc;
	case 5 : return ether;
	case 7 : return reg1;
	case 10 : return asc;
	case 12 : return reg2;
	default : return {};
	}
}
uint8_t MEMCjr::io_read_b(uint32_t addr, int attr) {
	switch((addr>>13) & 0x7f) {
	case 0 : return via1->read(addr >> 9 & 0xf);
	case 1 : return via2->read(addr >> 9 & 0xf);
	case 4  : return scc->read(addr>>1&3);
	case 5  : return ether->read(addr & 0x1fff);
	case 7  : return reg1->read(addr & 0x1fff);
	case 10 : return asc->read(addr&0xffff);
	case 12 : return reg2->read(addr & 0x1fff);
	default : bus_error(addr, attr);
	}
}
void MEMCjr::io_write_b(uint32_t addr, uint8_t v, int attr) {
	switch((addr>>13) & 0x7f) {
	case 0 : return via1->write(addr >> 9 & 0xf, v);
	case 1 : return via2->write(addr >> 9 & 0xf, v);
	case 4  : return scc->write(addr>>1&3, v);
	case 5  : return ether->write(addr & 0x1fff, v);
	case 7  : return reg1->write(addr & 0x1fff, v);
	case 10 : return asc->write(addr&0xffff, v);
	case 12 : return reg2->write(addr & 0x1fff, v);
	default : bus_error(addr, attr);
	}
}
