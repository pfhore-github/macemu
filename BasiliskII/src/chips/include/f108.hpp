#pragma once
#include "io.hpp"
#include <memory>
#include "machine.hpp"
class F108_REG : public IO_BASE {
public:
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};
class F108 : public Machine {
public:
	enum MODEL {
		LC630_1, LC630_2, LC630_3, LC630_4, LC630_5, LC630_6, LC580_1, LC580_2, LC580_3 
	};
	F108(MODEL model = MODEL::LC630_1);	
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
};
