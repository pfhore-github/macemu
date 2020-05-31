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

MEMCjr::MEMCjr() {
	via1 = std::make_shared<VIA1>();
	via2 = std::make_shared<VIA2>();
	asc = newPlaneASC();
	scc = newZ8530();
	reg1 = std::make_shared<MEMCjr_REG1>();
	reg2 = std::make_shared<MEMCjr_REG2>();
	ether = std::make_shared<SONIC>();
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
