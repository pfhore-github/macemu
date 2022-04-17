/* -*- c++ -*- */
#ifndef MEMORY_H
#define MEMORY_H
#include <vector>
#include "newcpu.h"
#include "SDL.h"
extern std::vector<uint8_t> RAM;
enum class TT {
	NORMAL, MOVE16, LFC, AA
};
enum class TM {
	DATA_PUSH, USER_DATA, USER_CODE, MMU_DATA, MMU_CODE, SUPER_DATA, SUPER_CODE, RES
};
enum class SZ {
	LONG, BYTE, WORD, LINE
};

struct paddr {
	uint32_t addr;
	unsigned upa : 2;
	SZ sz : 2;
	TT tt : 2;
	TM tm : 3;
	bool rw : 1;
};
uint8_t readIO8(uint32_t addr);
uint16_t readIO16(uint32_t addr);
uint32_t readIO32(uint32_t addr);
void writeIO8(uint32_t addr, uint8_t v);
void writeIO16(uint32_t addr, uint16_t v) ;
void writeIO32(uint32_t addr, uint32_t v) ;
struct ssw_t {
	bool read = true;
	bool cp = false, cu = false, ct = false, cm = false, ma = false, atc = false;
	TT tt = TT::NORMAL;
	TM tm = TM::USER_DATA;
	SZ sz;
	uint16_t to_value() const {
		return cp << 15 |
			cu << 14 |
			ct << 13 |
			cm << 12 |
			ma << 11 |
			atc << 10 |
			read << 8 |
			int(sz) << 5 |
			int(tt) << 3 |
			int(tm);
	}
};
struct line { uint8_t value[16]; };
void* get_real_addr(uint32_t addr, ssw_t&& ssw);
template<class T>
void paccess(paddr v, T* p) {
	ssw_t ssw{ .read = !v.rw, .tt = v.tt, .tm = v.tm, .sz = v.sz };
	auto p2 = static_cast<T*>(get_real_addr(v.addr, std::move(ssw)));
	switch( v.sz) {
		case SZ::BYTE :
			if(! v.rw) {
				*p = p2 ? *p2 : readIO8(v.addr);
			} else {
				uint8_t val = *p;
				if( p2 ) {
					*p2 = val;
				} else {
					writeIO8(v.addr, val);
				}
			}
			break;
		case SZ::WORD :
			if(! v.rw) {
				*p = SDL_SwapBE16( p2 ? *p2 : readIO16(v.addr) );
			} else {
				uint16_t val = SDL_SwapBE16( *p );
				if( p2 ) {
					*p2 = val;
				} else {
					writeIO16(v.addr, val);
				}
			}
			break;
		case SZ::LONG :
			if(! v.rw) {
				*p = SDL_SwapBE32( p2 ? *p2 : readIO32(v.addr) );
			} else {
				uint32_t val = SDL_SwapBE32( *p );
				if( p2 ) {
					*p2 = val;
				} else {
					writeIO32(v.addr, val);
				}
			}
			break;
		case SZ::LINE :
			if(! v.rw) {
				if( p2 ) {
					memcpy(p, p2, 16);
				} else {
					for(int i = 0; i < 16; ++i ) {
						p[i] = readIO8(v.addr + i);
					}
				}
			} else {
				if( p2 ) {
					memcpy(p2, p, 16);
				} else {
					for(int i = 0; i < 16; ++i ) {
						writeIO8(v.addr + i, p[i]);
					}
				}
			}
			break;

	}
}
uint8_t read8(uint32_t addr);
uint16_t read16(uint32_t addr);
uint32_t read32(uint32_t addr);
void write8(uint32_t addr, uint8_t v);
void write16(uint32_t addr, uint16_t v);
void write32(uint32_t addr, uint32_t v);


extern ptrdiff_t MEMBaseDiff;


 uint16_t FETCH();
uint32_t FETCH32();
inline void PUSH16(uint16_t v) {
	write16( regs.a[7] -= 2 , v );
}
inline void PUSH32(uint32_t v) {
	write32( regs.a[7] -= 4 , v );
}

#endif /* MEMORY_H */

