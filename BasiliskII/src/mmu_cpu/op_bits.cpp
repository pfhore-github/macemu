#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include "sysdeps.h"
#include "cpu_emulation.h"
#include "main.h"

#include "newcpu.h"
#include "exception.h"
#include "ea.h"
#include "intop.h"

OP(ori_b) {
	uint8_t v2 = FETCH();
	EA_Update8(type, reg, [v2](auto v1) { return DO_OR_B( v1, v2); });
}

OP(ori_w) {
	uint16_t v2 = FETCH();
	EA_Update16(type, reg, [v2](auto v1) { return DO_OR_W( v1, v2); });
}

OP(ori_l) {
	uint32_t v2 = FETCH32();
	EA_Update32(type, reg, [v2](auto v1) { return DO_OR_L( v1, v2); });
}

OP(ori_b_ccr) {
	SET_CCR( GET_CCR() | (FETCH() & 0xff) );
}

OP(ori_w_sr) {
	uint16_t v = FETCH();
	if( ! regs.S) {
		PRIV_ERROR();
		return;
	}
	SET_SR( GET_SR() | v );
}

OP(andi_b) {
	uint8_t v2 = FETCH();
	EA_Update8(type, reg, [v2](auto v1) { return DO_AND_B( v1, v2); });
}

OP(andi_w) {
	uint16_t v2 = FETCH();
	EA_Update16(type, reg, [v2](auto v1) { return DO_AND_W( v1, v2); });
}

OP(andi_l) {
	uint32_t v2 = FETCH32();
	EA_Update32(type, reg, [v2](auto v1) { return DO_AND_L( v1, v2); });
}


OP(andi_b_ccr) {
	SET_CCR( GET_CCR() & (FETCH() & 0xff) );
}

OP(andi_w_sr) {
	uint16_t v = FETCH();
	if( ! regs.S) {
		PRIV_ERROR();
		return;
	}
	SET_SR( GET_SR() & v );
}



OP( btst_l_imm ) {
	int bn = FETCH() & 31;
	uint32_t v1 = regs.d[reg];	
	regs.z = ! (( v1 >> bn) & 1);
}



OP( bchg_l_imm ) {
	int bn = FETCH() & 31;
	uint32_t v1 = regs.d[reg];	
	regs.z = ! (( v1 >> bn) & 1);
	regs.d[reg] ^= 1 << bn;
}

OP( bclr_l_imm ) {
	int bn = FETCH() & 31;
	uint32_t v1 = regs.d[reg];	
	regs.z = ! (( v1 >> bn) & 1);
	regs.d[reg] &= ~(1 << bn);
}

OP( bset_l_imm ) {
	int bn = FETCH() & 31;
	uint32_t v1 = regs.d[reg];	
	regs.z = ! (( v1 >> bn) & 1);
	regs.d[reg] |= (1 << bn);
}

OP( btst_b_imm ) {
	int bn = FETCH() & 7;
	uint8_t v1 = EA_READ8(type, reg);
	regs.z = ! (( v1 >> bn) & 1);
}

OP( bchg_b_imm ) {
	int bn = FETCH() & 7;
	EA_Update8( type, reg, [bn](auto v1) {
		regs.z = ! (( v1 >> bn) & 1);
		return v1 ^ (1 << bn);
	});
}

OP( bclr_b_imm ) {
	int bn = FETCH() & 7;
	EA_Update8( type, reg, [bn](auto v1) {
		regs.z = ! (( v1 >> bn) & 1);
		return v1 &~ (1 << bn);
	});
}

OP( bset_b_imm ) {
	int bn = FETCH() & 7;
	EA_Update8( type, reg, [bn](auto v1) {
		regs.z = ! (( v1 >> bn) & 1);
		return v1 | (1 << bn);
	});
}

OP( eori_b ) {
	uint8_t v2 = FETCH();
	EA_Update8(type, reg, [v2](auto v1) { return DO_EOR_B( v1, v2); });
}

OP( eori_w ) {
	uint16_t v2 = FETCH();
	EA_Update16(type, reg, [v2](auto v1) { return DO_EOR_W( v1, v2); });
}

OP( eori_l ) {
	uint32_t v2 = FETCH32();
	EA_Update32(type, reg, [v2](auto v1) { return DO_EOR_L( v1, v2); });
}

OP( eori_b_ccr) {
	SET_CCR( GET_CCR() & (FETCH() & 0xff) );
}

OP( eori_w_sr ) {
	uint16_t v = FETCH();
	if( ! regs.S) {
		PRIV_ERROR();
		return;
	}
	SET_SR( GET_SR() & v );
}



OP( btst_l_dm ) {
	uint32_t v1 = regs.d[reg];	
	int bn = regs.d[ dm ] & 31;
	regs.z = ! (( v1 >> bn) & 1);
}

OP( bchg_l_dm ) {
	uint32_t v1 = regs.d[reg];	
	int bn = regs.d[ dm ] & 31;
	regs.z = ! (( v1 >> bn) & 1);
	regs.d[reg] ^= 1 << bn;
}

OP( bclr_l_dm ) {
	uint32_t v1 = regs.d[reg];	
	int bn = regs.d[ dm ] & 31;
	regs.z = ! (( v1 >> bn) & 1);
	regs.d[reg] &= ~(1 << bn);
}

OP( bset_l_dm ) {
	uint32_t v1 = regs.d[reg];	
	int bn = regs.d[ dm ] & 31;
	regs.z = ! (( v1 >> bn) & 1);
	regs.d[reg] |= (1 << bn);
}

OP( btst_b_dm ) {
	uint8_t v1 = EA_READ8(type, reg);
	int bn = regs.d[ dm ] & 7;
	regs.z = ! (( v1 >> bn) & 1);
}


OP( bchg_b_dm ) {
	int bn = regs.d[ dm ] & 7;
	EA_Update8( type, reg, [bn](auto v1) {
		regs.z = ! (( v1 >> bn) & 1);
		return v1 ^ (1 << bn);
	});
}

OP( bclr_b_dm ) {
	int bn = regs.d[ dm ] & 7;
	EA_Update8( type, reg, [bn](auto v1) {
		regs.z = ! (( v1 >> bn) & 1);
		return v1 &~ (1 << bn);
	});
}

OP( bset_b_dm ) {
	int bn = regs.d[ dm ] & 7;
	EA_Update8( type, reg, [bn](auto v1) {
		regs.z = ! (( v1 >> bn) & 1);
		return v1 | (1 << bn);
	});
}






OP( not_b ) {
	EA_Update8(type, reg, [](auto v1) { return DO_NOT_B(v1); });
}


OP( not_w ) {
	EA_Update16(type, reg, [](auto v1) { return DO_NOT_W( v1); });
}

OP( not_l ) {
	EA_Update32(type, reg, [](auto v1) { return DO_NOT_L( v1); });
}

OP( swap ) {
	uint32_t v = regs.d[ reg ];
	v = ( v >> 16 ) | ( v << 16);
	TEST_NZ32( v);
	regs.v = regs.c = false;
	regs.d[ reg ] = v;
}

OP( tst_b) {
	uint8_t v = EA_READ8( type, reg);
	TEST_NZ8(v);
	regs.v = false;
	regs.c = false;
}

OP( tst_w) {
	uint16_t v = EA_READ16( type, reg);
	TEST_NZ16(v);
	regs.v = false;
	regs.c = false;
}

OP( tst_l) {
	uint32_t v = EA_READ32( type, reg);
	TEST_NZ32(v);
	regs.v = false;
	regs.c = false;
}

OP( tas ) {
	EA_Update8(type, reg, [](uint8_t v) {
		TEST_NZ8(v);
		regs.v = false;
		regs.c = false;
		return v | 0x80;
	});
}

OP(or_b) {
	uint8_t v2 = EA_READ8( type, reg );
	WRITE_D8(dm, DO_OR_B( regs.d[dm], v2));
}

OP(or_w) {
	uint16_t v2 = EA_READ16( type, reg );
	WRITE_D16(dm, DO_OR_W( regs.d[dm], v2));
}

OP(or_l) {
	uint32_t v2 = EA_READ32( type, reg );
	regs.d[dm] = DO_OR_L( regs.d[dm], v2);
}

OP(or_to_ea_b) {
	uint8_t v2 = regs.d[ dm ];
	EA_Update8(type, reg, [v2](auto v1) { return DO_OR_B( v1, v2); });
}
OP(or_to_ea_w) {
	uint16_t v2 = regs.d[ dm ];
	EA_Update16(type, reg, [v2](auto v1) { return DO_OR_W( v1, v2); });
}
OP(or_to_ea_l) {
	uint32_t v2 = regs.d[ dm ];
	EA_Update32(type, reg, [v2](auto v1) { return DO_OR_L( v1, v2); });
}

OP(eor_b) {
	uint8_t v2 = regs.d[ dm ];
	EA_Update8(type, reg, [v2](auto v1) { return DO_EOR_B( v1, v2); });
}
OP(eor_w) {
	uint16_t v2 = regs.d[ dm ];
	EA_Update16(type, reg, [v2](auto v1) { return DO_EOR_W( v1, v2); });
}
OP(eor_l) {
	uint32_t v2 = regs.d[ dm ];
	EA_Update32(type, reg, [v2](auto v1) { return DO_EOR_L( v1, v2); });
}

OP(and_b) {
	uint8_t v2 = EA_READ8( type, reg );
	WRITE_D8(dm, DO_AND_B( regs.d[dm], v2));
}

OP(and_w) {
	uint16_t v2 = EA_READ16( type, reg );
	WRITE_D16(dm, DO_AND_W( regs.d[dm], v2));
}

OP(and_l) {
	uint32_t v2 = EA_READ32( type, reg );
	regs.d[dm] = DO_AND_L( regs.d[dm], v2);
}

OP(and_to_ea_b) {
	uint8_t v2 = regs.d[ dm ];
	EA_Update8(type, reg, [v2](auto v1) { return DO_AND_B( v1, v2); });
}
OP(and_to_ea_w) {
	uint16_t v2 = regs.d[ dm ];
	EA_Update16(type, reg, [v2](auto v1) { return DO_AND_W( v1, v2); });
}
OP(and_to_ea_l) {
	uint32_t v2 = regs.d[ dm ];
	EA_Update32(type, reg, [v2](auto v1) { return DO_AND_L( v1, v2); });
}