#pragma once
#include "io.hpp"
#include "iop.hpp"
// TODO
class SWIM : virtual public IO_BASE {
public:
	uint8_t read(int) override { return 0; }
	void write(int, uint8_t) override { }
};
class SWIM_IOP : public IOP, public SWIM {
	uint8_t write_data;
	uint8_t write_mark;
	uint8_t write_crc;
	uint8_t wirte_param;
	uint8_t write_phase;
	uint8_t write_setup;
	uint8_t wrtie_zeros;
	uint8_t write_ones;
	uint8_t read_data;
	uint8_t read_mark;
	uint8_t read_error;
	uint8_t read_phase;
	uint8_t read_setup;
	uint8_t read_mode;
	uint8_t read_handshake;
	uint8_t read_hw(uint32_t addr) override { return 0; }
	void write_hw(uint32_t addr, uint8_t v) override {}

	uint8_t read(int addr) override { return IOP::read(addr); }
	void write(int addr, uint8_t v) override { IOP::write(addr, v); }
};
