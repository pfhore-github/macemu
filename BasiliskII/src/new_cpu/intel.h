#ifndef NEWCPU_INTEL
#define NEWCPU_INTEL
#include <climits>
#if defined(__x86_64__) || defined(__i386__)
inline void TEST_B(CPU* cpu, uint8_t a) {
	asm volatile("testb %2, %2\n\t"
				 "sets %0\n\t"
				 "setz %1\n\t"
				 : "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(a)
				 : "cc");
}
inline void TEST_W(CPU* cpu, uint16_t a) {
	asm volatile("testw %2, %2\n\t"
				 "sets %0\n\t"
				 "setz %1\n\t"
				 : "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(a)
				 : "cc");
}
inline void TEST_L(CPU* cpu, uint32_t a) {
	asm volatile("testl %2, %2\n\t"
				 "sets %0\n\t"
				 "setz %1\n\t"
				 : "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(a)
				 : "cc");
}
inline uint8_t OR_B(CPU* cpu, uint8_t a, uint8_t b) {
	asm volatile("orb %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->V = cpu->C = false;
	return a;
}
inline uint16_t OR_W(CPU* cpu, uint16_t a, uint16_t b) {
	asm volatile("orw %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->V = cpu->C = false;
	return a;
}
inline uint32_t OR_L(CPU* cpu, uint32_t a, uint32_t b) {
	asm volatile("orl %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->V = cpu->C = false;
	return a;
}
inline uint8_t AND_B(CPU* cpu, uint8_t a, uint8_t b) {
	asm volatile("andb %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->V = cpu->C = false;
	return a;
}
inline uint16_t AND_W(CPU* cpu, uint16_t a, uint16_t b) {
	asm volatile("andw %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->V = cpu->C = false;
	return a;
}
inline uint32_t AND_L(CPU* cpu, uint32_t a, uint32_t b) {
	asm volatile("andl %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->V = cpu->C = false;
	return a;
}
inline uint8_t XOR_B(CPU* cpu, uint8_t a, uint8_t b) {
	asm volatile("xorb %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->V = cpu->C = false;
	return a;
}
inline uint16_t XOR_W(CPU* cpu, uint16_t a, uint16_t b) {
	asm volatile("xorw %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->V = cpu->C = false;
	return a;
}
inline uint32_t XOR_L(CPU* cpu, uint32_t a, uint32_t b) {
	asm volatile("xorl %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->V = cpu->C = false;
	return a;
}
inline bool BTST(uint32_t v, int d) {
	bool c;
	asm volatile("bt %1, %2\n\t"
				 "setnc %0"
				 : "=g"(c)
				 : "r"(v), "r"(d)
				 : "cc" );
	return c;
}
inline uint32_t BCHG(uint32_t v, int d, bool* c) {
	asm volatile("btc %1, %2\n\t"
				 "setnc %0"
				 : "=g"(*c), "+g"(v)
				 : "r"(d)
				 : "cc" );
	return v;
}
inline uint32_t BCLR(uint32_t v, int d, bool* c) {
	asm volatile("btr %1, %2\n\t"
				 "setnc %0"
				 : "=g"(*c), "+g"(v)
				 : "r"(d)
				 : "cc" );
	return v;
}
inline uint32_t BSET(uint32_t v, int d, bool* c) {
	asm volatile("bts %1, %2\n\t"
				 "setnc %0"
				 : "=g"(*c), "+g"(v)
				 : "r"(d)
				 : "cc" );
	return v;
}
inline uint8_t SUB_B(CPU* cpu, uint8_t a, uint8_t b) {
	asm volatile("subb %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu->V), "=g"(cpu->C), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->X = cpu->C;
	return a;
}
inline uint16_t SUB_W(CPU* cpu, uint16_t a, uint16_t b) {
	asm volatile("subw %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu->V), "=g"(cpu->C), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->X = cpu->C;
	return a;
}
inline uint32_t SUB_L(CPU* cpu, uint32_t a, uint32_t b) {
	asm volatile("subl %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu->V), "=g"(cpu->C), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->X = cpu->C;
	return a;
}
inline void CMP_B(CPU* cpu, uint8_t a, uint8_t b) {
	asm volatile("cmpb %5, %4\n\t"
				 "seto %0\n\t"
				 "setc %1\n\t"
				 "sets %2\n\t"
				 "setz %3\n\t"
				 : "=g"(cpu->V), "=g"(cpu->C), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(a), "r"(b)
				 : "cc");
}
inline void CMP_W(CPU* cpu, uint16_t a, uint16_t b) {
	asm volatile("cmpw %5, %4\n\t"
				 "seto %0\n\t"
				 "setc %1\n\t"
				 "sets %2\n\t"
				 "setz %3\n\t"
				 : "=g"(cpu->V), "=g"(cpu->C), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(a), "r"(b)
				 : "cc");
}
inline void CMP_L(CPU* cpu, uint32_t a, uint32_t b) {
	asm volatile("cmpl %5, %4\n\t"
				 "seto %0\n\t"
				 "setc %1\n\t"
				 "sets %2\n\t"
				 "setz %3\n\t"
				 : "=g"(cpu->V), "=g"(cpu->C), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(a), "r"(b)
				 : "cc");
}
inline uint8_t ADD_B(CPU* cpu, uint8_t a, uint8_t b) {
	asm volatile("addb %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu->V), "=g"(cpu->C), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->X = cpu->C;
	return a;
}
inline uint16_t ADD_W(CPU* cpu, uint16_t a, uint16_t b) {
	asm volatile("addw %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu->V), "=g"(cpu->C), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->X = cpu->C;
	return a;
}
inline uint32_t ADD_L(CPU* cpu, uint32_t a, uint32_t b) {
	asm volatile("addl %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu->V), "=g"(cpu->C), "=g"(cpu->N), "=g"(cpu->Z)
				 : "r"(b)
				 : "cc");
	cpu->X = cpu->C;
	return a;
}
inline int8_t ASR_B(CPU* cpu, int8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "sarb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu->C), 
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
		cpu->X = cpu->C;
	} else {
		cpu->C = false;
	}
	TEST_B(cpu, v);
	cpu->V = false;
	return v;
}
inline int16_t ASR_W(CPU* cpu, int16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "sarw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu->C),
					 : "r"((uint8_t)sc)
					 : "cc", "%%cl");
		cpu->X = cpu->C;
	} else {
		cpu->C = false;
	}
	TEST_W(cpu, v);
	cpu->V = false;
	return v;
}
inline int32_t ASR_L(CPU* cpu, int32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "sarl %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu->C)
					 : "r"((uint8_t)sc)
					 : "cc", "%%cl");
		cpu->X = cpu->C;
	} else {
		cpu->C = false;
	}
	TEST_L(cpu, v);
	cpu->V = false;
	return v;
}
inline uint8_t LSR_B(CPU* cpu, uint8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "shrb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu->C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
		cpu->X = cpu->C;
	} else {
		cpu->C = false;
	}
	TEST_B(cpu, v);
	cpu->V = false;
	return v;
}
inline uint16_t LSR_W(CPU* cpu, uint16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "shrl %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu->C)
					 : "r"((uint8_t)sc)
					 : "cc", "%%cl");
		cpu->X = cpu->C;
	} else {
		cpu->C = false;
	}
	TEST_L(cpu, 0);
	cpu->V = false;
	return v;
}

inline uint8_t ROXR_B(CPU* cpu, uint8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %1, %%al\n\t"
					 "shrb %%al\n\t"
					 "movb %2, %%cl\n\t"
					 "rcrb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu->X)
					 : "g"((uint8_t)sc)
					 : "cc", "%al", "%cl");
		cpu->C = cpu->X;
	} else {
		cpu->C = false;
	}
	TEST_B(L, v);
	cpu->V = false;
	return v;
}

inline uint16_t ROXR_W(CPU* cpu, uint16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %1, %%al\n\t"
					 "shrb %%al\n\t"
					 "movb %2, %%cl\n\t"
					 "rcrw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu->X)
					 : "g"((uint8_t)sc)
					 : "cc", "%al", "%cl");
	}
	TEST_W(L, v);
	cpu->C = cpu->X;
	cpu->V = false;
	return v;
}

inline uint32_t ROXR_L(CPU* cpu, uint32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %1, %%al\n\t"
					 "shrb %%al\n\t"
					 "movb %2, %%cl\n\t"
					 "rcrl %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu->X)
					 : "g"((uint8_t)sc)
					 : "cc", "%al", "%cl");
	}
	TEST_W(L, v);
	cpu->C = cpu->X;
	cpu->V = false;
	return v;
}

inline uint8_t ROR_B(CPU* cpu, uint8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "rorb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu->C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
	} else {
		cpu->C = false;
	}
	TEST_B(L, v);
	cpu->V = false;
	return v;
}

inline uint16_t ROR_W(CPU* cpu, uint16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "rorw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu->C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
	} else {
		cpu->C = false;
	}
	TEST_W(L, v);
	cpu->V = false;
	return v;
}

inline uint32_t ROR_L(CPU* cpu, uint32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "rorl %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu->C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
	} else {
		cpu->C = false;
	}
	TEST_W(L, v);
	cpu->V = false;
	return v;
}


inline int8_t ASL_B(CPU* cpu, int8_t v, int sc) {
	if( sc == 0 ) {
		cpu->V = false;
		cpu->C = false;
		TEST_B(cpu, v);
		return v;
	}
	bool ov = false;
	for(int i = 0; i < sc; ++i ) {
		bool ovt;
		asm volatile("sarb %0\n\t"
					 "seto %1\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=r"(ovt), "=g"(cpu->C)
					 : "r"(sc)
					 : "cc");
		ov = ov || ovt;
	}
	cpu->X = cpu->C;
	TEST_B( cpu, v);
	return v;
}
inline int16_t ASL_W(CPU* cpu, int16_t v, int sc) {
	if( sc == 0 ) {
		cpu->V = false;
		cpu->C = false;
		cpu->Z = ! v;
		cpu->N = (v >> 15) & 1;
		return v;
	}
	bool ov = false;
	for(int i = 0; i < sc; ++i ) {
		bool ovt;
		asm volatile("sarw %0\n\t"
					 "seto %1\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=r"(ovt), "=g"(cpu->C)
					 : "r"(sc)
					 : "cc");
		ov = ov || ovt;
	}
	cpu->N = v >> 15;
	cpu->N = ! v;
	cpu->X = cpu->C;
	return v;
}
inline int16_t ASL_L(CPU* cpu, int16_t v, int sc) {
	if( sc == 0 ) {
		cpu->V = false;
		cpu->C = false;
		cpu->Z = ! v;
		cpu->N = (v >> 31) & 1;
		return v;
	}
	bool ov = false;
	for(int i = 0; i < sc; ++i ) {
		bool ovt;
		asm volatile("sarw %0\n\t"
					 "seto %1\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=r"(ovt), "=g"(cpu->C)
					 : "r"(sc)
					 : "cc");
		ov = ov || ovt;
	}
	cpu->N = v >> 31;
	cpu->N = ! v;
	cpu->X = cpu->C;
	return v;
}
#else
inline void TEST_B(CPU* cpu, uint8_t a) {
	cpu->N = (a >> 7) & 1;
	cpu->Z = !a;
}
inline void TEST_W(CPU* cpu, uint16_t a) {
	cpu->N = (a >> 15) & 1;
	cpu->Z = !a;
}
inline void TEST_L(CPU* cpu, uint32_t a) {
	cpu->N = (a >> 31) & 1;
	cpu->Z = !a;
}
inline uint8_t OR_B(CPU* cpu, uint8_t a, uint8_t b) {
	int8_t c = a | b;
	TEST_B(cpu, c);
	cpu->V = cpu->C = false;
	return c;
}
inline uint16_t OR_W(CPU* cpu, uint16_t a, uint16_t b) {
	int16_t c = a | b;
	TEST_W(cpu, c);
	cpu->V = cpu->C = false;
	return c;
}
inline uint32_t OR_L(CPU* cpu, uint32_t a, uint32_t b) {
	int32_t c = a | b;
	TEST_L(cpu, c);
	cpu->V = cpu->C = false;
	return c;
}
inline uint8_t AND_B(CPU* cpu, uint8_t a, uint8_t b) {
	int8_t c = a & b;
	TEST_B(cpu, c);
	cpu->V = cpu->C = false;
	return c;
}
inline uint16_t AND_W(CPU* cpu, uint16_t a, uint16_t b) {
	int16_t c = a & b;
	TEST_W(cpu, c);
	cpu->V = cpu->C = false;
	return c;
}
inline uint32_t AND_L(CPU* cpu, uint32_t a, uint32_t b) {
	int32_t c = a & b;
	TEST_L(cpu, c);
	cpu->V = cpu->C = false;
	return c;
}
inline uint8_t XOR_B(CPU* cpu, uint8_t a, uint8_t b) {
	int8_t c = a ^ b;
	TEST_B(cpu, c);
	cpu->V = cpu->C = false;
	return c;
}
inline uint16_t XOR_W(CPU* cpu, uint16_t a, uint16_t b) {
	int16_t c = a ^ b;
	TEST_W(cpu, c);
	cpu->V = cpu->C = false;
	return c;
}
inline uint32_t XOR_L(CPU* cpu, uint32_t a, uint32_t b) {
	int32_t c = a ^ b;
	TEST_L(cpu, c);
	cpu->V = cpu->C = false;
	return c;
}
inline bool BTST(uint32_t v, int d) {
	return !((v >> d) & 1);
}
inline uint32_t BCHG(uint32_t v, int d, bool* c) {
	*c = BTST(v, d);
	return v ^ (1 << d);
}
inline uint32_t BCLR(uint32_t v, int d, bool* c) {
	*c = BTST(v, d);
	return v &~ (1 << d);
}
inline uint32_t BSET(uint32_t v, int d, bool* c) {
	*c = BTST(v, d);
	return v | (1 << d);
}
inline uint8_t SUB_B(CPU* cpu, uint8_t a, uint8_t b) {
	int8_t c;
	cpu->V = __builtin_sub_overflow((int8_t)a, (int8_t)b, &c);
	cpu->X = cpu->C = __builtin_sub_overflow_p((uint8_t)a, (uint8_t)b, (uint8_t)0);
	TEST_B(cpu, c);
	return c;
}
inline uint16_t SUB_W(CPU* cpu, uint16_t a, uint16_t b) {
	int16_t c;
	cpu->V = __builtin_sub_overflow((int16_t)a, (int16_t)b, &c);
	cpu->X = cpu->C = __builtin_sub_overflow_p((uint16_t)a, (uint16_t)b, (uint16_t)0);
	TEST_W(cpu, c);
	return c; 
}
inline uint32_t SUB_L(CPU* cpu, uint32_t a, uint32_t b) {
	int32_t c;
	cpu->V = __builtin_ssub_overflow(a, b, &c);
	cpu->X = cpu->C = __builtin_sub_overflow_p(a, b, (uint32_t)0);
	TEST_L(cpu, c);
	return c;
}
inline void CMP_B(CPU* cpu, uint8_t a, uint8_t b) {
	int8_t c;
	cpu->V = __builtin_sub_overflow((int8_t)a, (int8_t)b, &c);
	cpu->X = cpu->C = __builtin_sub_overflow_p((uint8_t)a, (uint8_t)b, (uint8_t)0);
	TEST_B(cpu, c);
}
inline void CMP_W(CPU* cpu, uint16_t a, uint16_t b) {
	int16_t c;
	cpu->V = __builtin_sub_overflow((int16_t)a, (int16_t)b, &c);
	cpu->X = cpu->C = __builtin_sub_overflow_p((uint16_t)a, (uint16_t)b, (uint16_t)0);
	TEST_W(cpu, c);
}
inline void CMP_L(CPU* cpu, uint32_t a, uint32_t b) {
	int32_t c;
	cpu->V = __builtin_ssub_overflow(a, b, &c);
	cpu->X = cpu->C = __builtin_sub_overflow_p(a, b, (uint32_t)0);
	TEST_L(cpu, c);
}
inline uint8_t ADD_B(CPU* cpu, uint8_t a, uint8_t b) {
	int8_t c;
	cpu->V = __builtin_add_overflow((int8_t)a, (int8_t)b, &c);
	cpu->X = cpu->C = __builtin_add_overflow_p((uint8_t)a, (uint8_t)b, (uint8_t)0);
	TEST_B(cpu, c);
	return c;
}
inline uint16_t ADD_W(CPU* cpu, uint16_t a, uint16_t b) {
	int16_t c;
	cpu->V = __builtin_add_overflow((int16_t)a, (int16_t)b, &c);
	cpu->X = cpu->C = __builtin_add_overflow_p((uint16_t)a, (uint16_t)b, (uint16_t)0);
	TEST_W(cpu, c);
	return c; 
}
inline uint32_t ADD_L(CPU* cpu, uint32_t a, uint32_t b) {
	int32_t c;
	cpu->V = __builtin_sadd_overflow(a, b, &c);
	cpu->X = cpu->C = __builtin_add_overflow_p(a, b, (uint32_t)0);
	TEST_L(cpu, c);
	return c;
}
inline int8_t ASR_B(CPU* cpu, int8_t v, int sc) {
	cpu->V = false;
	if( sc == 0 ) {
		cpu->C = false;
	} else {
		cpu->X = cpu->C = (v >> (sc-1)) & 1;
		v >>= sc;
	}
	TEST_B(cpu, v);
	return v;
}
inline int16_t ASR_W(CPU* cpu, int16_t v, int sc) {
	cpu->V = false;
	if( sc == 0 ) {
		cpu->C = false;
	} else {
		cpu->X = cpu->C = (v >> (sc-1)) & 1;
		v >>= sc;
	}
	TEST_W(cpu, v);
	return v;
}
inline int32_t ASR_L(CPU* cpu, int32_t v, int sc) {
	cpu->V = false;
	if( sc == 0 ) {
		cpu->C = false;
	} else {
		cpu->X = cpu->C = (v >> (sc-1)) & 1;
		v >>= sc;
	}
	TEST_L(cpu, v);
	return v;
}
inline uint8_t LSR_B(CPU* cpu, uint8_t v, int sc) {
	cpu->V = false;
	if( sc == 0 ) {
		cpu->C = false;
		cpu->Z = ! v;
		cpu->N = (v >> 7) & 1;
		return v;
	}
	cpu->X = cpu->C = (v >> (sc-1)) & 1;
	v >>= sc;
	cpu->N = (v >> 7) & 1;
	cpu->Z = ! v;
	return v;
}
inline uint16_t LSR_W(CPU* cpu, uint16_t v, int sc) {
	cpu->V = false;
	if( sc == 0 ) {
		cpu->C = false;
		cpu->Z = ! v;
		cpu->N = (v >> 15) & 1;
		return v;
	} 
	cpu->X = cpu->C = (v >> (sc-1)) & 1;
	v >>= sc;
	cpu->N = (v >> 15) & 1;
	cpu->Z = ! v;
	return v;
}
inline uint32_t LSR_L(CPU* cpu, uint32_t v, int sc) {
	cpu->V = false;
	if( sc == 0 ) {
		cpu->C = false;
		cpu->Z = ! v;
		cpu->N = (v >> 31) & 1;
		return v;
	} 
	cpu->X = cpu->C = (v >> (sc-1)) & 1;
	v >>= sc;
	cpu->N = (v >> 31) & 1;
	cpu->Z = ! v;
	return v;
}
inline uint8_t ROXR_B(CPU* cpu, uint8_t v, int sc) {
	
}
#endif
inline void MUL_U(CPU* cpu, uint32_t a, uint32_t b, uint32_t *h, uint32_t *l) {
	uint64_t v = a;
	v *= b;
	cpu->N = v >> 63;
	cpu->Z = ! v;
	cpu->V = cpu->C = false;
	*h = v >> 32;
	*l = v & 0xffffffff;
}
inline void MUL_S(CPU* cpu, int32_t a, int32_t b, uint32_t *h, uint32_t *l) {
	int64_t v = a;
	v *= b;
	cpu->N = (v >> 63) & 1;
	cpu->Z = ! v;
	cpu->V = cpu->C = false;
	*h = v >> 32;
	*l = v & 0xffffffff;
}
inline void DIV_U(CPU* cpu, uint64_t a, uint32_t b, uint32_t *q, uint32_t *r) {
	uint64_t qq = a / b;
	*r = a % b;
	cpu->V = qq >> 32;
	uint32_t q2 = qq;
	*q = q2;
	cpu->N = q2 >> 31;
	cpu->Z = ! q2;
	cpu->C = false;
}
inline void DIV_S(CPU* cpu, int64_t a, int32_t b, uint32_t *q, uint32_t *r) {
	cpu->C = false;
	if( a == INT_MIN && b == -1 ) {
		cpu->V = true;
		return;
	}
	int64_t qq = a / b;
	*r = a % b;	  
	int32_t q2 = qq;
	cpu->V = qq != q2;
	*q = q2;
	cpu->N = q2 >> 31;
	cpu->Z = ! q2;
	cpu->C = false;
}


#endif
