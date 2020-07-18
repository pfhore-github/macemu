#pragma once
#include "iop.hpp"
#include "devices/serial.hpp"
#include <memory>
#include <atomic>
#include <vector>
/* SCC (Z8530) */
class SCC_impl;
namespace SCC_REG {
enum SCC_REG_t {
	B_CMD = 0,
	A_CMD,
	B_DATA,
	A_DATA,	
};
}
enum class SCC_INT {
	NONE = -1,
	/* internal name */
	TRANS = 0,
	EXTERNAL = 1,
	RECV_AVAIL = 2,
	RECV_SPECIAL = 3,
	/* external name */
	TRANS_B = 0,
	EXTERNAL_B = 1,
	RECV_AVAIL_B = 2,
	RECV_SPECIAL_B = 3,
	TRANS_A = 4,
	EXTERNAL_A = 5,
	RECV_AVAIL_A = 6,
	RECV_SPECIAL_A = 7
};

class SCC : virtual public IO_BASE {
	std::shared_ptr<SCC_impl> modem, printer;
	// MIC
	bool VIS, NV, DLC, MIE, status_H, INTACK;
	std::atomic<uint8_t> vec_f;

	SCC_INT IUS;
	void write_MIC(uint8_t);
public:
	SCC(const std::shared_ptr<SCC_impl>& o);
	// reset
	void reset();
	// connect
	void connect_modem(const std::shared_ptr<SerialDevice>&);
	void connect_printer(const std::shared_ptr<SerialDevice>&);
	uint8_t get_int_vec();
	uint8_t read(int) override;
	void write(int, uint8_t) override;

	// HLE
	void write_all_data(bool port, const std::vector<uint8_t>& velues);
	
	void write_reg(bool is_modem, int reg, uint8_t value);
	uint8_t read_reg(bool is_modem, int reg);
	void write_regA(int reg, uint8_t value) { write_reg(true, reg, value); }
	void write_regB(int reg, uint8_t value) { write_reg(false, reg, value); }
	uint8_t read_regA(int reg) { return read_reg(true, reg); }
	uint8_t read_regB(int reg) { return read_reg(false, reg); }
	
	uint8_t interrupt_ack();
protected:
	// IRQ
	void interrupt(SCC_INT i);
	friend class SCC_impl;
};
// IOP enabled
class SCC_IOP : public IOP, public SCC {
	uint8_t read_hw(uint32_t addr) override { return SCC::read(addr >> 5); }
	void write_hw(uint32_t addr, uint8_t v) override { SCC::write(addr >> 5, v); }
public:
	template<class T>
	SCC_IOP(const std::shared_ptr<T>& chip) :SCC(chip) {}
	uint8_t read(int addr) override { return IOP::read(addr); }
	void write(int addr, uint8_t v) override { IOP::write(addr, v); }
};
