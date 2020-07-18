#pragma once
#include "io.hpp"
#include "machine.hpp"
class JAWS_REG : public IO_BASE {
public:
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};
class JAWS_REG2 : public IO_BASE {
public:
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};

class JAWS : public Machine {
public:
	JAWS(bool is_pb170 = true);
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
};

class Niagara : public Machine {
public:
	Niagara();
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
};

// TODO
