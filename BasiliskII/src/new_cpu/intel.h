#ifndef NEWCPU_INTEL
#define NEWCPU_INTEL
#include <climits>
#if defined(__x86_64__) || defined(__i386__)
inline void TEST_B(uint8_t a) {
	asm volatile("testb %2, %2\n\t"
				 "sets %0\n\t"
				 "setz %1\n\t"
				 : "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(a)
				 : "cc");
}
inline void TEST_W( uint16_t a) {
	asm volatile("testw %2, %2\n\t"
				 "sets %0\n\t"
				 "setz %1\n\t"
				 : "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(a)
				 : "cc");
}
inline void TEST_L( uint32_t a) {
	asm volatile("testl %2, %2\n\t"
				 "sets %0\n\t"
				 "setz %1\n\t"
				 : "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(a)
				 : "cc");
}
inline uint8_t OR_B( uint8_t a, uint8_t b) {
	asm volatile("orb %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.V = cpu.C = false;
	return a;
}
inline uint16_t OR_W( uint16_t a, uint16_t b) {
	asm volatile("orw %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.V = cpu.C = false;
	return a;
}
inline uint32_t OR_L( uint32_t a, uint32_t b) {
	asm volatile("orl %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.V = cpu.C = false;
	return a;
}
inline uint8_t AND_B( uint8_t a, uint8_t b) {
	asm volatile("andb %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.V = cpu.C = false;
	return a;
}
inline uint16_t AND_W( uint16_t a, uint16_t b) {
	asm volatile("andw %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.V = cpu.C = false;
	return a;
}
inline uint32_t AND_L( uint32_t a, uint32_t b) {
	asm volatile("andl %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.V = cpu.C = false;
	return a;
}
inline uint8_t XOR_B( uint8_t a, uint8_t b) {
	asm volatile("xorb %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.V = cpu.C = false;
	return a;
}
inline uint16_t XOR_W( uint16_t a, uint16_t b) {
	asm volatile("xorw %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.V = cpu.C = false;
	return a;
}
inline uint32_t XOR_L( uint32_t a, uint32_t b) {
	asm volatile("xorl %3, %0\n\t"
				 "sets %1\n\t"
				 "setz %2\n\t"
				 : "+r"(a), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.V = cpu.C = false;
	return a;
}
inline bool BTST(uint32_t v, int d) {
	bool c;
	asm volatile("bt %2, %1\n\t"
				 "setnc %0"
				 : "=g"(c)
				 : "r"(v), "r"(d)
				 : "cc" );
	return c;
}
inline uint32_t BCHG(uint32_t v, int d, bool* c) {
	asm volatile("btc %2, %1\n\t"
				 "setnc %0"
				 : "=g"(*c), "+r"(v)
				 : "r"(d)
				 : "cc" );
	return v;
}
inline uint32_t BCLR(uint32_t v, int d, bool* c) {
	asm volatile("btr %2, %1\n\t"
				 "setnc %0"
				 : "=g"(*c), "+r"(v)
				 : "r"(d)
				 : "cc" );
	return v;
}
inline uint32_t BSET(uint32_t v, int d, bool* c) {
	asm volatile("bts %2, %1\n\t"
				 "setnc %0"
				 : "=g"(*c), "+r"(v)
				 : "r"(d)
				 : "cc" );
	return v;
}
inline uint8_t SUB_B( uint8_t a, uint8_t b) {
	asm volatile("subb %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu.V), "=g"(cpu.C), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.X = cpu.C;
	return a;
}
inline uint16_t SUB_W( uint16_t a, uint16_t b) {
	asm volatile("subw %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu.V), "=g"(cpu.C), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.X = cpu.C;
	return a;
}
inline uint32_t SUB_L( uint32_t a, uint32_t b) {
	asm volatile("subl %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu.V), "=g"(cpu.C), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.X = cpu.C;
	return a;
}
inline void CMP_B( uint8_t a, uint8_t b) {
	asm volatile("cmpb %5, %4\n\t"
				 "seto %0\n\t"
				 "setc %1\n\t"
				 "sets %2\n\t"
				 "setz %3\n\t"
				 : "=g"(cpu.V), "=g"(cpu.C), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(a), "r"(b)
				 : "cc");
}
inline void CMP_W( uint16_t a, uint16_t b) {
	asm volatile("cmpw %5, %4\n\t"
				 "seto %0\n\t"
				 "setc %1\n\t"
				 "sets %2\n\t"
				 "setz %3\n\t"
				 : "=g"(cpu.V), "=g"(cpu.C), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(a), "r"(b)
				 : "cc");
}
inline void CMP_L( uint32_t a, uint32_t b) {
	asm volatile("cmpl %5, %4\n\t"
				 "seto %0\n\t"
				 "setc %1\n\t"
				 "sets %2\n\t"
				 "setz %3\n\t"
				 : "=g"(cpu.V), "=g"(cpu.C), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(a), "r"(b)
				 : "cc");
}
inline uint8_t ADD_B( uint8_t a, uint8_t b) {
	asm volatile("addb %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu.V), "=g"(cpu.C), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.X = cpu.C;
	return a;
}
inline uint16_t ADD_W( uint16_t a, uint16_t b) {
	asm volatile("addw %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu.V), "=g"(cpu.C), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.X = cpu.C;
	return a;
}
inline uint32_t ADD_L( uint32_t a, uint32_t b) {
	asm volatile("addl %5, %0\n\t"
				 "seto %1\n\t"
				 "setc %2\n\t"
				 "sets %3\n\t"
				 "setz %4\n\t"
				 : "+r"(a),
				   "=g"(cpu.V), "=g"(cpu.C), "=g"(cpu.N), "=g"(cpu.Z)
				 : "r"(b)
				 : "cc");
	cpu.X = cpu.C;
	return a;
}
inline int8_t ASR_B( int8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "sarb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
		cpu.X = cpu.C;
	} else {
		cpu.C = false;
	}
	TEST_B(v);
	cpu.V = false;
	return v;
}
inline int16_t ASR_W( int16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "sarw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu.C)
					 : "r"((uint8_t)sc)
					 : "cc", "%cl");
		cpu.X = cpu.C;
	} else {
		cpu.C = false;
	}
	TEST_W(v);
	cpu.V = false;
	return v;
}
inline int32_t ASR_L( int32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "sarl %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu.C)
					 : "r"((uint8_t)sc)
					 : "cc", "%cl");
		cpu.X = cpu.C;
	} else {
		cpu.C = false;
	}
	TEST_L(v);
	cpu.V = false;
	return v;
}
inline uint8_t LSR_B( uint8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "shrb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
		cpu.X = cpu.C;
	} else {
		cpu.C = false;
	}
	TEST_B(v);
	cpu.V = false;
	return v;
}
inline uint16_t LSR_W( uint16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "shrw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu.C)
					 : "r"((uint8_t)sc)
					 : "cc", "%cl");
		cpu.X = cpu.C;
	} else {
		cpu.C = false;
	}
	TEST_L(v);
	cpu.V = false;
	return v;
}
inline uint32_t LSR_L( uint32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "shrl %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu.C)
					 : "r"((uint8_t)sc)
					 : "cc", "%cl");
		cpu.X = cpu.C;
	} else {
		cpu.C = false;
	}
	TEST_L(v);
	cpu.V = false;
	return v;
}

inline uint8_t ROXR_B( uint8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %1, %%al\n\t"
					 "shrb %%al\n\t"
					 "movb %2, %%cl\n\t"
					 "rcrb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.X)
					 : "g"((uint8_t)sc)
					 : "cc", "%al", "%cl");
	}
	TEST_B(v);
	cpu.C = cpu.X;
	cpu.V = false;
	return v;
}

inline uint16_t ROXR_W( uint16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %1, %%al\n\t"
					 "shrb %%al\n\t"
					 "movb %2, %%cl\n\t"
					 "rcrw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.X)
					 : "g"((uint8_t)sc)
					 : "cc", "%al", "%cl");
	}
	TEST_W(v);
	cpu.C = cpu.X;
	cpu.V = false;
	return v;
}

inline uint32_t ROXR_L( uint32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %1, %%al\n\t"
					 "shrb %%al\n\t"
					 "movb %2, %%cl\n\t"
					 "rcrl %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.X)
					 : "g"((uint8_t)sc)
					 : "cc", "%al", "%cl");
	}
	TEST_L(v);
	cpu.C = cpu.X;
	cpu.V = false;
	return v;
}

inline uint8_t ROR_B( uint8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "rorb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
	} else {
		cpu.C = false;
	}
	TEST_B(v);
	cpu.V = false;
	return v;
}

inline uint16_t ROR_W( uint16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "rorw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
	} else {
		cpu.C = false;
	}
	TEST_W(v);
	cpu.V = false;
	return v;
}
inline uint32_t ROR_L( uint32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "rorl %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
	} else {
		cpu.C = false;
	}
	TEST_L(v);
	cpu.V = false;
	return v;
}



inline int8_t ASL_B( int8_t v, int sc) {
	if( sc == 0 ) {
		cpu.V = false;
		cpu.C = false;
		TEST_B(v);
		return v;
	}
	int nk = (1 << sc) - 1;
	int lw = sc == 8 ? v : v >> (7-sc) & nk;
	bool sg = (v >> 7) & 1;
	cpu.V = sg ? lw != nk : lw ; 
	asm volatile("movb %2, %%cl\n\t"
				 "salb %%cl, %0\n\t"
				 "setc %1\n\t"
				 : "+r"(v), "=g"(cpu.C)
				 : "r"((uint8_t)sc)
				 : "cc", "%cl");
	cpu.X = cpu.C;
	TEST_B( v);
	return v;
}
inline int16_t ASL_W( int16_t v, int sc) {
	if( sc == 0 ) {
		cpu.V = false;
		cpu.C = false;
		TEST_W(  v);
		return v;
	}
	int nk = (1 << sc) - 1;
	int lw = v >> (15-sc) & nk;
	bool sg = (v >> 15) & 1;
	cpu.V = sg ? lw != nk : lw ; 
	asm volatile("movb %2, %%cl\n\t"
				 "salw %%cl, %0\n\t"
				 "setc %1\n\t"
				 : "+r"(v), "=g"(cpu.C)
				 : "r"((uint8_t)sc)
				 : "cc", "%cl");
	TEST_W( v);
	cpu.X = cpu.C;
	return v;
}
inline int32_t ASL_L( int32_t v, int sc) {
	if( sc == 0 ) {
		cpu.V = false;
		cpu.C = false;
		TEST_L( v);
		return v;
	}
	int nk = (1 << sc) - 1;
	int lw = v >> (31-sc) & nk;
	bool sg = (v >> 31) & 1;
	cpu.V = sg ? lw != nk : lw ; 
	asm volatile("movb %2, %%cl\n\t"
				 "sall %%cl, %0\n\t"
				 "setc %1\n\t"
				 : "+r"(v), "=g"(cpu.C)
				 : "r"((uint8_t)sc)
				 : "cc", "%cl");
	TEST_L(  v);
	cpu.X = cpu.C;
	return v;
}
inline uint8_t LSL_B( uint8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "shlb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
		cpu.X = cpu.C;
	} else {
		cpu.C = false;
	}
	TEST_B(v);
	cpu.V = false;
	return v;
}
inline uint16_t LSL_W( uint16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "shlw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
		cpu.X = cpu.C;
	} else {
		cpu.C = false;
	}
	TEST_W(v);
	cpu.V = false;
	return v;
}
inline uint32_t LSL_L( uint32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "shll %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "=g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
		cpu.X = cpu.C;
	} else {
		cpu.C = false;
	}
	TEST_W(v);
	cpu.V = false;
	return v;
}

inline uint8_t ROXL_B( uint8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %1, %%al\n\t"
					 "shrb %%al\n\t"
					 "movb %2, %%cl\n\t"
					 "rclb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.X)
					 : "g"((uint8_t)sc)
					 : "cc", "%al", "%cl");
		cpu.C = cpu.X;
	} else {
		cpu.C = false;
	}
	TEST_B(v);
	cpu.V = false;
	return v;
}

inline uint16_t ROXL_W( uint16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %1, %%al\n\t"
					 "shrb %%al\n\t"
					 "movb %2, %%cl\n\t"
					 "rclw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.X)
					 : "g"((uint8_t)sc)
					 : "cc", "%al", "%cl");
	}
	TEST_W(v);
	cpu.C = cpu.X;
	cpu.V = false;
	return v;
}

inline uint32_t ROXL_L( uint32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %1, %%al\n\t"
					 "shrb %%al\n\t"
					 "movb %2, %%cl\n\t"
					 "rcll %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.X)
					 : "g"((uint8_t)sc)
					 : "cc", "%al", "%cl");
	}
	TEST_L(v);
	cpu.C = cpu.X;
	cpu.V = false;
	return v;
}

inline uint8_t ROL_B( uint8_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "rolb %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
	} else {
		cpu.C = false;
	}
	TEST_B(v);
	cpu.V = false;
	return v;
}

inline uint16_t ROL_W( uint16_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "rolw %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
	} else {
		cpu.C = false;
	}
	TEST_W(v);
	cpu.V = false;
	return v;
}
inline uint32_t ROL_L( uint32_t v, int sc) {
	if( sc ) {
		asm volatile("movb %2, %%cl\n\t"
					 "roll %%cl, %0\n\t"
					 "setc %1\n\t"
					 : "+r"(v), "+g"(cpu.C)
					 : "g"((uint8_t)sc)
					 : "cc", "%cl");
	} else {
		cpu.C = false;
	}
	TEST_L(v);
	cpu.V = false;
	return v;
}

#else
inline void TEST_B( uint8_t a) {
	cpu.N = (a >> 7) & 1;
	cpu.Z = !a;
}
inline void TEST_W( uint16_t a) {
	cpu.N = (a >> 15) & 1;
	cpu.Z = !a;
}
inline void TEST_L( uint32_t a) {
	cpu.N = (a >> 31) & 1;
	cpu.Z = !a;
}
inline uint8_t OR_B( uint8_t a, uint8_t b) {
	int8_t c = a | b;
	TEST_B(c);
	cpu.V = cpu.C = false;
	return c;
}
inline uint16_t OR_W( uint16_t a, uint16_t b) {
	int16_t c = a | b;
	TEST_W(c);
	cpu.V = cpu.C = false;
	return c;
}
inline uint32_t OR_L( uint32_t a, uint32_t b) {
	int32_t c = a | b;
	TEST_L(c);
	cpu.V = cpu.C = false;
	return c;
}
inline uint8_t AND_B( uint8_t a, uint8_t b) {
	int8_t c = a & b;
	TEST_B(c);
	cpu.V = cpu.C = false;
	return c;
}
inline uint16_t AND_W( uint16_t a, uint16_t b) {
	int16_t c = a & b;
	TEST_W(c);
	cpu.V = cpu.C = false;
	return c;
}
inline uint32_t AND_L( uint32_t a, uint32_t b) {
	int32_t c = a & b;
	TEST_L(c);
	cpu.V = cpu.C = false;
	return c;
}
inline uint8_t XOR_B( uint8_t a, uint8_t b) {
	int8_t c = a ^ b;
	TEST_B(c);
	cpu.V = cpu.C = false;
	return c;
}
inline uint16_t XOR_W( uint16_t a, uint16_t b) {
	int16_t c = a ^ b;
	TEST_W(c);
	cpu.V = cpu.C = false;
	return c;
}
inline uint32_t XOR_L( uint32_t a, uint32_t b) {
	int32_t c = a ^ b;
	TEST_L(c);
	cpu.V = cpu.C = false;
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
inline uint8_t SUB_B( uint8_t a, uint8_t b) {
	int8_t c;
	cpu.V = __builtin_sub_overflow((int8_t)a, (int8_t)b, &c);
	cpu.X = cpu.C = __builtin_sub_overflow_p((uint8_t)a, (uint8_t)b, (uint8_t)0);
	TEST_B(c);
	return c;
}
inline uint16_t SUB_W( uint16_t a, uint16_t b) {
	int16_t c;
	cpu.V = __builtin_sub_overflow((int16_t)a, (int16_t)b, &c);
	cpu.X = cpu.C = __builtin_sub_overflow_p((uint16_t)a, (uint16_t)b, (uint16_t)0);
	TEST_W(c);
	return c; 
}
inline uint32_t SUB_L( uint32_t a, uint32_t b) {
	int32_t c;
	cpu.V = __builtin_ssub_overflow(a, b, &c);
	cpu.X = cpu.C = __builtin_sub_overflow_p(a, b, (uint32_t)0);
	TEST_L(c);
	return c;
}
inline void CMP_B( uint8_t a, uint8_t b) {
	int8_t c;
	cpu.V = __builtin_sub_overflow((int8_t)a, (int8_t)b, &c);
	cpu.X = cpu.C = __builtin_sub_overflow_p((uint8_t)a, (uint8_t)b, (uint8_t)0);
	TEST_B(c);
}
inline void CMP_W( uint16_t a, uint16_t b) {
	int16_t c;
	cpu.V = __builtin_sub_overflow((int16_t)a, (int16_t)b, &c);
	cpu.X = cpu.C = __builtin_sub_overflow_p((uint16_t)a, (uint16_t)b, (uint16_t)0);
	TEST_W(c);
}
inline void CMP_L( uint32_t a, uint32_t b) {
	int32_t c;
	cpu.V = __builtin_ssub_overflow(a, b, &c);
	cpu.X = cpu.C = __builtin_sub_overflow_p(a, b, (uint32_t)0);
	TEST_L(c);
}
inline uint8_t ADD_B( uint8_t a, uint8_t b) {
	int8_t c;
	cpu.V = __builtin_add_overflow((int8_t)a, (int8_t)b, &c);
	cpu.X = cpu.C = __builtin_add_overflow_p((uint8_t)a, (uint8_t)b, (uint8_t)0);
	TEST_B(c);
	return c;
}
inline uint16_t ADD_W( uint16_t a, uint16_t b) {
	int16_t c;
	cpu.V = __builtin_add_overflow((int16_t)a, (int16_t)b, &c);
	cpu.X = cpu.C = __builtin_add_overflow_p((uint16_t)a, (uint16_t)b, (uint16_t)0);
	TEST_W(c);
	return c; 
}
inline uint32_t ADD_L( uint32_t a, uint32_t b) {
	int32_t c;
	cpu.V = __builtin_sadd_overflow(a, b, &c);
	cpu.X = cpu.C = __builtin_add_overflow_p(a, b, (uint32_t)0);
	TEST_L(c);
	return c;
}
inline int8_t ASR_B( int8_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = false;
	} else {
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v >>= sc;
	}
	TEST_B(v);
	return v;
}
inline int16_t ASR_W( int16_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = false;
	} else {
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v >>= sc;
	}
	TEST_W(v);
	return v;
}
inline int32_t ASR_L( int32_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = false;
	} else {
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v >>= sc;
	}
	TEST_L(v);
	return v;
}
inline uint8_t LSR_B( uint8_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = false;
	} else {
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v >>= sc;
	}
	TEST_B(v);
	return v;
}
inline uint16_t LSR_W( uint16_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = false;
	} else {		
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v >>= sc;
	}
	TEST_W(v);
	return v;
}
inline uint32_t LSR_L( uint32_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = false;
	} else {
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v >>= sc;
	}
	TEST_L(v);
	return v;
}

inline uint8_t ROXR_B( uint8_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		int c = cpu.X;
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v = (v >> sc) | c << (8-c) | ((v << (9-c)) & 0xff);
	}
	TEST_B(v);
	return v;	
}
inline uint16_t ROXR_W( uint16_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		int c = cpu.X;
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v = (v >> sc) | c << (16-c) | ((v << (17-c)) & 0xffff);
	}
	TEST_W(v);
	return v;	
}
inline uint32_t ROXR_L( uint32_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		int c = cpu.X;
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v = (v >> sc) | c << (32-c) | (v << (33-c));
	}
	TEST_L(v);
	return v;	
}

inline uint8_t ROR_B( uint8_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v = (v >> sc) | ((v << (8-c)) & 0xff);
	}
	TEST_B(v);
	return v;	
}
inline uint16_t ROR_W( uint16_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v = (v >> sc) | ((v << (16-c)) & 0xffff);
	}
	TEST_W(v);
	return v;	
}
inline uint32_t ROR_L( uint32_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		cpu.X = cpu.C = (v >> (sc-1)) & 1;
		v = (v >> sc) | ((v << (32-c));
	}
	TEST_L(v);
	return v;	
}

inline int8_t ASL_B( int8_t v, int sc) {
	if( sc == 0 ) {
		cpu.V = false;
		cpu.C = false;
		TEST_B(v);
		return v;
	}
	int nk = (1 << sc) - 1;
	int lw = v >> (8-sc) & nk;
	bool sg = (v >> 7) & 1;
	cpu.V = sg ? lw != nk : lw ;
	cpu.X = cpu.C = (v >> (8-sc)) & 1;
	v >>= sc;
	TEST_B(  v);
	return v;
}
inline int16_t ASL_W( int16_t v, int sc) {
	if( sc == 0 ) {
		cpu.V = false;
		cpu.C = false;
		TEST_W(  v);
		return v;
	}
	int nk = (1 << sc) - 1;
	int lw = v >> (16-sc) & nk;
	bool sg = (v >> 15) & 1;
	cpu.V = sg ? lw != nk : lw ; 
	cpu.X = cpu.C = (v >> (16-sc)) & 1;
	v >>= sc;
	TEST_W(  v);
	return v;
}
inline int16_t ASL_L( int16_t v, int sc) {
	if( sc == 0 ) {
		cpu.V = false;
		cpu.C = false;
		TEST_L(  v);
		return v;
	}
	int nk = (1 << sc) - 1;
	int lw = v >> (32-sc) & nk;
	bool sg = (v >> 31) & 1;
	cpu.V = sg ? lw != nk : lw ; 
	cpu.X = cpu.C = (v >> (32-sc)) & 1;
	v >>= sc;
	TEST_L(  v);
	return v;
}

inline uint8_t LSL_B( uint8_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = false;
	} else {
		cpu.X = cpu.C = (v >> (8-sc)) & 1;
		v <<= sc;
	} 
	TEST_B(v);
	return v;
}
inline uint16_t LSL_W( uint16_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = false;
	} else {
		cpu.X = cpu.C = (v >> (16-sc)) & 1;
		v <<= sc;
	} 
	TEST_W(v);
	return v;
}
inline uint32_t LSL_W( uint32_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = false;
	} else {
		cpu.X = cpu.C = (v >> (32-sc)) & 1;
		v <<= sc;
	} 
	TEST_W(v);
	return v;
}
inline uint8_t ROXL_B( uint8_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		int c = cpu.X;
		cpu.X = cpu.C = (v >> (8-sc)) & 1;
		v = (uint8_t)(v << (sc+1)) | c << sc | ((v >> (9-c));
	}
	TEST_B(v);
	return v;	
}
inline uint16_t ROXL_W( uint16_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		int c = cpu.X;
		cpu.X = cpu.C = (v >> (16-sc)) & 1;
		v = (uint8_t)(v << (sc+1)) | c << sc | ((v >> (17-c));
	}
	TEST_W(v);
	return v;	
}
inline uint32_t ROXL_L( uint32_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		int c = cpu.X;
		cpu.X = cpu.C = (v >> (32-sc)) & 1;
		v = (uint8_t)(v << (sc+1)) | c << sc | (v >> (33-c));
	}
	TEST_L(v);
	return v;	
}

inline uint8_t ROL_B( uint8_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		cpu.X = cpu.C = (v >> (8-sc)) & 1;
		v = (uint8_t)(v << sc) | (v >> (8-c));
	}
	TEST_B(v);
	return v;	
}
inline uint16_t ROL_W( uint16_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		cpu.X = cpu.C = (v >> (16-sc)) & 1;
		v = (uint16_t)(v << sc) | (v << (16-c));
	}
	TEST_W(v);
	return v;	
}
inline uint32_t ROL_L( uint32_t v, int sc) {
	cpu.V = false;
	if( sc == 0 ) {
		cpu.C = cpu.X;
	} else {
		cpu.X = cpu.C = (v >> (32-sc)) & 1;
		v = (v << sc) | ((v >> (32-c));
	}
	TEST_L(v);
	return v;	
}

#endif
inline void MUL_U( uint32_t a, uint32_t b, uint32_t *h, uint32_t *l) {
	uint64_t v = a;
	v *= b;
	cpu.N = v >> 63;
	cpu.Z = ! v;
	cpu.V = cpu.C = false;
	*h = v >> 32;
	*l = v & 0xffffffff;
}
inline void MUL_S( int32_t a, int32_t b, uint32_t *h, uint32_t *l) {
	int64_t v = a;
	v *= b;
	cpu.N = (v >> 63) & 1;
	cpu.Z = ! v;
	cpu.V = cpu.C = false;
	*h = v >> 32;
	*l = v & 0xffffffff;
}
inline void DIV_U( uint64_t a, uint32_t b, uint32_t *q, uint32_t *r) {
	uint64_t qq = a / b;
	if( (cpu.V = qq >> 32) )
		return;
	uint32_t q2 = qq;
	*q = q2;
	*r = a % b;
	cpu.N = q2 >> 31;
	cpu.Z = ! q2;
	cpu.C = false;
}
inline void DIV_S( int64_t a, int32_t b, uint32_t *q, uint32_t *r) {
	cpu.C = false;
	if( a == INT_MIN && b == -1 ) {
		cpu.V = true;
		return;
	}
	int64_t qq = a / b;
	int32_t q2 = qq;
	if( (cpu.V = qq != q2) ) {
		return;
	}
	*r = a % b;	  
	*q = q2;
	cpu.N = q2 >> 31;
	cpu.Z = ! q2;
	cpu.C = false;
}

inline uint32_t ROR(uint32_t x, int sc) {
	return sc == 32 || sc == 0 ? x : x >> sc | ( x << (32-sc)); 
}
inline uint32_t ROL(uint32_t x, int sc) {
	return sc == 32 || sc == 0 ? x : x << sc | ( x >> (32-sc)); 
}

#endif
