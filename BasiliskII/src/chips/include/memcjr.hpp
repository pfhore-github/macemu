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
	enum MODEL {
		Q605_20, Q605_25, Q605_33, UNUSED1, UNUSED2,
		UNUSED3, C650, Q800, Q650, UNUSED4, UNUSED5, C610, Q610, UNUSED6, UNUSED7
	};
	MEMCjr(MODEL model = MODEL::Q605_33);
	std::shared_ptr<IO_BASE> get_io(uint32_t base);
	uint8_t io_read_b(uint32_t addr, int attr);
	void io_write_b(uint32_t addr, uint8_t v, int attr);
};
