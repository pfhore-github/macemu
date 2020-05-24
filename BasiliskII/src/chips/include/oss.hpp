#pragma once
#include "io.hpp"
#include <memory>
#include "machine.hpp"
class SCC_IOP;
class OSS_REG : public IO_BASE {
	uint8_t irq_level[0x10];
	uint16_t irq_pending;
	uint8_t rom_ctrl;
	uint8_t ack_60hz;
public:
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};
// TODO
class IIFX_exp0 : public IO_BASE {
	uint16_t val;
	uint16_t reg2[256];	
public:
	uint16_t UT_IIFX_exp0_GET(uint8_t addr) { return reg2[addr]; }
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};
class IIFX_exp1 : public IO_BASE {
	bool ptr;
	uint8_t k[2];
public:
	uint8_t read(int addr) override;
	void write(int addr, uint8_t v) override;
};

struct OSS : public Machine {	
	bool is_scsi_addr(uint32_t addr) {
		return (( addr>>13) & 0x1f) >= 4 && ((addr>>13) & 0x1f) <= 7;
	}
	OSS();
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
	uint16_t io_read_w(uint32_t addr, int attr) override;
	uint32_t io_read_l(uint32_t addr, int attr) override;
    void io_write_w(uint32_t addr, uint16_t v, int attr) override;
    void io_write_l(uint32_t addr, uint32_t v, int attr) override;
};

