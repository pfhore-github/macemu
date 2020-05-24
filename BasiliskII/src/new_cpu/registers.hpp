#pragma once
#include <stdint.h>
#include "mem.hpp"
#include "mmu/mmu.hpp"
#include "fpu/fpu.hpp"
#include "SDL_endian.h"
#include "exceptions.hpp"
#include <atomic>
struct  CPU {

	// register
	uint32_t R[16];
	uint32_t USP;
	uint32_t PC;
	uint32_t NPC;
	bool X, N, Z, V, C;
	bool M, S;
	std::atomic<uint8_t> IX;
	uint8_t T;
	uint32_t ISP;
	uint32_t MSP;
	uint32_t VBR;
	uint32_t SFC;
	uint32_t DFC;
	uint32_t CACR;
	uint16_t ACUSR;

	uint32_t ea_v;
	bool trace;
	bool run;
	bool misaligned;
	// INTERNAL
	enum EA_MODE { REG_D, REG_A, ADDR, IMM } ea_mode;
	uint8_t ea_sz;	
	uint8_t intmask;
	bool restart;
	bool in_ex;
	void parse_EA(int mode, int sz, int n, bool w);
	uint32_t ea_read();
	void ea_write(uint32_t v);

	void setD_B(int n, uint8_t v) {	R[n] = (R[n] &~ 0xff) | v; }
	void setD_W(int n, uint16_t v) { R[n] = (R[n] &~ 0xffff) | v; }
	void setA_W(int n, int16_t v) {	R[8+n] = v; }
	int16_t getA_W(int n) { return (int16_t)R[8+n]; }
	
	void reset();
	void init();
	void irq(int level);
	void do_irq();
	void do_op();
	void dump();

	void return_from_exception();
	void change_PC(uint32_t new_pc);
};
// we don't support multi CPU/FPU/MMU(has any?)
extern CPU cpu;
extern FPU* fpu;
extern MMU* mmu;
inline void push16(uint16_t v) { write_w(cpu.R[15] -= 2, v); }
inline void push32(uint32_t v) { write_l(cpu.R[15] -= 4, v); }
inline uint16_t pop16() {
	uint16_t v = read_w(cpu.R[15]);
	cpu.R[15] += 2;
	return v;
}

inline uint32_t pop32() {
	uint32_t v = read_l(cpu.R[15]);
	cpu.R[15] += 4;
	return v;
}

inline uint16_t fetch_w() {
	uint16_t op;
	int attr = ATTR(cpu.S, true)|ATTR_SZ_W;
	bus_read(mmu->to_real(cpu.PC, attr), attr, &op);
	cpu.PC += 2;
	return op;
}
inline uint32_t fetch_l() {
	uint32_t op;
	int attr = ATTR(cpu.S, true)|ATTR_SZ_L;
	bus_read(mmu->to_real(cpu.PC, attr), attr, &op);
	cpu.PC += 4;
	return op;
}

