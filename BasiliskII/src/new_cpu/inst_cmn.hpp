#ifndef INST_CMN_HPP
#define INST_CMN_HPP

template<class T>
T op_or_cmn(REGS* regs, T a, T b) {
	T v = a | b;
	regs->N = v < 0;
	regs->Z = ! v;
	regs->V = false;
	regs->C = false;
	return v;
}

template<class T>
T op_and_cmn(REGS* regs, T a, T b) {
	T v = a & b;
	regs->N = v < 0;
	regs->Z = ! v;
	regs->V = false;
	regs->C = false;
	return v;
}

template<class T>
T op_xor_cmn(REGS* regs, T a, T b) {
	T v = a ^ b;
	regs->N = v < 0;
	regs->Z = ! v;
	regs->V = false;
	regs->C = false;
	return v;
}

inline uint8_t op_subb(REGS* regs, uint8_t a, uint8_t b) {
	int8_t c = a;
	asm volatile("subb %3, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 : "+g"(c), "=g"(regs->V), "=g"(regs->C)
				 : "g"(b));
	regs->X = regs->C;
	regs->Z = c == 0;
	regs->N = c < 0;
	return c;
}

inline uint16_t op_subw(REGS* regs, uint16_t a, uint16_t b) {
	int16_t c = a;
	asm volatile("subw %3, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 : "+g"(c), "=g"(regs->V), "=g"(regs->C)
				 : "g"(b));
	regs->X = regs->C;
	regs->Z = c == 0;
	regs->N = c < 0;
	return c;
}

inline uint32_t op_subl(REGS* regs, uint32_t a, uint32_t b) {
	int32_t c = a;
	asm volatile("subl %3, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 : "+g"(c), "=g"(regs->V), "=g"(regs->C)
				 : "g"(b));
	regs->X = regs->C;
	regs->Z = c == 0;
	regs->N = c < 0;
	return c;
}



inline void btst_d_N(REGS* regs, int N, uint32_t v) {
	int d = regs->D[N];
	regs->Z = !((v >> d) & 1);
}
inline uint32_t bchg_d_N(REGS* regs, int N, uint32_t v) {
	int d = regs->D[N];
	regs->Z = !((v >> d) & 1);
	return v ^ (1 << d);
}
inline uint32_t bclr_d_N(REGS* regs, int N, uint32_t v) {
	int d = regs->D[N];
	regs->Z = !((v >> d) & 1);
	return v &~ (1 << d);
}
inline uint32_t bset_d_N(REGS* regs, int N, uint32_t v) {
	int d = regs->D[N];
	regs->Z = !((v >> d) & 1);
	return v | (1 << d);
}


inline void btst_i_N(REGS* regs, int N, uint32_t v) {
	regs->Z = !((v >> N) & 1);
}
inline uint32_t bchg_i_N(REGS* regs, int N, uint32_t v) {
	regs->Z = !((v >> N) & 1);
	return v ^ (1 << N);
}
inline uint32_t bclr_i_N(REGS* regs, int N, uint32_t v) {
	regs->Z = !((v >> N) & 1);
	return v &~ (1 << N);
}
inline uint32_t bset_i_N(REGS* regs, int N, uint32_t v) {
	regs->Z = !((v >> N) & 1);
	return v | (1 << N);
}

inline uint8_t op_addb(REGS* regs, uint8_t a, uint8_t b) {
	int8_t c = a;
	asm volatile("addb %3, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 : "+g"(c), "=g"(regs->V), "=g"(regs->C)
				 : "g"(b));
	regs->X = regs->C;
	regs->Z = c == 0;
	regs->N = c < 0;
	return c;
}

inline uint16_t op_addw(REGS* regs, uint16_t a, uint16_t b) {
	int16_t c = a;
	asm volatile("addw %3, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 : "+g"(c), "=g"(regs->V), "=g"(regs->C)
				 : "g"(b));
	regs->X = regs->C;
	regs->Z = c == 0;
	regs->N = c < 0;
	return c;
}

inline uint32_t op_addl(REGS* regs, uint32_t a, uint32_t b) {
	int32_t c = a;
	asm volatile("addl %3, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 : "+g"(c), "=g"(regs->V), "=g"(regs->C)
				 : "g"(b));
	regs->X = regs->C;
	regs->Z = c == 0;
	regs->N = c < 0;
	return c;
}


#endif
