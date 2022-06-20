#ifndef EA_H__
#include "newcpu.h"
uint32_t EA_Addr(int type, int reg, int sz, bool w);
uint8_t EA_READ8(int type, int reg, bool override = false);
uint16_t EA_READ16(int type, int reg, bool override = false);
uint32_t EA_READ32(int type, int reg, bool override = false);
void EA_WRITE8(int type, int reg, uint8_t v);
void EA_WRITE16(int type, int reg, uint16_t v);
void EA_WRITE32(int type, int reg, uint32_t v);

template<class F> void EA_Update8(int type, int reg, const F& f) {
    uint8_t v1 = EA_READ8( type, reg, true );
	EA_WRITE8(type, reg, f( v1) );
}

template<class F> void EA_Update16(int type, int reg, const F& f) {
    uint16_t v1 = EA_READ16( type, reg, true );
	EA_WRITE16(type, reg, f( v1) );
}

template<class F> void EA_Update32(int type, int reg, const F& f) {
    uint32_t v1 = EA_READ32( type, reg, true );
	EA_WRITE32(type, reg, f( v1) );
}

inline void WRITE_D8(int reg, uint8_t v) {
    regs.d[reg] = (regs.d[reg] & 0xffffff00) | v;

}
inline void WRITE_D16(int reg, uint16_t v) {
    regs.d[reg] = (regs.d[reg] & 0xffff0000) | v;
}
inline void WRITE_A16(int reg, int16_t v) {
    regs.a[reg] = v;
}

#endif