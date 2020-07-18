#pragma once
#include "../registers.hpp"
#include "../common.hpp"

constexpr uint16_t EX_INEX1 = 1;
constexpr uint16_t EX_INEX2 = 1 << 1;
constexpr uint16_t EX_DZ = 1 << 2;
constexpr uint16_t EX_UNFL = 1 << 3;
constexpr uint16_t EX_OVFL = 1 << 4;
constexpr uint16_t EX_OPERR = 1 << 5;
constexpr uint16_t EX_SNAN = 1 << 6;
constexpr uint16_t EX_BSUN = 1 << 7;

constexpr uint16_t EX_ACC_INEX = 1 << 3;
constexpr uint16_t EX_ACC_DZ = 1 << 4;
constexpr uint16_t EX_ACC_UNFL = 1 << 5;
constexpr uint16_t EX_ACC_OVFL = 1 << 6;
constexpr uint16_t EX_ACC_IOP = 1 << 7;

constexpr uint8_t cc_nan = 1;
constexpr uint8_t cc_i = 1 << 1;
constexpr uint8_t cc_z = 1 << 2;
constexpr uint8_t cc_n = 1 << 3;
struct M68040_FPU : FPU {
	friend class fixture;
	long double FP[8];
	uint8_t FPSR_aex;
	uint8_t FPSR_ex;
	int8_t FPSR_quotient;
	uint8_t FPSR_cond;
	uint16_t FPCR;
	uint32_t FPIAR;
	
	using op_type = void (*)(uint16_t op, long double a, int b);
	op_type op_list_fpu[0x7f];
	M68040_FPU();
	void exec(uint16_t op, int mode, int reg) override;
	void reset();
	void test_fp(long double l);
	void set(int dstn, long double ld);
	uint16_t get_fpcr();
	void set_fpcr(uint16_t);
	uint32_t get_fpsr();
	void set_fpsr(uint32_t);
	uint32_t get_iar() { return FPIAR; }
	void set_iar(uint32_t iar) { FPIAR = iar; }
	void set(int dstn, double d) {
		test_fp(d);
		FP[dstn] = d;
	}
	void set(int dstn, float f) {
		test_fp(f);
		FP[dstn] = f;
	}
	bool cond(int op);

	// internal data
	bool idle;
	uint16_t cmdreg1b;
	uint8_t cmdreg3b;
	uint32_t cu_savepc;
	bool e1, e3;
	long double et;
	uint32_t fpiarcu;
	long double fpt;
	uint8_t opclass;
	uint8_t stag, dtag;
	bool t;
	long double wbt;

private:
	long double load_packed_decimal(const uint8_t raw[]);
	long double load_fd(bool rm, int t, int mode, int reg);
	void store_fd(int t, int mode, int reg, long double ld, int k);
	void store_packed_decimal(long double from, uint8_t raw[], int k);
	
	
	
};

void write_ld(uint32_t addr, long double ld);
long double read_ld( uint32_t addr);
