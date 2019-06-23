#ifndef NEWCPU_COMMON
#define NEWCPU_COMMON
#include "exceptions.hpp"
#define FETCH16() (cpu->fetch_w())
#define FETCH32() (cpu->fetch_l())

class EA {
	CPU* cpu;
	int mode;
	int n;
	uint32_t iaddr;
	bool w;
	int sz;
public:
	EA(CPU* cpu, int mode, int sz, int n, bool w)
		:cpu(cpu), mode(mode), n(n)
		, iaddr((uint32_t)-1), w(w), sz(sz) {}
	uint32_t read();
	void write(uint32_t v);
	uint32_t addr();
};

#endif
