#pragma once
#include <stdint.h>
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

struct FPU_REGS {
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

struct REGS {
	union {
		struct {
			uint32_t D[8];
			uint32_t A[8];
		};
		uint32_t R[16];
	};			
	uint32_t USP;
	uint32_t PC;
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
	uint32_t BVR;
	uint32_t SFC;
	uint32_t DFC;
	uint16_t ACUSR;
	TTC ttc;
	FPU_REGS fpu;
};


