#include "cpu_emulation.h"
#include "main.h"
#include "sysdeps.h"
#include <stdlib.h>
#include <limits.h>

#include "ea.h"
#include "exception.h"
#include "intop.h"
#include "newcpu.h"

int from_bcd(uint8_t v) { return ((v >> 4) & 0xf) * 10 + (v & 0xf); }
uint8_t to_bcd(int v) { return ((v / 10) % 10) << 4 | (v % 10); }



OP(nbcd) {
    EA_Update8(type, reg, [](auto v1) {
        int v = 100 - from_bcd(v1) - regs.x;
        bool cc = (v1 % 100) != 0;
        regs.x = regs.c = cc;
        if(cc) {
            regs.z = false;
        }
        return to_bcd(v);
    });
}


OP(sbcd_d) {
	uint8_t src = regs.d[reg];
	uint8_t dst = regs.d[dm];
	int v = from_bcd(dst) - from_bcd(src) - regs.x;
	if( v < 0) {
		regs.c = true;
		v += 100;
	} else {
		regs.c = false;
	}
	regs.x = regs.c;
	regs.z = v == 0;
	WRITE_D8(dm, to_bcd(v));
}

OP(sbcd_a) {	
	uint8_t src = read8( --regs.a[reg] );
	uint8_t dst = read8( --regs.a[dm] );
	int v = from_bcd(dst) - from_bcd(src) - regs.x;
	if( v < 0) {
		regs.c = true;
		v += 100;
	} else {
		regs.c = false;
	}
	regs.x = regs.c;
	regs.z = v == 0;
	write8( regs.a[ dm], to_bcd(v));
}

OP(pack_d) {
	uint16_t src = regs.d[reg];
	uint16_t adjust = FETCH();
	uint16_t dst = src + adjust;
    WRITE_D16(dm, (dst & 0xf) | (dst >> 4 & 0xf0));
}

OP(pack_a) {	
	uint16_t adjust = FETCH();
	uint8_t src1 = read8( --regs.a[reg] );
	uint8_t src2 = read8( --regs.a[reg] );
    uint16_t dst = ( src1 | src2 << 8 ) + adjust;
    write8( --regs.a[ dm], (dst & 0xf) | (dst >> 4 & 0xf0));
}

OP(unpk_d) {
	uint16_t src = (regs.d[reg] & 0xf0) << 4 | ( regs.d[reg] & 0x0f);
	uint16_t adjust = FETCH();
    WRITE_D16( dm, src + adjust);
}

OP(unpk_a) {	
	uint16_t adjust = FETCH();
	uint8_t src = read8( --regs.a[reg] );
    uint16_t dst = ((src & 0xf0) << 4 | ( src & 0x0f )) + adjust;
    write16( regs.a[dm] -= 2, dst);
}

OP(abcd_d) {
	uint8_t src = regs.d[reg];
	uint8_t dst = regs.d[dm];
	int v = from_bcd(dst) + from_bcd(src) + regs.x;
	if( v > 99) {
		regs.c = true;
		v -= 100;
	} else {
		regs.c = false;
	}
	regs.x = regs.c;
	regs.z = v == 0;
	WRITE_D8(dm, to_bcd(v));
}

OP(abcd_a) {	
	uint8_t src = read8( --regs.a[reg] );
	uint8_t dst = read8( --regs.a[dm] );
	int v = from_bcd(dst) + from_bcd(src) + regs.x;
	if( v > 99) {
		regs.c = true;
		v -= 100;
	} else {
		regs.c = false;
	}
	regs.x = regs.c;
	regs.z = v == 0;
	write8( regs.a[ dm], to_bcd(v));
}
