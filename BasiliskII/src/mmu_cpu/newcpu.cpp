/*
 * UAE - The Un*x Amiga Emulator
 *
 * MC68000 emulation
 *
 * (c) 1995 Bernd Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <functional>
#include "sysdeps.h"
#include "cpu_emulation.h"
#include "main.h"
#include "emul_op.h"
#include "arith.h"
extern int intlev(void);	// From baisilisk_glue.cpp

#include "newcpu.h"
#include "exception.h"
#include "fpu/fpu.h"
#include "ea.h"

bool quit_program = false;

m68k_reg regs;
void SET_SR(uint16_t v) {
	if( regs.interrupt ) {
		regs.isp = regs.a[7];
	} else if( regs.S ) {
		regs.msp = regs.a[7];
	} else {
		regs.usp = regs.a[7];
	}
	regs.c = v & 1;
	regs.v = v >> 1 & 1;
	regs.z = v >> 2 & 1;
	regs.n = v >> 3 & 1;
	regs.x = v >> 4 & 1;
	regs.IM = v >> 8 & 3;
	regs.M = v >> 12 & 1;
	regs.S = v >> 13 & 1;
	regs.T = v >> 15 & 1;
	if( regs.interrupt ) {
		regs.a[7] = regs.isp;
	} else if( regs.S ) {
		regs.a[7] = regs.msp;
	} else {
		regs.a[7] = regs.usp;
	}
}

#define OP(name) void op_##name(uint16_t __attribute__((unused)) xop, int __attribute__((unused)) dm, int __attribute__((unused)) type, int __attribute__((unused)) reg)


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
	}
	SET_SR( GET_SR() & v );
}

OP(cmp2_chk2_b) {
	uint16_t op2 = FETCH();
	regs.i_ea = EA_Addr( type, reg, 0, false );
	uint8_t low = read8(regs.i_ea);
	uint8_t high = read8(regs.i_ea+1);
	if( op2 & 0x8000 ) {
		// An vs Rn
		int32_t lowa = (int8_t)low;
		int32_t higha = (int8_t)high;
		int32_t vx = regs.a[ op2 >> 12 & 7];
		regs.z = vx == lowa  || vx == higha;
		regs.c = vx < lowa || vx > higha;
	} else {
		uint8_t v = regs.d[ op2 >> 12 & 7];
		regs.z = v == low || v == high;
		regs.c = v < low || v > high;
	}
	if( (op2 & 0x800) && regs.c ) {
		CHK2_ERROR();
	}
	
}

OP(cmp2_chk2_w)  {
	uint16_t op2 = FETCH();
	regs.i_ea = EA_Addr( type, reg, 0, false );
	uint16_t low = read16(regs.i_ea);
	uint16_t high = read16(regs.i_ea+2);
	if( op2 & 0x8000 ) {
		// An vs Rn
		int32_t lowa = (int16_t)low;
		int32_t higha = (int16_t)high;
		int32_t vx = regs.a[ op2 >> 12 & 7];
		regs.z = vx == lowa  || vx == higha;
		regs.c = vx < lowa || vx > higha;
	} else {
		uint16_t v = regs.d[ op2 >> 12 & 7];
		regs.z = v == low || v == high;
		regs.c = v < low || v > high;
	}
	if( (op2 & 0x800) && regs.c ) {
		CHK2_ERROR();
	}	
}

OP( subi_b ) {
	uint8_t v2 = FETCH();
	EA_Update8(type, reg, [v2](auto v1) { return DO_SUB_B( v1, v2); });
}


OP( subi_w ) {
	uint16_t v2 = FETCH();
	EA_Update16(type, reg, [v2](auto v1) { return DO_SUB_W( v1, v2); });
}

OP( subi_l ) {
	uint32_t v2 = FETCH32();
	EA_Update32(type, reg, [v2](auto v1) { return DO_SUB_L( v1, v2); });
}

OP( cmp2_chk2_l ) {
	uint32_t op2 = FETCH();
	regs.i_ea = EA_Addr( type, reg, 0, false );
	uint32_t low = read32(regs.i_ea);
	uint32_t high = read32(regs.i_ea+4);
	if( op2 & 0x8000 ) {
		// An vs Rn
		int32_t lowa = low;
		int32_t higha = high;
		int32_t vx = regs.a[ op2 >> 12 & 7];
		regs.z = vx == lowa  || vx == higha;
		regs.c = vx < lowa || vx > higha;
	} else {
		uint32_t v = regs.d[ op2 >> 12 & 7];
		regs.z = v == low || v == high;
		regs.c = v < low || v > high;
	}
	if( (op2 & 0x800) && regs.c ) {
		CHK2_ERROR();
	}	
}

OP( addi_b ) {
	uint8_t v2 = FETCH();
	EA_Update8(type, reg, [v2](auto v1) { return DO_ADD_B( v1, v2); });
}

OP( addi_w ) {
	uint16_t v2 = FETCH();
	EA_Update16(type, reg, [v2](auto v1) { return DO_ADD_W( v1, v2); });
}

OP( addi_l ) {
	uint32_t v2 = FETCH32();
	EA_Update32(type, reg, [v2](auto v1) { return DO_ADD_L( v1, v2); });
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
	}
	SET_SR( GET_SR() & v );
}

OP( cas_b ) {
	uint16_t op2 = FETCH();
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t d = read8(regs.i_ea);
	DO_CMP_B( d, regs.d[ op2 & 7]);
	if( regs.z ) {
		write8( regs.i_ea, regs.d[ op2 >> 6 & 7]);
	} else {
		EA_WRITE8(0, op2&7, d);
	}
}

OP( cas_w ) {
	uint16_t op2 = FETCH();
	regs.i_ea = EA_Addr( type, reg, 2, true );
	uint16_t d = read16(regs.i_ea);
	DO_CMP_W( d, regs.d[ op2 & 7]);
	if( regs.z ) {
		write16( regs.i_ea, regs.d[ op2 >> 6 & 7]);
	} else {
		EA_WRITE16(0, op2&7, d);
	}
}

OP( cas_l ) {
	uint16_t op2 = FETCH();
	regs.i_ea = EA_Addr( type, reg, 4, true );
	uint32_t d = read32(regs.i_ea);
	DO_CMP_L( d, regs.d[ op2 & 7]);
	if( regs.z ) {
		write32( regs.i_ea, regs.d[ op2 >> 6 & 7]);
	} else {
		EA_WRITE32(0, op2&7, d);
	}
}


OP( cas2_w ) {
	uint16_t op2 = FETCH();
	uint16_t op3 = FETCH();
	uint32_t rn1 = op2 >> 15 ? regs.a[ op2 >> 12 & 7 ] : regs.d[ op2 >> 12 & 7];
	uint32_t rn2 = op3 >> 15 ? regs.a[ op3 >> 12 & 7 ] : regs.d[ op3 >> 12 & 7];
	int du1 = op2 >> 6 & 7;
	int dc1 = op2 & 7;
	int du2 = op3 >> 6 & 7;
	int dc2 = op3 & 7;
	uint16_t v1 = read16( rn1);
	uint16_t v2 = read16( rn2);
	DO_CMP_W( v1, regs.d[ dc1]);	
	if( regs.z ) {
		DO_CMP_W( v2, regs.d[ dc2]);
		if( regs.z ) {
			write16( rn1, regs.d[ du1]);
			write16( rn2, regs.d[ du2]);
			return;
		}
	}
	EA_WRITE16(0, dc1, v1);
	EA_WRITE16(0, dc2, v2);
}

OP( cas2_l ) {
	uint16_t op2 = FETCH();
	uint16_t op3 = FETCH();
	uint32_t rn1 = regs.r[ op2 >> 12 & 15 ];
	uint32_t rn2 = regs.r[ op3 >> 12 & 15 ];
	int du1 = op2 >> 6 & 7;
	int dc1 = op2 & 7;
	int du2 = op3 >> 6 & 7;
	int dc2 = op3 & 7;
	uint32_t v1 = read32( rn1);
	uint32_t v2 = read32( rn2);
	DO_CMP_L( v1, regs.d[ dc1]);	
	if( regs.z ) {
		DO_CMP_L( v2, regs.d[ dc2]);
		if( regs.z ) {
			write32( rn1, regs.d[ du1]);
			write32( rn2, regs.d[ du2]);
			return;
		}
	}
	regs.d[dc1] = v1;
	regs.d[dc2] = v2;
}



OP( cmpi_b ) {
	uint8_t v2 = FETCH();
	uint8_t v1 = EA_READ8( type, reg );
	DO_CMP_B(v1, v2);
}

OP( cmpi_w ) {
	uint16_t v2 = FETCH();
	uint16_t v1 = EA_READ16( type, reg );
	DO_CMP_W(v1, v2);
}

OP( cmpi_l ) {
	uint32_t v2 = FETCH32();
	uint32_t v1 = EA_READ32( type, reg );
	DO_CMP_L(v1, v2);
}

uint8_t read8s(uint32_t addr) {
	uint8_t v;
	paccess(paddr{ addr, 0, SZ::BYTE, TT::LFC, TM(regs.sfc), false}, &v);
	return v;
}
uint16_t read16s(uint32_t addr) {
	uint16_t v;
	paccess(paddr{ addr, 0, SZ::WORD, TT::LFC, TM(regs.sfc), false}, &v);
	return v;
}
uint32_t read32s(uint32_t addr) {
	uint32_t v;
	paccess(paddr{ addr, 0, SZ::LONG, TT::LFC, TM(regs.sfc), false}, &v);
	return v;
}
void write8s(uint32_t addr, uint8_t v) {
	paccess(paddr{ addr, 0, SZ::BYTE, TT::LFC, TM(regs.dfc), true}, &v);
}
void write16s(uint32_t addr, uint16_t v) {
	paccess(paddr{ addr, 0, SZ::WORD, TT::LFC, TM(regs.dfc), true}, &v);
}
void write32s(uint32_t addr, uint32_t v) {
	paccess(paddr{ addr, 0, SZ::LONG, TT::LFC, TM(regs.dfc), true}, &v);
}
OP( moves_b) {
	uint16_t op2 = FETCH();
	if( ! regs.S) {
		PRIV_ERROR();
	}
	int rg2 = op2 >> 12 & 7;
	bool w = op2 >> 11 & 1;
	uint32_t addr = EA_Addr( type, reg, 1, true );
	if( w ) {
		write8s( addr, regs.r[rg2]);
	} else {
		EA_WRITE8(0, rg2, read8s(addr));
	}
}

OP( moves_w) {
	uint16_t op2 = FETCH();
	if( ! regs.S) {
		PRIV_ERROR();
	}
	int rg2 = op2 >> 12 & 7;
	bool w = op2 >> 11 & 1;
	uint32_t addr = EA_Addr( type, reg, 2, true );
	if( w ) {
		write16s( addr, regs.r[rg2]);
	} else {
		EA_WRITE16(0, rg2, read8s(addr));
	}
}

OP( moves_l) {
	uint16_t op2 = FETCH();
	if( ! regs.S) {
		PRIV_ERROR();
	}
	int rg2 = op2 >> 12 & 7;
	bool w = op2 >> 11 & 1;
	uint32_t addr = EA_Addr( type, reg, 2, true );
	if( w ) {
		write32s( addr, regs.r[rg2]);
	} else {
		regs.r[ rg2 ] = read32s(addr);
	}
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

OP( movep_w_from) {
	int16_t disp = FETCH();
	uint32_t addr = regs.a[ reg ] + disp;
	EA_WRITE16( 0, dm, read8( addr) << 8 | read8( addr + 2));
}

OP( movep_l_from) {
	int16_t disp = FETCH();
	uint32_t addr = regs.a[ reg ] + disp;
	regs.d[ dm ] = read8( addr) << 24 |
	read8(addr + 2) << 16 |
	read8(addr + 4) << 8 |
	read8(addr + 6);
}

OP( movep_w_to) {
	int16_t disp = FETCH();
	uint32_t addr = regs.a[ reg ] + disp;
	uint16_t v = regs.d[ dm ];
	write8( addr, v >> 8);
	write8( addr + 2, v);
}

OP( movep_l_to) {
	int16_t disp = FETCH();
	uint32_t addr = regs.a[ reg ] + disp;
	uint32_t v = regs.d[ dm ];
	write8( addr, v >> 24);
	write8( addr + 2, v >> 16);
	write8( addr + 4, v >> 8);
	write8( addr + 6, v);
}

OP( move_b) {
	uint8_t v = EA_READ8( type, reg );
	regs.i_ea = 0;
	TEST_NZ8(v);
	regs.c = false;
	regs.v = false;
	EA_WRITE8( xop >> 6 & 7, dm, v);
}

OP( move_w) {
	uint16_t v = EA_READ16( type, reg );
	regs.i_ea = 0;
	TEST_NZ16(v);
	regs.c = false;
	regs.v = false;
	EA_WRITE16( xop >> 6 & 7, dm, v);
}

OP( move_l) {
	uint32_t v = EA_READ32( type, reg );
	regs.i_ea = 0;
	TEST_NZ32(v);
	regs.c = false;
	regs.v = false;
	EA_WRITE32( xop >> 6 & 7, dm, v);
}

OP( movea_w) {
	int16_t v = EA_READ16( type, reg );
	regs.a[ dm ] = v;
}

OP( movea_l) {
	uint32_t v = EA_READ32( type, reg );
	regs.a[ dm ] = v;
}

OP( negx_b ) {
	EA_Update8(type, reg, [](auto v1) { 
		bool tp = regs.z; 
		uint8_t v = DO_SUB_B( 0, v1 + regs.x); 
		if( v ) {
			regs.z = false;
		} else {
			regs.z = tp;
		}
		return v;
	});
}


OP( negx_w ) {
	EA_Update16(type, reg, [](auto v1) { 
		bool tp = regs.z; 
		uint16_t v = DO_SUB_W( 0, v1 + regs.x); 
		if( v ) {
			regs.z = false;
		} else {
			regs.z = tp;
		}
		return v;
	});
}

OP( negx_l ) {
	EA_Update32(type, reg, [](auto v1) { 
		bool tp = regs.z; 
		uint32_t v = DO_SUB_L( 0, v1 + regs.x); 
		if( v ) {
			regs.z = false;
		} else {
			regs.z = tp;
		}
		return v;
	});
}

OP( move_from_sr ) {
	if( ! regs.S) {
		PRIV_ERROR();
	}
	EA_WRITE16( type, reg,  GET_SR() );
}

OP( clr_b ) {
	EA_WRITE8( type, reg, 0);
	regs.n = regs.v = regs.c = false;
	regs.z = true;
}


OP( clr_w ) {
	EA_WRITE16( type, reg, 0);
	regs.n = regs.v = regs.c = false;
	regs.z = true;
}

OP( clr_l ) {
	EA_WRITE32( type, reg, 0);
	regs.n = regs.v = regs.c = false;
	regs.z = true;
}

OP( move_from_ccr ) {
	EA_WRITE16( type, reg,  GET_CCR() );
}

OP( neg_b ) {
	EA_Update8(type, reg, [](auto v1) { return DO_SUB_B( 0, v1); });
}


OP( neg_w ) {
	EA_Update16(type, reg, [](auto v1) { return DO_SUB_W( 0, v1); });
}

OP( neg_l ) {
	EA_Update32(type, reg, [](auto v1) { return DO_SUB_L( 0, v1); });
}

OP( move_to_ccr ) {
	SET_CCR( EA_READ16( type, reg));
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
OP( move_to_sr ) {
	if( ! regs.S) {
		PRIV_ERROR();
	}
	SET_SR( EA_READ16(type, reg));
}

int from_bcd(uint8_t v) {
	return ((v >> 4)&0xf)*10 + (v&0xf);
}
uint8_t to_bcd(int v) {
	return ((v/10)%10)<<4 | (v%10);
}

OP( nbcd ) {
	EA_Update8(type, reg, [](auto v1) { 
		int v = 100 - from_bcd(v1) - regs.x;
		bool cc = (v1 %100) != 0;
		regs.x = regs.c = cc;
		if( cc ) {
			regs.z = false;
		} 
		return to_bcd(v);
	});
}

OP( link_l) {
	int32_t disp = FETCH32();
	PUSH32( regs.a[ reg ]);
	regs.a[ reg ] = regs.a[7];
	regs.a[7] += disp;
}

OP( swap ) {
	uint32_t v = regs.d[ reg ];
	v = ( v >> 16 ) | ( v << 16);
	TEST_NZ32( v);
	regs.v = regs.c = false;
	regs.d[ reg ] = v;
}

OP( bkpt ) {
	ILLEGAL_INST();
}

OP( pea ) {
	regs.i_ea = EA_Addr( type, reg, 0, false);
	PUSH32( regs.i_ea );
}

OP( ext_w ) {
	int8_t v = regs.d[ reg];
	regs.v = regs.c = false;
	TEST_NZ16( v );
	EA_WRITE16(0, reg, static_cast<int16_t>( v ));
}

OP( ext_l ) {
	int16_t v = regs.d[ reg];
	regs.v = regs.c = false;
	TEST_NZ32( v );
	EA_WRITE32(0, reg, static_cast<int32_t>( v ));
}

OP( movem_to_w ) {
	uint16_t reg_list = FETCH();
	if( regs.in_movem == -1 ) {
		// start movem from
		regs.i_eav = EA_Addr( type, reg, 2, true);
	}
	if( type == 4) {
		if( regs.in_movem  == -1 ) {
			// start movem from
			regs.i_ea = regs.i_eav + 2; 
			regs.in_movem = 15;
		}
		for(int i = regs.in_movem; i >= 0; --i) {
			if( reg_list & (1 << (15-i)) ) {
				regs.i_ea -= 2;
				write16( regs.i_ea, regs.r[i] );
			}
		}
		regs.a[ reg ] = regs.i_ea;
	} else {
		if( regs.in_movem == -1 ) {
			// start movem from
			regs.in_movem = 0;
			regs.i_ea = regs.i_eav; 
		}
		for(int i = regs.in_movem; i < 16; ++i) {
			if( reg_list & (1 << i) ) {
				write16( regs.i_ea, regs.r[i] );
				regs.i_ea += 2;
			}
		}
	}
	regs.i_eav = 0;
	regs.in_movem = -1;
}

OP( movem_to_l ) {
	uint16_t reg_list = FETCH();
	if( regs.in_movem == -1 ) {
		// start movem from
		regs.i_eav = EA_Addr( type, reg, 4, true);
	}
	if( type == 4) {
		if( regs.in_movem  == -1 ) {
			// start movem from
			regs.i_ea = regs.i_eav + 4; 
			regs.in_movem = 15;
		}
		for(int i = regs.in_movem; i >= 0; --i) {
			if( reg_list & (1 << (15-i)) ) {
				regs.i_ea -= 4;
				write32( regs.i_ea, regs.r[i] );
			}
		}
		regs.a[ reg ] = regs.i_ea;
	} else {
		if( regs.in_movem == -1 ) {
			// start movem from
			regs.in_movem = 0;
			regs.i_ea = regs.i_eav; 
		}
		for(int i = regs.in_movem; i < 16; ++i) {
			if( reg_list & (1 << i) ) {
				write32( regs.i_ea, regs.r[i] );
				regs.i_ea += 4;
			}
		}
	}
	regs.i_eav = 0;
	regs.in_movem = -1;
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

OP( illegal ) {
	ILLEGAL_INST();
}

OP( mul_l) {
	uint16_t op2 = FETCH();
	int dl = op2 >> 12 & 7;
	int dh = op2 & 7;
	regs.c = false;
	if( op2 & 1 << 11) {
		// muls
		int64_t v1 = static_cast<int32_t>( EA_READ32( type, reg));
		int64_t v2 = static_cast<int32_t>( regs.d[ dl ]);
		int64_t ret = v1 * v2;
		regs.n = ret < 0;
		regs.z = ret == 0;
		regs.d[ dl ] = static_cast<uint32_t>( ret & 0xffffffff);
		int32_t high = ret >> 32;
		if( op2 & 1 << 10) {
			regs.d[ dh ] = high;
		} else {
			regs.v = high != 0;
		}
	} else {
		// mulu
		uint64_t v1 = EA_READ32( type, reg);
		uint64_t v2 = regs.d[ dl ];
		uint64_t ret = v1 * v2;
		regs.n = ret >> 63 & 1;
		regs.z = ret == 0;
		regs.d[ dl ] = static_cast<uint32_t>(ret);
		uint32_t high = ret >> 32;
		if( op2 & 1 << 10) {
			regs.d[ dh ] = high;
		} else {
			regs.v = high != 0;
		}
	}
}
std::unordered_map<uint16_t, void (*)(uint16_t, int, int, int)> opc_map;
static void build_cpufunctbl ()
{
	opc_map.clear();

	// EA=DR
	for(int i = 0; i < 7; ++i ) {
		opc_map[ 0000000 | i ] = op_ori_b;
		opc_map[ 0000100 | i ] = op_ori_w;
		opc_map[ 0000200 | i ] = op_ori_l;
		opc_map[ 0001000 | i ] = op_andi_b;
		opc_map[ 0001100 | i ] = op_andi_w;
		opc_map[ 0001200 | i ] = op_andi_l;
		opc_map[ 0002000 | i ] = op_subi_b;
		opc_map[ 0002100 | i ] = op_subi_w;
		opc_map[ 0002200 | i ] = op_subi_l;
		opc_map[ 0003000 | i ] = op_addi_b;
		opc_map[ 0003100 | i ] = op_addi_w;
		opc_map[ 0003200 | i ] = op_addi_l;

		opc_map[ 0004000 | i ] = op_btst_l_imm;
		opc_map[ 0004100 | i ] = op_bchg_l_imm;
		opc_map[ 0004200 | i ] = op_bclr_l_imm;
		opc_map[ 0004300 | i ] = op_bset_l_imm;

		opc_map[ 0005000 | i ] = op_eori_b;
		opc_map[ 0005100 | i ] = op_eori_w;
		opc_map[ 0005200 | i ] = op_eori_l;

		opc_map[ 0006000 | i ] = op_cmpi_b;
		opc_map[ 0006100 | i ] = op_cmpi_w;
		opc_map[ 0006200 | i ] = op_cmpi_l;

		for( int dn = 0; dn < 8; ++dn) {
			opc_map[ 0000400 | dn << 9 | i ] = op_btst_l_dm;
			opc_map[ 0000500 | dn << 9 | i ] = op_bchg_l_dm;
			opc_map[ 0000600 | dn << 9 | i ] = op_bclr_l_dm;
			opc_map[ 0000700 | dn << 9 | i ] = op_bset_l_dm;

			for(int xg = 0; xg < 8; ++xg) {
				if( xg != 1) {
					opc_map[ 0010000 | xg << 6 | dn << 9 | i ] = op_move_b;
					opc_map[ 0020000 | xg << 6 | dn << 9 | i ] = op_move_l;
					opc_map[ 0030000 | xg << 6 | dn << 9 | i ] = op_move_w;
				} else {
					opc_map[ 0020100 | dn << 9 | i ] = op_movea_l;
					opc_map[ 0030100 | dn << 9 | i ] = op_movea_w;
				}
			}
		}

		opc_map[ 0040000 | i ] = op_negx_b;
		opc_map[ 0040100 | i ] = op_negx_w;
		opc_map[ 0040200 | i ] = op_negx_l;
		opc_map[ 0040300 | i ] = op_move_from_sr;

		opc_map[ 0041000 | i ] = op_clr_b;
		opc_map[ 0041100 | i ] = op_clr_w;
		opc_map[ 0041200 | i ] = op_clr_l;
		opc_map[ 0041300 | i ] = op_move_from_ccr;

		opc_map[ 0042000 | i ] = op_neg_b;
		opc_map[ 0042100 | i ] = op_neg_w;
		opc_map[ 0042200 | i ] = op_neg_l;
		opc_map[ 0042300 | i ] = op_move_to_ccr;

		opc_map[ 0043000 | i ] = op_not_b;
		opc_map[ 0043100 | i ] = op_not_w;
		opc_map[ 0043200 | i ] = op_not_l;
		opc_map[ 0043300 | i ] = op_move_to_sr;

		opc_map[ 0044000 | i ] = op_nbcd;
		opc_map[ 0044100 | i ] = op_swap;
		opc_map[ 0044200 | i ] = op_ext_w;
		opc_map[ 0044300 | i ] = op_ext_l;

		opc_map[ 0045000 | i ] = op_tst_b;
		opc_map[ 0045100 | i ] = op_tst_w;
		opc_map[ 0045200 | i ] = op_tst_l;
		opc_map[ 0045300 | i ] = op_tas;

		opc_map[ 0046000 | i ] = op_mul_l;

	}

	// EA=AR
	for(int an = 0; an < 8; ++an) {
		for( int dn = 0; dn < 8; ++dn) {
			opc_map[ 0000410 | dn << 9 | an ] = op_movep_w_from;
			opc_map[ 0000510 | dn << 9 | an ] = op_movep_l_from;
			opc_map[ 0000610 | dn << 9 | an ] = op_movep_w_to;
			opc_map[ 0000710 | dn << 9 | an ] = op_movep_l_to;
		}
		opc_map[ 0044010 | an] = op_link_l;
		opc_map[ 0044110 | an] = op_bkpt;

		opc_map[ 0045110 | an ] = op_tst_w;
		opc_map[ 0045210 | an ] = op_tst_l;

	}

	// EA=OTHER
	for(int i = 020; i < 074 ; ++i ) {
		opc_map[ 0000000 | i ] = op_ori_b;
		opc_map[ 0000100 | i ] = op_ori_w;
		opc_map[ 0000200 | i ] = op_ori_l;
		opc_map[ 0000300 | i ] = op_cmp2_chk2_b;

		opc_map[ 0001000 | i ] = op_andi_b;
		opc_map[ 0001100 | i ] = op_andi_w;
		opc_map[ 0001200 | i ] = op_andi_l;
		opc_map[ 0001300 | i ] = op_cmp2_chk2_w;

		opc_map[ 0002000 | i ] = op_subi_b;
		opc_map[ 0002100 | i ] = op_subi_w;
		opc_map[ 0002200 | i ] = op_subi_l;
		opc_map[ 0002300 | i ] = op_cmp2_chk2_l;

		opc_map[ 0003000 | i ] = op_addi_b;
		opc_map[ 0003100 | i ] = op_addi_w;
		opc_map[ 0003200 | i ] = op_addi_l;

		opc_map[ 0004000 | i ] = op_btst_b_imm;
		opc_map[ 0004100 | i ] = op_bchg_b_imm;
		opc_map[ 0004200 | i ] = op_bclr_b_imm;
		opc_map[ 0004300 | i ] = op_bset_b_imm;

		opc_map[ 0005000 | i ] = op_eori_b;
		opc_map[ 0005100 | i ] = op_eori_w;
		opc_map[ 0005200 | i ] = op_eori_l;		
		opc_map[ 0005300 | i ] = op_cas_b;

		opc_map[ 0006000 | i ] = op_cmpi_b;
		opc_map[ 0006100 | i ] = op_cmpi_w;
		opc_map[ 0006200 | i ] = op_cmpi_l;
		opc_map[ 0006300 | i ] = op_cas_w;

		opc_map[ 0007000 | i ] = op_moves_b;
		opc_map[ 0007100 | i ] = op_moves_w;
		opc_map[ 0007200 | i ] = op_moves_l;
		opc_map[ 0007300 | i ] = op_cas_l;

		opc_map[ 0040000 | i ] = op_negx_b;
		opc_map[ 0040100 | i ] = op_negx_w;
		opc_map[ 0040200 | i ] = op_negx_l;
		opc_map[ 0040300 | i ] = op_move_from_sr;

		opc_map[ 0041000 | i ] = op_clr_b;
		opc_map[ 0041100 | i ] = op_clr_w;
		opc_map[ 0041200 | i ] = op_clr_l;
		opc_map[ 0041300 | i ] = op_move_from_ccr;


		opc_map[ 0042000 | i ] = op_neg_b;
		opc_map[ 0042100 | i ] = op_neg_w;
		opc_map[ 0042200 | i ] = op_neg_l;
		opc_map[ 0042300 | i ] = op_move_to_ccr;

		opc_map[ 0043000 | i ] = op_not_b;
		opc_map[ 0043100 | i ] = op_not_w;
		opc_map[ 0043200 | i ] = op_not_l;
		opc_map[ 0043300 | i ] = op_move_to_sr;

		opc_map[ 0044000 | i ] = op_nbcd;
		opc_map[ 0044100 | i ] = op_pea;
		opc_map[ 0044200 | i ] = op_movem_to_w;
		opc_map[ 0044300 | i ] = op_movem_to_l;

		opc_map[ 0045000 | i ] = op_tst_b;
		opc_map[ 0045100 | i ] = op_tst_w;
		opc_map[ 0045200 | i ] = op_tst_l;
		opc_map[ 0045300 | i ] = op_tas;

		opc_map[ 0046000 | i ] = op_mul_l;

		for( int dn = 0; dn < 8; ++dn) {
			opc_map[ 0000400 | dn << 9 | i ] = op_btst_b_dm;
			opc_map[ 0000500 | dn << 9 | i ] = op_bchg_b_dm;
			opc_map[ 0000600 | dn << 9 | i ] = op_bclr_b_dm;
			opc_map[ 0000700 | dn << 9 | i ] = op_bset_b_dm;

			for(int xg = 0; xg < 8; ++xg) {
				if( xg != 1) {
					opc_map[ 0010000 | xg << 6 | dn << 9 | i ] = op_move_b;
					opc_map[ 0020000 | xg << 6 | dn << 9 | i ] = op_move_l;
					opc_map[ 0030000 | xg << 6 | dn << 9 | i ] = op_move_w;
				} else {
					opc_map[ 0020100 | dn << 9 | i ] = op_movea_l;
					opc_map[ 0030100 | dn << 9 | i ] = op_movea_w;

				}
			}

		}

	}

	// EA=IMM
	opc_map[ 0000074] = op_ori_b_ccr;
	opc_map[ 0000174] = op_ori_w_sr;
	opc_map[ 0001074] = op_andi_b_ccr;
	opc_map[ 0001174] = op_andi_w_sr;
	opc_map[ 0005074] = op_eori_b_ccr;
	opc_map[ 0005174] = op_eori_w_sr;
	opc_map[ 0006374] = op_cas2_w;
	opc_map[ 0007374] = op_cas2_l;
	opc_map[ 0043374] = op_move_to_sr;
	opc_map[ 0045374] = op_illegal; 
	for( int dn = 0; dn < 8; ++dn) {
			for(int xg = 0; xg < 8; ++xg) {
				if( xg != 1 ) {
					opc_map[ 0010074 | xg << 6 | dn << 9 ] = op_move_b;
					opc_map[ 0020074 | xg << 6 | dn << 9 ] = op_move_l;
					opc_map[ 0030074 | xg << 6 | dn << 9 ] = op_move_w;
				} else {
					opc_map[ 0020174 | dn << 9 ] = op_movea_l;
					opc_map[ 0030174 | dn << 9 ] = op_movea_w;
				}
			}

		}
}

void init_m68k ()
{
	build_cpufunctbl();
//	fpu_init(CPUType == 4);
}

void exit_m68k (void)
{
//	fpu_exit ();
}

int lastint_no;

#define get_ibyte_1(o) get_byte(get_virtual_address(regs.pc) + (o) + 1)
#define get_iword_1(o) get_word(get_virtual_address(regs.pc) + (o))
#define get_ilong_1(o) get_long(get_virtual_address(regs.pc) + (o))

#if 0
int m68k_do_specialties (void)
{
#if USE_JIT
	// Block was compiled
	SPCFLAGS_CLEAR( SPCFLAG_JIT_END_COMPILE );

	// Retain the request to get out of compiled code until
	// we reached the toplevel execution, i.e. the one that
	// can compile then run compiled code. This also means
	// we processed all (nested) EmulOps
	if ((m68k_execute_depth == 0) && SPCFLAGS_TEST( SPCFLAG_JIT_EXEC_RETURN ))
		SPCFLAGS_CLEAR( SPCFLAG_JIT_EXEC_RETURN );
#endif

	if (SPCFLAGS_TEST( SPCFLAG_DOTRACE )) {
		Exception (9,last_trace_ad);
	}
	while (SPCFLAGS_TEST( SPCFLAG_STOP )) {
		if (SPCFLAGS_TEST( SPCFLAG_INT | SPCFLAG_DOINT )){
			SPCFLAGS_CLEAR( SPCFLAG_INT | SPCFLAG_DOINT );
			int intr = intlev ();
			if (intr != -1 && intr > regs.intmask) {
				Interrupt (intr);
				regs.stopped = 0;
				SPCFLAGS_CLEAR( SPCFLAG_STOP );
			}
		}
	}
	if (SPCFLAGS_TEST( SPCFLAG_TRACE ))
		do_trace ();

	if (SPCFLAGS_TEST( SPCFLAG_DOINT )) {
		SPCFLAGS_CLEAR( SPCFLAG_DOINT );
		int intr = intlev ();
		if (intr != -1 && intr > regs.intmask) {
			Interrupt (intr);
			regs.stopped = 0;
		}
	}
	if (SPCFLAGS_TEST( SPCFLAG_INT )) {
		SPCFLAGS_CLEAR( SPCFLAG_INT );
		SPCFLAGS_SET( SPCFLAG_DOINT );
	}
	if (SPCFLAGS_TEST( SPCFLAG_BRK )) {
		SPCFLAGS_CLEAR( SPCFLAG_BRK );
		return 1;
	}
	return 0;
}
#endif
void m68k_do_execute ()
{
	if( setjmp( ex_jmp) == 0 ) {
		regs.opc = regs.pc;
		uint16_t opc = FETCH();
		auto f = opc_map.find(opc);
		if( f != opc_map.end() ) {
			f->second(opc, opc >> 9 & 7, opc >> 3 & 7, opc & 7);
		} else {
			ILLEGAL_INST();
		}
		if (SPCFLAGS_TEST(SPCFLAG_ALL_BUT_EXEC_RETURN)) {
//				if (m68k_do_specialties())
//					return;
		}
	} else {
		// go back here
	}
}


void m68k_execute (void)
{
	regs.i_ea = regs.i_eav = 0;
	while (quit_program) {
		m68k_do_execute();
	}
}

