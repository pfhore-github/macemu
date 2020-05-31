#pragma once
#include "iop.hpp"
#include "devices/serial.hpp"
#include <memory>
#include <atomic>
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
	std::atomic<uint8_t> vec_h;
	std::atomic<uint8_t> vec_l;
	std::atomic<uint8_t> vec_f;
	// modem port RR2
	std::atomic<uint8_t> int_pending;
	SCC_INT IUS;
	void write_MIC(uint8_t);
public:
	SCC(const std::shared_ptr<SCC_impl>& o);
	// reset
	void reset();
	// connect
	void connect_modem(const std::shared_ptr<SerialDevice>&);
	void connect_printer(const std::shared_ptr<SerialDevice>&);
	
	uint8_t read(int) override;
	void write(int, uint8_t) override;

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
