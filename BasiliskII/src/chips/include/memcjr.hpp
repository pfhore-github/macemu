#pragma once
#include "io.hpp"
// @50F0E000
class MEMCjr_REG1 : public IO_BASE {
public:
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};
// @50F18000
class MEMCjr_REG2 : public IO_BASE {
public:
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};
class SCC;

class MEMCjr : public Machine {
public:
	MEMCjr();
	std::shared_ptr<IO_BASE> get_io(uint32_t base);
	uint8_t io_read_b(uint32_t addr, int attr);
	void io_write_b(uint32_t addr, uint8_t v, int attr);
};
