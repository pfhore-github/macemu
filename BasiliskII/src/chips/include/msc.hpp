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
protected:
	void cb2_out(uint8_t v) override;
	uint8_t read(int n) override ;
	
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

class MSC : public Machine {
	bool ready = false;
	friend class MSCRbv;
public:
	enum class MODEL {
		PB_Duo210,
		PB_Duo230,
		PB_Duo270c,
	};
	MSC(MODEL model = MODEL::PB_Duo210);	
	std::shared_ptr<IO_BASE> get_io(uint32_t base) override;
	uint8_t io_read_b(uint32_t addr, int attr) override;
	void io_write_b(uint32_t addr, uint8_t v, int attr) override;
};


uint32_t msc_nu_e_read_l(uint32_t v);
void msc_nu_e_write_b(uint32_t v, uint8_t);
uint8_t msc_nu_e_read_b(uint32_t v);
