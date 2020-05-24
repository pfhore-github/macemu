#pragma once
#include "io.hpp"
#include "machine.hpp"
class MCU_REG : public IO_BASE {
	uint32_t reg[0x100>>2];	
public:
	MCU_REG();
	uint32_t UT_GET_REG(int addr) { return reg[addr]; }
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};
// TODO
class MCU : public Machine {
public:
	MCU();
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;

};
