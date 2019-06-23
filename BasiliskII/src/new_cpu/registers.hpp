#pragma once
#include <stdint.h>
#include "mmu.hpp"
#include "SDL_endian.h"
struct FPU_EX {
	bool INEX1;
	bool INEX2;
	bool DZ;
	bool UNFL;
	bool OVFL;
	bool OPERR;
	bool snan;
	bool bsun;
};

struct TTC {
	enum CPU { X030, X040 } cpu;
	union {
		struct MC68030 {
			uint8_t FC_MASK;
			uint8_t FC_BASE;
			bool RWM;
			bool RW;
			bool CI;
			bool E;
		} mc68030;
		struct MC68040 {
			bool W;
			uint8_t CM;
			uint8_t U;
			uint8_t S;
			bool E;
		} mc68040;
	} regs;
};

struct FPU {
	long double FP[8];
	struct {
		enum { NEAREST = 0, ZERO, M_INF, P_INF } RND;
		enum { EXTENDED = 0, SINGLE, DOUBLE } PREC;
		FPU_EX ex_enable;
	} FPCR;
	struct {
		bool nan;
		bool I;
		bool Z;
		bool N;
		int16_t quotient;
		FPU_EX ex;
		bool INEX;
		bool DZ;
		bool UNFL;
		bool OFVL;
		bool IOP;
	} FPSR;
	uint32_t FPIAR;
};

struct CPU {
	// register
	union {
		struct {
			uint32_t D[8];
			uint32_t A[8];
		};
		uint32_t R[16];
	};			
	uint32_t USP;
	uint32_t PC;
	uint32_t NPC;
	bool X;
	bool N;
	bool Z;
	bool V;
	bool C;
	uint32_t ISP;
	uint32_t MSP;
	uint8_t IX;
	bool M;
	bool S;
	uint8_t T;
	uint32_t VBR;
	uint32_t SFC;
	uint32_t DFC;
	uint16_t ACUSR;
	TTC ttc;
	FPU* fpu;
	MMU* mmu;
	uint8_t intmask;
	void setD_B(int n, uint8_t v) {
		D[n] = (D[n] &~ 0xff) | v;
	}
	void setD_W(int n, uint16_t v) {
		D[n] = (D[n] &~ 0xffff) | v;
	}
	void setA_W(int n, int16_t v) {
		A[n] = v;
	}
	int16_t getA_W(int n) { return (int16_t)A[n]; }
	
	void push16(uint16_t v) {
		mmu->write_w(A[7] -= 2, v);
	}
	
	void push32(uint32_t v) {
		mmu->write_l(A[7] -= 4, v);
	}
	
	uint16_t pop16() {
		uint16_t v = mmu->read_w(A[7]);
		A[7] += 2;
		return v;
	}

	uint32_t pop32() {
		uint32_t v = mmu->read_l(A[7]);
		A[7] += 4;
		return v;
	}

	uint16_t fetch_w() {
		uint16_t op = READ16(mmu->to_real(PC, 2, false, 2 | ( S << 2 ) ));
		PC += 2;
		return op;
	}
	uint32_t fetch_l() {
		uint32_t op = READ32(mmu->to_real(PC, 4, false, 2 | ( S << 2 ) ));
		PC += 4;
		return op;
	}
	uint32_t ea_ax(uint32_t ax);
	void reset();
	void init();
	void irq(int level);
};

inline int MMU::mode(int c, bool intr) {
	return intr ? 7 : c | ( cpu->S << 2 );
}
