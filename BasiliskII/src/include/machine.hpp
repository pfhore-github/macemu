#pragma once
#include <stdint.h>
#include <memory>
#include <unordered_map>
#include "io.hpp"
#include "mem.hpp"

/* machine.hpp */
/* machine specific data */
class IO_BASE;
class VIA;
class VIA1;
class SCC;
class ASC;
class ADB_Bus;
class IO_OSS;
class Ncr5380;
class IOP;
class SWIM;
class PB_EX_REG;
class F108_REG;
class IO_NONE : public IO_BASE {
public:
	static std::shared_ptr<IO_BASE> SELF;
	IO_NONE() {}
	uint8_t read(int ) override final { bus_error(0, ATTR_SZ_B); }
	void write(int , uint8_t ) override final { bus_error(0, ATTR_SZ_B | 1 << 2); }
};

struct NuBus {
	uint8_t (*read_b)(uint32_t);
	uint16_t (*read_w)(uint32_t);
	uint32_t (*read_l)(uint32_t);
	void (*write_b)(uint32_t, uint8_t);
	void (*write_w)(uint32_t, uint16_t);
	void (*write_l)(uint32_t, uint32_t);
};
enum class SIMM30_SIZE {
	NONE,
	SIZE_1MB,
	SIZE_4MB,
	SIZE_16MB,
	SIZE_64MB
};
struct Machine {
	Machine();
	uint32_t model_id = 0;
	bool model_map[4] = { false, false, false, false };
	bool model_map2 = false;
	bool rom_mirror = true;

	std::shared_ptr<VIA1> via1;
	std::shared_ptr<ASC> asc;
	std::shared_ptr<SCC> scc;
	std::shared_ptr<IO_BASE> scsi;
	std::shared_ptr<SWIM> floppy;
	// not required
	std::shared_ptr<IO_BASE> via2;
	std::shared_ptr<IO_BASE> iop;
	std::shared_ptr<IO_BASE> rbv;
	std::shared_ptr<IO_BASE> vdac;
	std::shared_ptr<IO_BASE> ether;
	// machine specific
	std::shared_ptr<PB_EX_REG> pb_ex;
	std::shared_ptr<IO_BASE> oss;
	std::shared_ptr<IO_BASE> exp0;
	std::shared_ptr<IO_BASE> exp1;
	std::shared_ptr<IO_BASE> pb;
	std::shared_ptr<IO_BASE> pb2;
	std::shared_ptr<IO_BASE> mcu;
	std::shared_ptr<IO_BASE> reg1;
	std::shared_ptr<IO_BASE> reg2;
	std::shared_ptr<IO_BASE> f108_reg;
	NuBus* nubus[6];
public:
	std::shared_ptr<IO_BASE> get_via2();
	virtual void* get_ram_addr(uint32_t addr, int attr);
	virtual std::shared_ptr<IO_BASE> get_io(uint32_t base) = 0;
	virtual uint8_t io_read_b(uint32_t addr, int attr) = 0;
	virtual uint16_t io_read_w(uint32_t addr, int attr) {
		return io_read_b(addr, attr) << 8 | io_read_b(addr + 1, attr);
	}
	virtual uint32_t io_read_l(uint32_t addr, int attr) {
		return io_read_b(addr, attr) << 24 |
			io_read_b(addr+1, attr) << 16 |
			io_read_b(addr+2, attr) << 8 |
			io_read_b(addr+3, attr);
	}
	virtual void io_write_b(uint32_t addr, uint8_t v, int attr) = 0;
	virtual void io_write_w(uint32_t addr, uint16_t v, int attr) {
		io_write_b(addr, v >> 8, attr);
		io_write_b(addr+1, v, attr);
	}
	virtual void io_write_l(uint32_t addr, uint32_t v, int attr) {
		io_write_b(addr, v >> 24, attr);
		io_write_b(addr+1, v >> 16, attr);
		io_write_b(addr+2, v >> 8, attr);
		io_write_b(addr+3, v, attr);
	}
	virtual ~Machine() {}
};
class PB_REG : public IO_BASE {
	uint8_t reg[256];
public:
	uint8_t read(int addr) override { return reg[addr&0xff]; }
	void write(int addr, uint8_t v) override { reg[addr&0xff] = v; }
};
//void init_machine(MB_TYPE type, int model = -1);
extern std::unique_ptr<Machine> machine;



struct SIMM {
	uint32_t sz;
	uint8_t bit;
	uint8_t id;
};
extern const SIMM simm30[4];
extern const SIMM simm30_l[3];
extern const SIMM simm64[3];
extern const SIMM simm72[8];

extern const uint32_t simm30_table[5];
template<class T> T* machine_is() { return dynamic_cast<T*>(machine.get()); } 
