#pragma once
#include "iop.hpp"
#include "via.hpp"
#include "via1.hpp"
#include "rbv.hpp"
#include "machine.hpp"
// TODO
class MSC;
class MSC_VIA1 : public VIA1 {
	MSC* base;
	std::shared_ptr<IOP> iop;
protected:
	void cb2_out(bool v) override;
public:
	MSC_VIA1(MSC* parent);
};

class MSCRbv : public RBV {
	MSC* base;
protected:	
	bool readB(int n) override;
	void writeB(int n, bool v) override;
public:
	void write(int addr, uint8_t v) override;
	uint8_t read(int addr) override;
	MSCRbv(MSC* parent) :base(parent) {}
};

class MSC_REG {
	friend class MSC_VIA1;
	friend class MSCRbv;
	uint8_t firmware[0x8003];
	bool firmware_loaded = false;
	int firmware_pos = -1;
	uint8_t c_in = 0;	
	uint8_t c_out = 0;
	uint8_t c_in_c = 0;
	bool ready = true;
public:
	virtual uint8_t cmd(uint8_t c);
	uint32_t DEBUG_GET_C_IN() { return c_in; }
	uint8_t DEBUG_GET_C_OUT() { return c_out; }
	void DEBUG_SET_C_OUT(uint8_t v) { c_out = v; ready = false; }
};

class MSC : public Machine {
public:
	MSC();	
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
};

