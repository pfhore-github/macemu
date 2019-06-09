#ifndef NEWCPU_COMMON
#define NEWCPU_COMMON
#define FETCH16() (fetch_w(regs->PC))
#define FETCH32() (fetch_l(regs->PC))

void ILLEGAL(REGS* regs) __attribute__((noreturn));

uint32_t ea_ax(REGS* regs, uint16_t nw, uint32_t ax);
inline void raise_ex(REGS* regs, int n) {}
class EA_DN {
	REGS* regs;
	int n;
public:
	EA_DN(REGS* regs, int i) :regs(regs), n(i) {}
	uint8_t readb() { return regs->D[n]; }
	uint16_t readw() { return regs->D[n]; }
	uint32_t readl() { return regs->D[n]; }
	void writeb(uint8_t v) { regs->D[n] = v; }
	void writew(uint16_t v) { regs->D[n] = v; }
	void writel(uint32_t v) { regs->D[n] = v; }
	uint32_t addr() { ILLEGAL(regs); }
};

class EA_AN {
	REGS* regs;
	int n;
public:
	EA_AN(REGS* regs, int i) :n(i) {}
	uint8_t readb()  { ILLEGAL(regs); }
	int16_t readw()  { return (int16_t)regs->A[n]; }
	uint32_t readl()  { return regs->A[n]; }
	void writeb(uint8_t v)  { ILLEGAL(regs); }
	void writew(int16_t v)  { regs->A[n] = v; }
	void writel(uint32_t v)  { regs->D[n] = v; }
	uint32_t addr()  { ILLEGAL(regs); }
};

class EA_MEM {
	bool got = false;
	int tmp = 0;
protected:
	REGS* regs;
	int n;
public:
	EA_MEM(REGS* regs, int i) :regs(regs), n(i) {}
	uint8_t readb()  { return read_b(addr()); }
	uint16_t readw()  { return read_w(addr()); }
	uint32_t readl()  { return read_l(addr()); }
	void writeb(uint8_t v)  { write_b(addr(), v); }
	void writew(uint16_t v)  { write_w(addr(), v); }
	void writel(uint32_t v)  { write_l(addr(), v); }
	uint32_t addr() {
		if( got ) { return tmp; }
		else {
			got = true;
			return tmp = get_addr();
		}
	}
	virtual uint32_t get_addr() { return regs->A[n]; }
};

template<int N>
struct EA_INCR : EA_MEM {
	EA_INCR(REGS* regs, int i) :EA_MEM(regs, i) {}
	uint32_t get_addr() override {
		uint32_t v = regs->A[n];
		regs->A[n] += N;
		return v;
	}
};
template<int N>
struct EA_DECR : EA_MEM {
	EA_DECR(REGS* regs, int i) :EA_MEM(regs, i) {}
	uint32_t get_addr() override {
		return regs->A[n] -= N;
	}
};

struct EA_D16 : EA_MEM {
	EA_D16(REGS* regs, int i) :EA_MEM(regs, i) {}
	uint32_t get_addr() override {
		return regs->A[n] + FETCH16();
	}
};

struct EA_EX : EA_MEM {
	EA_EX(REGS* regs, int i) : EA_MEM(regs, i) {}
	uint32_t get_addr() override {
		return ea_ax(regs, FETCH16(), regs->A[n]);
	}
};


struct EA_EX2_RW : EA_MEM {
	EA_EX2_RW(REGS* regs, int i) : EA_MEM(regs, i) {}
	uint32_t get_addr() override {
		if( n == 0 ) {
			return (int16_t)FETCH16();
		} else if( n == 1 ) {
			return FETCH32();
		} else {
			ILLEGAL(regs);
		}
	}
};

struct EA_EX2_C : EA_MEM {
	EA_EX2_C(REGS* regs, int i) : EA_MEM(regs, i) {}
	uint32_t get_addr() override {
		uint32_t pc = regs->PC;
		switch(n) {
		case 0 : return (int16_t)FETCH16(); 
		case 1 : return FETCH32(); 
		case 2 : return pc+FETCH16(); 
		case 3 : return ea_ax(regs, FETCH16(), pc);
		}
		ILLEGAL(regs);
	}
};

class EA_EX2_R_B {
	bool got = false;
	uint8_t v = 0;
protected:
	REGS* regs;
	int n;
public:
	EA_EX2_R_B(REGS* regs, int i) :regs(regs), n(i) {}
	uint8_t readb() {
		if( got )
			return v;
		uint32_t pc = regs->PC;
		got = true;
		switch(n) {
		case 0 : return v = read_b((int16_t)FETCH16()); 
		case 1 : return v = read_b(FETCH32()); 
		case 2 : return v = read_b(pc+FETCH16()); 
		case 3 : return v = read_b(ea_ax(regs, FETCH16(), pc));
		case 4 : return v = FETCH16();			
		}
		ILLEGAL(regs);
	}
};

class EA_EX2_R_W {
	bool got = false;
	uint16_t v = 0;
protected:
	REGS* regs;
	int n;
public:
	EA_EX2_R_W(REGS* regs, int i) :regs(regs), n(i) {}
	uint16_t readw() {
		if( got )
			return v;
		uint32_t pc = regs->PC;
		got = true;
		switch(n) {
		case 0 : return v = read_w((int16_t)FETCH16()); 
		case 1 : return v = read_w(FETCH32()); 
		case 2 : return v = read_w(pc+FETCH16()); 
		case 3 : return v = read_w(ea_ax(regs, FETCH16(), pc));
		case 4 : return v = FETCH16();			
		}
		ILLEGAL(regs);
	}
};

class EA_EX2_R_L {
	bool got = false;
	uint32_t v = 0;
protected:
	REGS* regs;
	int n;
public:
	EA_EX2_R_L(REGS* regs, int i) :regs(regs), n(i) {}
	uint32_t readl() {
		if( got )
			return v;
		uint32_t pc = regs->PC;
		got = true;
		switch(n) {
		case 0 : return v = read_l((int16_t)FETCH16()); 
		case 1 : return v = read_l(FETCH32()); 
		case 2 : return v = read_l(pc+FETCH16()); 
		case 3 : return v = read_l(ea_ax(regs, FETCH16(), pc));
		case 4 : return v = FETCH32();			
		}
		ILLEGAL(regs);
	}
};

#endif
