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

uint32_t get_ea(uint32_t base) {
	uint16_t nextw = FETCH();
	bool full = (nextw >> 8 & 1);
	int reg = nextw > 12 & 15;
	bool wl = nextw >> 11 & 1;
	int scale = nextw >> 9 & 3;
	uint32_t xn = regs.r[reg];
	if( !wl ) {
		xn = (int16_t)xn;
	}
	if( ! full ) {
		int8_t disp = nextw & 0xff;
		return base + xn << scale + disp;
	} else {
		bool bs = nextw >> 7 & 1;
		bool is = nextw >> 6 & 1;
		int bd_c = nextw >> 4 & 3;
		bool pre = nextw >> 2 & 1;
		int od_c = nextw & 3;
		int32_t bd = 0;
		int32_t od = 0;
		switch( bd_c ) {
		case 0 :
			ILLEGAL_INST();
		case 1 :
			break;
		case 2 :
			bd = (int16_t)FETCH();
			break;
		case 3 :
			bd = FETCH32();
			break;
		}
		uint32_t ret = bd;
		if( ! bs ) {
			ret += base;
		}
		switch( od_c ) {
		case 0 :
			if( pre ) {
				ILLEGAL_INST();
			}			
			if( ! is ) {
				ret += xn << scale;
			}
			return ret;
		case 1 :
			break;
		case 2 :
			od = (int16_t)FETCH();
			break;
		case 3 :
			od = FETCH32();
			break;
		}
		if( ! pre ) {
			if( ! is ) {
				ret += xn << scale;
			}
			uint32_t vx = read32( ret );
			return vx + od;
		} else {
			uint32_t vx = read32( ret );
			return vx + xn << scale + od;
		}
	}
}
enum class EA_OP { DR, AR, MEM, INCR, DECR, OFFSET, EXT, EXT2 };
uint32_t EA_Addr(int type, int reg, int sz, bool w) {
	uint32_t& base = regs.a[reg];
	switch( EA_OP{type} ) {
	case EA_OP::DR     : ILLEGAL_INST();
	case EA_OP::AR     : ILLEGAL_INST();
	case EA_OP::MEM    : return base;
	case EA_OP::INCR   : 
	if( sz ) {
		return std::exchange(base, base + sz);
	} else {
		ILLEGAL_INST();
	}
	case EA_OP::DECR   :
	if( sz ) { 
		return base -= sz;
	} else {
		ILLEGAL_INST();
	}
	case EA_OP::OFFSET : return base + (int16_t)FETCH();
	case EA_OP::EXT    : return get_ea(base);
	case EA_OP::EXT2 :
		switch( reg ) {
		case 0 : return FETCH();
		case 1 : return FETCH32();
		case 2 :
			if( ! w ) {
				return regs.pc + (int16_t)FETCH();
			}
			break;
		case 3 :
			if( ! w ) {
				return get_ea(regs.pc);
			}
			break;
		}
	}
	ILLEGAL_INST();
}

void set_dr_b(int rn, uint8_t v) {
	regs.d[rn] = regs.d[rn] & 0xffffff00 | v ;
}

void set_dr_w(int rn, uint16_t v) {
	regs.d[rn] = regs.d[rn] & 0xffff0000 | v ;
}

#define OP(name) void op_##name(int __attribute__((unused)) dm, int __attribute__((unused)) type, int __attribute__((unused)) reg)
OP(ori_b_d) {
	uint8_t v1 = regs.d[reg];	
	uint8_t v2 = FETCH();
	set_dr_b(reg, DO_OR_B( v1, v2));
}

OP(ori_b) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8( regs.i_ea );
	uint8_t v2 = FETCH();
	write8(regs.i_ea, DO_OR_B( v1, v2) );
}

OP(ori_w_d) {
	uint16_t v1 = regs.d[reg];	
	uint16_t v2 = FETCH();
	set_dr_w(reg, DO_OR_W( v1, v2));
}

OP(ori_w) {
	regs.i_ea = EA_Addr( type, reg, 2, true );
	uint16_t v1 = read16(regs.i_ea);
	uint16_t v2 = FETCH();
	write16(regs.i_ea, DO_OR_W( v1, v2)  );
}

OP(ori_l_d) {
	uint32_t v1 = regs.d[reg];	
	uint32_t v2 = FETCH32();
	regs.d[reg] = DO_OR_L( v1, v2);
}

OP(ori_l) {
	regs.i_ea = EA_Addr( type, reg, 4, true );
	uint32_t v1 = read32(regs.i_ea);
	uint32_t v2 = FETCH32();
	write32(regs.i_ea, DO_OR_L( v1, v2) );
}

OP(ori_b_ccr) {
	SET_CCR( GET_CCR() | (FETCH() & 0xff) );
}

OP(ori_w_sr) {
	if( ! regs.S) {
		PRIV_ERROR();
	}
	SET_SR( GET_SR() | FETCH() );
}

OP(andi_b_d) {
	uint8_t v1 = regs.d[reg];	
	uint8_t v2 = FETCH();
	set_dr_b(reg, DO_AND_B( v1, v2));
}

OP(andi_b) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8( regs.i_ea );
	uint8_t v2 = FETCH();
	write8(regs.i_ea, DO_AND_B( v1, v2) );
}

OP(andi_w_d) {
	uint16_t v1 = regs.d[reg];	
	uint16_t v2 = FETCH();
	set_dr_w(reg, DO_AND_W( v1, v2));
}

OP(andi_w) {
	regs.i_ea = EA_Addr( type, reg, 2, true );
	uint16_t v1 = read16( regs.i_ea );
	uint16_t v2 = FETCH();
	write16(regs.i_ea, DO_AND_W( v1, v2) );
}

OP(andi_l_d) {
	uint32_t v1 = regs.d[reg];	
	uint32_t v2 = FETCH32();
	regs.d[reg] = DO_AND_L( v1, v2);
}

OP(andi_l) {
	regs.i_ea = EA_Addr( type, reg, 4, true );
	uint32_t v1 = read32( regs.i_ea );
	uint32_t v2 = FETCH32();
	write32(regs.i_ea, DO_AND_L( v1, v2) );
}

OP(andi_b_ccr) {
	SET_CCR( GET_CCR() & (FETCH() & 0xff) );
}

OP(andi_w_sr) {
	if( ! regs.S) {
		PRIV_ERROR();
	}
	SET_SR( GET_SR() & FETCH() );
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

OP( subi_b_d) {
	uint8_t v1 = regs.d[reg];	
	uint8_t v2 = FETCH();
	set_dr_b( reg, DO_SUB_B(v1, v2));
}

OP( subi_b ) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8( regs.i_ea );
	uint8_t v2 = FETCH();
	write8(regs.i_ea, DO_SUB_B(v1, v2) );
}

OP( subi_w_d) {
	uint16_t v1 = regs.d[reg];	
	uint16_t v2 = FETCH();
	set_dr_w( reg, DO_SUB_W(v1, v2));
}

OP( subi_w ) {
	regs.i_ea = EA_Addr( type, reg, 2, true );
	uint16_t v1 = read16( regs.i_ea );
	uint16_t v2 = FETCH();
	write16(regs.i_ea, DO_SUB_W(v1, v2) );
}

OP( subi_l_d) {
	uint32_t v1 = regs.d[reg];	
	uint32_t v2 = FETCH32();
	regs.d[reg] = DO_SUB_L(v1, v2);
}

OP( subi_l ) {
	regs.i_ea = EA_Addr( type, reg, 4, true );
	uint32_t v1 = read32( regs.i_ea );
	uint32_t v2 = FETCH32();
	write32(regs.i_ea, DO_SUB_L(v1, v2) );
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

OP( addi_b_d ) {
	uint8_t v1 = regs.d[reg];	
	uint8_t v2 = FETCH();
	set_dr_b( reg, DO_ADD_B(v1, v2));
}

OP( addi_b ) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8( regs.i_ea );
	uint8_t v2 = FETCH();
	write8(regs.i_ea, DO_ADD_B(v1, v2) );
}

OP( addi_w ) {
	regs.i_ea = EA_Addr( type, reg, 2, true );
	uint16_t v1 = read16( regs.i_ea );
	uint16_t v2 = FETCH();
	write16(regs.i_ea, DO_ADD_W(v1, v2) );
}

OP( addi_w_d ) {
	uint16_t v1 = regs.d[reg];	
	uint16_t v2 = FETCH();
	set_dr_w( reg, DO_ADD_W(v1, v2));
}

OP( addi_l ) {
	regs.i_ea = EA_Addr( type, reg, 4, true );
	uint32_t v1 = read32( regs.i_ea );
	uint32_t v2 = FETCH32();
	write32(regs.i_ea,  DO_ADD_L(v1, v2) );
}

OP( addi_l_d ) {
	uint32_t v1 = regs.d[reg];	
	uint32_t v2 = FETCH32();
	regs.d[reg] = DO_ADD_L(v1, v2);
}


OP( btst_l_imm ) {
	uint32_t v1 = regs.d[reg];	
	int bn = FETCH() & 31;
	regs.z = ! (( v1 >> bn) & 1);
}

OP( btst_b_imm ) {
	regs.i_ea = EA_Addr( type, reg, 1, false );
	uint8_t v1 = read8(regs.i_ea);
	int bn = FETCH() & 7;
	regs.z = ! (( v1 >> bn) & 1);
}

OP( bchg_l_imm ) {
	uint32_t v1 = regs.d[reg];	
	int bn = FETCH() & 31;
	regs.z = ! (( v1 >> bn) & 1);
	regs.d[reg] ^= 1 << bn;
}

OP( bclr_l_imm ) {
	uint32_t v1 = regs.d[reg];	
	int bn = FETCH() & 31;
	regs.z = ! (( v1 >> bn) & 1);
	regs.d[reg] &= ~(1 << bn);
}

OP( bset_l_imm ) {
	uint32_t v1 = regs.d[reg];	
	int bn = FETCH() & 31;
	regs.z = ! (( v1 >> bn) & 1);
	regs.d[reg] |= (1 << bn);
}



OP( bchg_b_imm ) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8(regs.i_ea);
	int bn = FETCH() & 7;
	regs.z = ! (( v1 >> bn) & 1);
	write8( regs.i_ea, v1 ^ (1 << bn));
}

OP( bclr_b_imm ) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8(regs.i_ea);
	int bn = FETCH() & 7;
	regs.z = ! (( v1 >> bn) & 1);
	write8( regs.i_ea, v1 &~ (1 << bn));
}

OP( bset_b_imm ) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8(regs.i_ea);
	int bn = FETCH() & 7;
	regs.z = ! (( v1 >> bn) & 1);
	write8( regs.i_ea, v1 | (1 << bn));
}

OP( eori_b_d ) {
	uint8_t v1 = regs.d[reg];	
	uint8_t v2 = FETCH();
	set_dr_b( reg, DO_EOR_B(v1, v2));
}

OP( eori_b ) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8( regs.i_ea );
	uint8_t v2 = FETCH();
	write8(regs.i_ea, DO_EOR_B( v1, v2) );
}

OP( eori_w_d ) {
	uint16_t v1 = regs.d[reg];	
	uint16_t v2 = FETCH();
	set_dr_w( reg, DO_EOR_W(v1, v2));
}
OP( eori_w ) {
	regs.i_ea = EA_Addr( type, reg, 2, true );
	uint16_t v1 = read16( regs.i_ea );
	uint16_t v2 = FETCH();
	write16(regs.i_ea, DO_EOR_W( v1, v2) );
}

OP( eori_l_d ) {
	uint32_t v1 = regs.d[reg];	
	uint32_t v2 = FETCH32();
	regs.d[reg] = DO_EOR_L( v1, v2);
}
OP( eori_l ) {
	regs.i_ea = EA_Addr( type, reg, 4, true );
	uint32_t v1 = read32( regs.i_ea );
	uint32_t v2 = FETCH32();
	write32(regs.i_ea, DO_EOR_L( v1, v2) );
}

OP( eori_b_ccr) {
	SET_CCR( GET_CCR() & (FETCH() & 0xff) );
}

OP( eori_w_sr ) {
	if( ! regs.S) {
		PRIV_ERROR();
	}
	SET_SR( GET_SR() & FETCH() );
}

OP( cas_b ) {
	uint16_t op2 = FETCH();
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t d = read8(regs.i_ea);
	DO_CMP_B( d, regs.d[ op2 & 7]);
	if( regs.z ) {
		write8( regs.i_ea, regs.d[ op2 >> 6 & 7]);
	} else {
		set_dr_b(op2&7, d);
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
		set_dr_w(op2&7, d);
	}
}

OP( cas_l ) {
	uint16_t op2 = FETCH();
	regs.i_ea = EA_Addr( type, reg, 4, true );
	uint32_t d = read32(regs.i_ea);
	DO_CMP_L( d, regs.d[ op2 & 7]);
	if( regs.z ) {
		WriteMacInt32( regs.i_ea, regs.d[ op2 >> 6 & 7]);
	} else {
		regs.d[ op2 & 7] = d;
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
	set_dr_w(dc1, v1);
	set_dr_w(dc2, v2);
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

OP( cmpi_b_d ) {
	uint8_t v1 = regs.d[reg];	
	uint8_t v2 = FETCH();
	DO_CMP_B(v1, v2);
}

OP( cmpi_b ) {
	regs.i_ea = EA_Addr( type, reg, 1, false );
	uint8_t v1 = read8( regs.i_ea );
	uint8_t v2 = FETCH();
	DO_CMP_B(v1, v2);
}

OP( cmpi_w_d ) {
	uint16_t v1 = regs.d[reg];	
	uint16_t v2 = FETCH();
	DO_CMP_W(v1, v2);
}

OP( cmpi_w ) {
	regs.i_ea = EA_Addr( type, reg, 2, false );
	uint16_t v1 = read16( regs.i_ea );
	uint16_t v2 = FETCH();
	DO_CMP_W(v1, v2);
}

OP( cmpi_l_d ) {
	uint32_t v1 = regs.d[reg];	
	uint32_t v2 = FETCH32();
	DO_CMP_L(v1, v2);
}

OP( cmpi_l ) {
	regs.i_ea = EA_Addr( type, reg, 4, false );
	uint32_t v1 = read32( regs.i_ea );
	uint32_t v2 = FETCH32();
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
	if( ! regs.S) {
		PRIV_ERROR();
	}
	uint16_t op2 = FETCH();
	int rg2 = op2 >> 12 & 7;
	bool w = op2 >> 11 & 1;
	uint32_t addr = EA_Addr( type, reg, 1, true );
	if( w ) {
		write8s( addr, regs.r[rg2]);
	} else {
		set_dr_b( rg2, read8s(addr));
	}
}

OP( moves_w) {
	if( ! regs.S) {
		PRIV_ERROR();
	}
	uint16_t op2 = FETCH();
	int rg2 = op2 >> 12 & 7;
	bool w = op2 >> 11 & 1;
	uint32_t addr = EA_Addr( type, reg, 2, true );
	if( w ) {
		write16s( addr, regs.r[rg2]);
	} else {
		set_dr_w( rg2, read16s(addr));
	}
}

OP( moves_l) {
	if( ! regs.S) {
		PRIV_ERROR();
	}
	uint16_t op2 = FETCH();
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

OP( btst_b_dm ) {
	regs.i_ea = EA_Addr( type, reg, 1, false );
	uint8_t v1 = read8(regs.i_ea);
	int bn = regs.d[ dm ] & 7;
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



OP( bchg_b_dm ) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8(regs.i_ea);
	int bn = regs.d[ dm ] & 7;
	regs.z = ! (( v1 >> bn) & 1);
	write8( regs.i_ea, v1 ^ (1 << bn));
}

OP( bclr_b_dm ) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8(regs.i_ea);
	int bn = regs.d[ dm ] & 7;
	regs.z = ! (( v1 >> bn) & 1);
	write8( regs.i_ea, v1 &~ (1 << bn));
}

OP( bset_b_dm ) {
	regs.i_ea = EA_Addr( type, reg, 1, true );
	uint8_t v1 = read8(regs.i_ea);
	int bn = regs.d[ dm ] & 7;
	regs.z = ! (( v1 >> bn) & 1);
	write8( regs.i_ea, v1 | (1 << bn));
}
std::unordered_map<uint16_t, void (*)(int, int, int)> opc_map;
static void build_cpufunctbl ()
{
	opc_map.clear();
	for(int i = 0; i < 7; ++i ) {
		opc_map[ 0000000 | i ] = op_ori_b_d;
		opc_map[ 0000100 | i ] = op_ori_w_d;
		opc_map[ 0000200 | i ] = op_ori_l_d;
		opc_map[ 0001000 | i ] = op_andi_b_d;
		opc_map[ 0001100 | i ] = op_andi_w_d;
		opc_map[ 0001200 | i ] = op_andi_l_d;
		opc_map[ 0002000 | i ] = op_subi_b_d;
		opc_map[ 0002100 | i ] = op_subi_w_d;
		opc_map[ 0002200 | i ] = op_subi_l_d;
		opc_map[ 0003000 | i ] = op_addi_b_d;
		opc_map[ 0003100 | i ] = op_addi_w_d;
		opc_map[ 0003200 | i ] = op_addi_l_d;

		opc_map[ 0004000 | i ] = op_btst_l_imm;
		opc_map[ 0004100 | i ] = op_bchg_l_imm;
		opc_map[ 0004200 | i ] = op_bclr_l_imm;
		opc_map[ 0004300 | i ] = op_bset_l_imm;

		opc_map[ 0005000 | i ] = op_eori_b_d;
		opc_map[ 0005100 | i ] = op_eori_w_d;
		opc_map[ 0005200 | i ] = op_eori_l_d;

		opc_map[ 0006000 | i ] = op_cmpi_b_d;
		opc_map[ 0006100 | i ] = op_cmpi_w_d;
		opc_map[ 0006200 | i ] = op_cmpi_l_d;

		for( int dn = 0; dn < 8; ++dn) {
			opc_map[ 0000400 | dn << 9 | i ] = op_btst_l_dm;
			opc_map[ 0000500 | dn << 9 | i ] = op_bchg_l_dm;
			opc_map[ 0000600 | dn << 9 | i ] = op_bclr_l_dm;
			opc_map[ 0000700 | dn << 9 | i ] = op_bset_l_dm;
		}


	}
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

		for( int dn = 0; dn < 8; ++dn) {
			opc_map[ 0000400 | dn << 9 | i ] = op_btst_b_dm;
			opc_map[ 0000500 | dn << 9 | i ] = op_bchg_b_dm;
			opc_map[ 0000600 | dn << 9 | i ] = op_bclr_b_dm;
			opc_map[ 0000700 | dn << 9 | i ] = op_bset_b_dm;
		}

	}
	opc_map[ 0000074] = op_ori_b_ccr;
	opc_map[ 0000174] = op_ori_w_sr;
	opc_map[ 0001074] = op_andi_b_ccr;
	opc_map[ 0001174] = op_andi_w_sr;
	opc_map[ 0005074] = op_eori_b_ccr;
	opc_map[ 0005174] = op_eori_w_sr;
	opc_map[ 0006374] = op_cas2_w;
	opc_map[ 0007374] = op_cas2_l;
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
			f->second(opc >> 9 & 7, opc >> 3 & 7, opc & 7);
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
	while (quit_program) {
		m68k_do_execute();
	}
}

