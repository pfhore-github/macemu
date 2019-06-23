#ifndef MMU_HPP
#define MMU_HPP
#include "SDL_endian.h"
inline void WRITE8(void* p, uint8_t v) {
	*(uint8_t*)p = v;
}
inline void WRITE16(void* p, uint16_t v) {
	*(uint16_t*)p = SDL_SwapBE16(v);
}
inline void WRITE32(void* p, uint32_t v) {
	*(uint32_t*)p = SDL_SwapBE16(v);
}
inline uint8_t READ8(const void* p) {
	return *(uint8_t*)p;
}
inline uint16_t READ16(const void* p) {
	return SDL_SwapBE16(*(uint16_t*)p);
}
inline uint32_t READ32(const void* p) {
	return SDL_SwapBE32(*(uint32_t*)p);
}

class MMU {
	class CPU* cpu;
	int mode(int c, bool intr);
protected:
	MMU(CPU* cpu) :cpu(cpu) {}
public:
	virtual void movec_to(int v, uint32_t value) = 0;
	virtual uint32_t movec_from(int v) = 0;
	virtual void* to_real(uint32_t va, int sz, bool write, int mode = 1) = 0;
	void write_b(uint32_t va, uint8_t b) {
		WRITE8(to_real(va, 1, true, mode(1, false)), b);
	}
	void write_w(uint32_t va, uint16_t w, bool intr = false) {		
		WRITE16(to_real(va, 2, true, mode(1, intr)), w);
	}
	void write_l(uint32_t va, uint32_t lw, bool intr = false) {
		WRITE32(to_real(va, 4, true, mode(1, intr)), lw);
	}
	uint8_t read_b(uint32_t va) {
		return READ8(to_real(va, 1, false, mode(1, false)));
	}
	uint16_t read_w(uint32_t va, bool intr = false) {
		return READ16(to_real(va, 2, false, mode(1, intr)));
	}
	uint32_t read_l(uint32_t va, bool intr = false) {
		return READ32(to_real(va, 4, false, mode(1, intr)));
	}
};
class NonMMU : MMU {
public:
	NonMMU(CPU* cpu) :MMU(cpu) {}
	void* to_real(uint32_t va, int sz, bool write, int mode = 1) override;
	void movec_to(int v, uint32_t value) override {}
	uint32_t movec_from(int v) override { return 0; }
};
#endif
