#include "registers.hpp"
class M68040_FPU : FPU {
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
public:
	void exec(CPU* cpu, uint16_t op, int mode, int reg) override;
};

void M68040_FPU::exec(CPU* cpu, uint16_t op, int mode, int reg) {}
