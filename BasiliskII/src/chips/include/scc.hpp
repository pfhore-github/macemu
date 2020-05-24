#pragma once
#include "iop.hpp"
#include "devices/serial.hpp"
#include <memory>
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
class SCC : virtual public IO_BASE {
	std::shared_ptr<SCC_impl> modem, printer;
public:
	SCC(const std::shared_ptr<SCC_impl>& o);
	bool wait_request();
	// reset
	void reset();
	// connect
	void connect_modem(const std::shared_ptr<SerialDevice>&);
	void connect_printer(const std::shared_ptr<SerialDevice>&);
	// input signal
	void r_xd(SerialDevice& dev, uint8_t v);
	void hsk_i(SerialDevice& dev);
	void gp_i(SerialDevice& dev, bool v);
	
	uint8_t read(int) override;
	void write(int, uint8_t) override;
	void irq();
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
