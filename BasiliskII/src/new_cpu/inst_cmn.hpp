#ifndef INST_CMN_HPP
#define INST_CMN_HPP
void ILLEGAL(CPU* cpu) __attribute__((noreturn));

inline void test_trace_branch(CPU* cpu) {
	if( cpu->T == 1 ) {
		TraceEx e;
		e.run(cpu, cpu->NPC);
	}
}
inline uint8_t get_cr(CPU* regs) {
	return regs->X << 4 |
		regs->N << 3 |
		regs->Z << 2 |
		regs->V << 1 |
		regs->C;
}

inline void set_cr(CPU* regs, uint8_t v) {
	regs->X = (v >> 4) & 1;
	regs->N = (v >> 3) & 1;
	regs->Z = (v >> 2) & 1;
	regs->V = (v >> 1) & 1;
	regs->C = v & 1;
}

inline uint16_t get_sr(CPU* regs) {
	uint16_t low = get_cr(regs);
	low |=
		regs->IX << 8 |
		regs->M << 12 |
		regs->S << 13 |
		regs->T << 14;
	return low;
}
inline void save_sp(CPU* regs) {
	if( ! regs->S ) {
		regs->ISP = regs->A[7];
	} else if( regs->M ) {
		regs->MSP = regs->A[7];
	} else {
		regs->ISP = regs->A[7];
	}
}
inline void load_sp(CPU* regs) {
	if( ! regs->S ) {
		regs->A[7] = regs->ISP;
	} else if( regs->M ) {
		regs->A[7] = regs->MSP;
	} else {
		regs->A[7] = regs->ISP;
	}
}
inline void set_sr(CPU* regs, uint16_t v) {
	save_sp(regs);
	set_cr(regs, v);
	regs->IX =(v >> 8) & 3;
	regs->M =(v >> 12) & 1;
	regs->S =(v >> 13) & 1;
	regs->T =(v >> 14) & 3;
	load_sp(regs);
}
inline void set_nz(CPU* regs, int8_t v) {
	regs->N = v < 0;
	regs->Z = !v;
}
inline void set_nz(CPU* regs, int16_t v) {
	regs->N = v < 0;
	regs->Z = !v;
}
inline void set_nz(CPU* regs, int32_t v) {
	regs->N = v < 0;
	regs->Z = !v;
}






#endif
