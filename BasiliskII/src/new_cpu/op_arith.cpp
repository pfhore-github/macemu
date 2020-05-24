#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "inst_cmn.hpp"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define OP(name_) extern "C" void op_##name_(uint16_t op, int dn, int mode, int reg)
#include "op.h"
#include "intel.h"


OP(addi_b) {
	int8_t imm = fetch_w();
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( ADD_B( cpu.ea_read(), imm) );
}


OP(addi_w) {
	uint16_t imm = fetch_w();
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( ADD_W( cpu.ea_read(), imm) );
}

OP(addi_l) {
	uint32_t imm = fetch_l();
	cpu.parse_EA( mode, 4, reg, true);
	cpu.ea_write( ADD_L( cpu.ea_read(), imm) );
}


OP(cas_b) {
	uint16_t nw = fetch_w();
	cpu.parse_EA( mode, 1, reg, true);
	int u = (nw >> 6) & 7;
	int c = nw & 7;
	uint8_t v = cpu.ea_read();
	CMP_B( v, cpu.R[c]);
	if( cpu.Z ) {
		cpu.ea_write(cpu.R[u]);
	} else {
		cpu.R[c] = v;
	}
	test_trace_branch();
}


OP(cmpi_b) {
	uint8_t imm = fetch_w();
	cpu.parse_EA( mode, 1, reg, false);
	CMP_B( cpu.ea_read(), imm);
}

OP(cmpi_w) {
	uint16_t imm = fetch_w();
	cpu.parse_EA( mode, 2, reg, false);
	CMP_W( cpu.ea_read(), imm);
}

OP(cmpi_l) {
	uint32_t imm = fetch_l();
	cpu.parse_EA( mode, 4, reg, false);
	CMP_L( cpu.ea_read(), imm);
}

OP(cas2_w) {
	uint16_t nw = fetch_w();
	uint16_t nw2 = fetch_w();
	int s1 = (nw >> 12) & 15;
	int u1 = (nw >> 6) & 7;
	int c1 = nw & 7;
	int s2 = (nw2 >> 12) & 15;
	int u2 = (nw2 >> 6) & 7;
	int c2 = nw2 & 7;
	CMP_W( read_w(cpu.R[s1]), cpu.R[c1]);
	if( cpu.Z ) {
		CMP_W( read_w(cpu.R[s2]), cpu.R[c2]);
		if( cpu.Z ) {
			write_w(cpu.R[s1], cpu.R[u1]);
			write_w(cpu.R[s2], cpu.R[u2]);
			goto end;
		}
	}
	cpu.R[c1] = read_w(cpu.R[s1]);
	cpu.R[c2] = read_w(cpu.R[s2]);
  end:
	test_trace_branch();
}
OP(cas_w) {
	uint16_t nw = fetch_w();
	cpu.parse_EA( mode, 2, reg, true);
	int u = (nw >> 6) & 7;
	int c = nw & 7;
	uint16_t v = cpu.ea_read();
	CMP_W( v, cpu.R[c]);
	if( cpu.Z ) {
		cpu.ea_write(cpu.R[u]);
	} else {
		cpu.R[c] = v;
	}
	test_trace_branch();
}

OP(addq_b) {
	cpu.parse_EA( mode, 1, reg, true);	
	int8_t v = cpu.ea_read();
	v = ADD_B( v, dn ? dn : 8);
	cpu.ea_write( v );
}

OP(addq_w) {
	cpu.parse_EA( mode, 2, reg, true);	
	int16_t v = cpu.ea_read();
	v = ADD_W( v, dn ? dn : 8);
	cpu.ea_write( v );
}

OP(addq_a) {
	cpu.R[8+ reg ] += dn ? dn : 8;
}
OP(addq_l) {
	if( mode != 1 ) {
		cpu.parse_EA( mode, 4, reg, true);	
		int32_t v = cpu.ea_read();
		v = ADD_L( v, dn ? dn : 8);
		cpu.ea_write( v );
	} else {
		cpu.R[8+ reg ] += dn ? dn : 8;
	}
}

OP(subq_b) {
	cpu.parse_EA( mode, 1, reg, true);	
	int8_t v = cpu.ea_read();
	v = SUB_B( v, dn ? dn : 8);
	cpu.ea_write( v );
}

OP(subq_w) {
	cpu.parse_EA( mode, 2, reg, true);	
	int16_t v = cpu.ea_read();
	v = SUB_W( v, dn ? dn : 8);
	cpu.ea_write( v );
}

OP(subq_l) {
	cpu.parse_EA( mode, 4, reg, true);	
	int32_t v = cpu.ea_read();
	v = SUB_L( v, dn ? dn : 8);
	cpu.ea_write( v );
}

OP(subq_a) {
	cpu.R[8+ reg ] -= dn ? dn : 8;
}

OP(subi_b) {
	uint8_t imm = fetch_w();
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write(SUB_B( cpu.ea_read(), imm));
}


OP(subi_w) {
	uint16_t imm = fetch_w();
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( SUB_W( cpu.ea_read(), imm));
}


OP(subi_l) {
	uint32_t imm = fetch_l();
	cpu.parse_EA( mode, 4, reg, true);
	cpu.ea_write( SUB_L( cpu.ea_read(), imm) );
}

OP(cmp2_b) {
	int wd = fetch_w();
	int8_t v = cpu.R[wd >> 12];
	cpu.parse_EA( mode, 1, reg, false);
	int8_t lw = read_b(cpu.ea_v);
	int8_t hi = read_b(cpu.ea_v+1);
	cpu.Z = ( v == lw || v == hi );
	cpu.C = ( v < lw || v > hi );
	if( ((wd >> 11) & 1) && cpu.C ) {
		throw Exception::ChkError();
	}
}





OP(cmp2_w) {
	int wd = fetch_w();
	int16_t v = cpu.R[wd >> 12];
	cpu.parse_EA( mode, 2, reg, false);
	int16_t lw = read_w(cpu.ea_v);
	int16_t hi = read_w(cpu.ea_v+2);
	cpu.Z = ( v == lw || v == hi );
	cpu.C = ( v < lw || v > hi );
	if( ((wd >> 11) & 1) && cpu.C ) {
		throw Exception::ChkError();
	}
}


OP(cmp2_l) {
	int wd = fetch_w();
	int32_t v = cpu.R[wd >> 12];
	cpu.parse_EA( mode, 4, reg, false);
	int32_t lw = read_l(cpu.ea_v);
	int32_t hi = read_l(cpu.ea_v+4);
	cpu.Z = ( v == lw || v == hi );
	cpu.C = ( v < lw || v > hi );
	if( ((wd >> 11) & 1) && cpu.C ) {
		throw Exception::ChkError();
	}
}

OP(cas2_l) {
	uint16_t nw = fetch_w();
	uint16_t nw2 = fetch_w();
	int s1 = (nw >> 12) & 15;
	int u1 = (nw >> 6) & 7;
	int c1 = nw & 7;
	int s2 = (nw2 >> 12) & 15;
	int u2 = (nw2 >> 6) & 7;
	int c2 = nw2 & 7;
	CMP_L( read_l(cpu.R[s1]), cpu.R[c1]);
	if( cpu.Z ) {
		CMP_L( read_l(cpu.R[s2]), cpu.R[c2]);
		if( cpu.Z ) {
			write_l(cpu.R[s1], cpu.R[u1]);
			write_l(cpu.R[s2], cpu.R[u2]);
			goto done;
		}
	}
	cpu.R[c1] = read_l(cpu.R[s1]);
	cpu.R[c2] = read_l(cpu.R[s2]);
  done:
	test_trace_branch();
}
OP(cas_l) {
	uint16_t nw = fetch_w();
	cpu.parse_EA( mode, 4, reg, true);
	int u = (nw >> 6) & 7;
	int c = nw & 7;
	uint32_t v = cpu.ea_read();
	CMP_L( v, cpu.R[c]);
	if( cpu.Z ) {
		cpu.ea_write(cpu.R[u]);
	} else {
		cpu.R[c] = v;
	}
	test_trace_branch();
}

OP(negx_b) { 
	cpu.parse_EA( mode, 1, reg, true);
	bool oldz = cpu.Z;
	uint8_t v = cpu.ea_read() + cpu.X;
	cpu.ea_write( SUB_B( 0, v) );
	cpu.Z &= oldz; 
	cpu.X = cpu.C;
}

OP(negx_w) { 
	cpu.parse_EA( mode, 2, reg, true);
	bool oldz = cpu.Z;
	uint16_t v = cpu.ea_read() + cpu.X;
	cpu.ea_write( SUB_W( 0, v) );
	cpu.Z &= oldz; 
	cpu.X = cpu.C;
}

OP(negx_l) { 
	cpu.parse_EA( mode, 4, reg, true);
	bool oldz = cpu.Z;
	uint32_t v = cpu.ea_read() + cpu.X;
	cpu.ea_write( SUB_L( 0, v) );
	cpu.Z &= oldz; 
	cpu.X = cpu.C;
}

OP(neg_b) {
	cpu.parse_EA( mode, 1, reg, true);
	uint8_t v = cpu.ea_read();
	cpu.ea_write( SUB_B( 0, v) );
	cpu.X = cpu.C;
}

OP(neg_w) {
	cpu.parse_EA( mode, 2, reg, true);
	uint16_t v = cpu.ea_read();
	cpu.ea_write( SUB_W( 0, v) );
	cpu.X = cpu.C;
}

OP(neg_l) {
	cpu.parse_EA( mode, 4, reg, true);
	uint32_t v = cpu.ea_read();
	cpu.ea_write( SUB_L( 0, v) );
	cpu.X = cpu.C;
}


OP(mul_l) {
	uint16_t nw = fetch_w();
	cpu.parse_EA( mode, 4, reg, false);
	uint32_t src = cpu.ea_read();
	int di = ( nw >> 12 ) & 7;
	int dh = nw & 7;
	if( !((nw >> 11) & 1) ) {
		if( (nw >> 10) & 1 ) {
			MUL_U( src, cpu.R[di], &cpu.R[dh], &cpu.R[di]);
		} else {
			uint32_t ret;
			cpu.V = __builtin_umul_overflow(src, cpu.R[di], &ret);
			cpu.C = false;
			set_nz( (int32_t)ret);
			cpu.R[di] = ret;
		}
	} else {
		if( (nw >> 10) & 1 ) {
			MUL_S( src, cpu.R[di], &cpu.R[dh], &cpu.R[di]);
		} else {
			int32_t ret;
			cpu.V = __builtin_smul_overflow(src, cpu.R[di], &ret);
			cpu.C = false;
			set_nz( ret);
			cpu.R[di] = ret;
		}
	}
}

OP(div_l) {
	uint16_t nw = fetch_w();
	cpu.parse_EA( mode, 4, reg, false);
	uint32_t src = cpu.ea_read();
	if( src == 0 ) {
		throw Exception::DivBy0();
	}
	int dq = ( nw >> 12 ) & 7;
	int dr = nw & 7;
	if( ! ((nw >> 11) & 1) ) {
		if( (nw >> 10) & 1 ) {
			uint64_t v = (uint64_t)cpu.R[dr] << 32 | cpu.R[dq];
			DIV_U( v, src, &cpu.R[dq], &cpu.R[dr]);
		} else {
			uint32_t v = cpu.R[dq];
			uint32_t q = v / src;
			uint32_t r = v % src;
			cpu.C = false;
			set_nz( (int32_t)q);
			cpu.V = false;
			cpu.R[dr] = r;
			cpu.R[dq] = q;
		}
	} else {
		int32_t srcs = src;
		if( (nw >> 10) & 1 ) {
			int64_t v = (int64_t)cpu.R[dr] << 32 | cpu.R[dq];
			DIV_S( v, src, &cpu.R[dq], dq == dr ? nullptr : &cpu.R[dr]);
		} else {
			if( cpu.R[dq] == 0x80000000 && srcs == -1 ) {
				cpu.V = true;
				return;
			}
			int32_t v = cpu.R[dq];
			int32_t q = v / srcs;
			int32_t r = v % srcs;
			cpu.C = false;
			cpu.V = false;
			set_nz( q);
			cpu.R[dr] = r;
			cpu.R[dq] = q;
		}
	}
}


OP(divu_w) {
	cpu.parse_EA( mode, 2, reg, false);
	uint32_t d = cpu.R[dn];
	uint16_t s = cpu.ea_read();
	if( s == 0 ) {
		throw Exception::DivBy0();
	}
	uint32_t q = d / s;
	uint32_t r = d % s;
	if( (cpu.V = q >> 16) )
		return;
	cpu.N = ( q >> 15 ) & 1;
	cpu.Z = ! q;
	cpu.C = false;
	cpu.R[dn] = r << 16 | (uint16_t)q;
}

OP(divs_w) {
	cpu.parse_EA( mode, 2, reg, false);
	int32_t d = cpu.R[dn];
	int16_t s = cpu.ea_read();
	if( s == 0 ) {
		throw Exception::DivBy0();
	}
	int32_t q = d / s;
	int32_t r = d % s;
	int16_t qq = q;
	if( (cpu.V = qq != q) )
		return;
	cpu.N = qq >> 15;
	cpu.Z = ! q;
	cpu.C = false;
	cpu.R[dn] = ((r & 0xffff) << 16) | (uint16_t)qq;
}

OP(sub_b) {
	cpu.parse_EA( mode, 1, reg, false);
	cpu.setD_B(dn, SUB_B( cpu.R[dn], cpu.ea_read()));
}

OP(sub_w) {
	cpu.parse_EA( mode, 2, reg, false);
	cpu.setD_W(dn, SUB_W( cpu.R[dn], cpu.ea_read()));
}

OP(suba_w) {
	cpu.parse_EA( mode, 2, reg, false);
	cpu.R[8+dn] -= (int16_t)cpu.ea_read();
}

OP(sub_l) {
	cpu.parse_EA( mode, 4, reg, false);
	cpu.R[dn] = SUB_L( cpu.R[dn], cpu.ea_read());
}

OP(suba_l) {
	cpu.parse_EA( mode, 4, reg, false);
	cpu.R[8+dn] = SUB_L( cpu.R[8+dn], cpu.ea_read());
}

OP(sub_m_b) {
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( SUB_B( cpu.ea_read(), cpu.R[dn]));
}

OP(sub_m_w) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( SUB_W( cpu.ea_read(), cpu.R[dn]));
}

OP(sub_m_l) {
	cpu.parse_EA( mode, 4, reg, true);
	cpu.ea_write( SUB_L( cpu.ea_read(), cpu.R[dn]));
}

OP(subx_b) {
	bool oldz = cpu.Z;
	cpu.setD_B(dn, SUB_B( cpu.R[dn], cpu.R[reg]+cpu.X));
	cpu.Z &= oldz;
}

OP(subx_w) {
	bool oldz = cpu.Z;
	cpu.setD_W(dn, SUB_W( cpu.R[dn], cpu.R[reg]+cpu.X));
	cpu.Z &= oldz;
}

OP(subx_l) {
	bool oldz = cpu.Z;
	cpu.R[dn] = SUB_L( cpu.R[dn], cpu.R[reg]+cpu.X);
	cpu.Z &= oldz;
}

OP(subx_m_b) {
	int8_t v1 = read_b(--cpu.R[8+dn]);
	int8_t v2 = read_b(--cpu.R[8+reg]);
	write_b(cpu.R[8+dn], SUB_B( v1, v2+cpu.X));
}

OP(subx_m_w) {
	int16_t v1 = read_w(cpu.R[8+dn] -= 2);
	int16_t v2 = read_w(cpu.R[8+reg] -= 2);
	write_w(cpu.R[8+dn], SUB_W( v1, v2+cpu.X));
}

OP(subx_m_l) {
	int32_t v1 = read_l(cpu.R[8+dn] -= 4);
	int32_t v2 = read_l(cpu.R[8+reg] -= 4);
	write_l(cpu.R[8+dn], SUB_L( v1, v2+cpu.X));
}

OP(cmp_b) {
	cpu.parse_EA( mode, 1, reg, false);
	CMP_B( cpu.R[dn], cpu.ea_read());
}

OP(cmp_w) {
	cpu.parse_EA( mode, 2, reg, false);
	CMP_W( cpu.R[dn], cpu.ea_read());
}

OP(cmp_l) {
	cpu.parse_EA( mode, 4, reg, false);
	CMP_L( cpu.R[dn], cpu.ea_read());
}

OP(cmpa_w) {
	cpu.parse_EA( mode, 2, reg, false);
	CMP_W( (int16_t)cpu.R[8+dn], cpu.ea_read());
}

OP(cmpa_l) {
	cpu.parse_EA( mode, 4, reg, false);
	CMP_L( cpu.R[8+dn], cpu.ea_read());
}

OP(cmpm_b) {
	int8_t v1 = read_b(cpu.R[8+dn]);
	int8_t v2 = read_b(cpu.R[8+reg]);
	CMP_B( v1, v2);
	cpu.R[8+dn] += 1;
	cpu.R[8+reg] += 1;
}

OP(cmpm_w) {
	int16_t v1 = read_w(cpu.R[8+dn]);
	int16_t v2 = read_w(cpu.R[8+reg]);
	CMP_W( v1, v2);
	cpu.R[8+dn] += 2;
	cpu.R[8+reg] += 2;
}

OP(cmpm_l) {
	int32_t v1 = read_l(cpu.R[8+dn]);
	int32_t v2 = read_l(cpu.R[8+reg]);
	CMP_L( v1, v2);
	cpu.R[8+dn] += 4;
	cpu.R[8+reg] += 4;
}

OP(mulu_w) {
	cpu.parse_EA( mode, 2, reg, false);
	uint16_t src = cpu.ea_read();
	uint16_t v = cpu.R[dn];
	uint32_t x = (uint32_t)src * v;
	cpu.R[dn] = x;
	cpu.C = cpu.V = false;
	set_nz( (int32_t)x);
}

OP(muls_w) {
	cpu.parse_EA( mode, 2, reg, false);
	int16_t src = cpu.ea_read();
	int16_t v = cpu.R[dn];
	int32_t x = (int32_t)src * v;
	cpu.R[dn] = x;
	cpu.C = cpu.V = false;
	set_nz( x);
}


OP(add_b) {
	cpu.parse_EA( mode, 1, reg, false);
	cpu.setD_B(dn, ADD_B( cpu.R[dn], cpu.ea_read()));
}

OP(add_w) {
	cpu.parse_EA( mode, 2, reg, false);
	cpu.setD_W(dn, ADD_W( cpu.R[dn], cpu.ea_read()));
}

OP(adda_w) {
	cpu.parse_EA( mode, 2, reg, false);
	cpu.R[8+dn] += (int16_t)cpu.ea_read();
}

OP(add_l) {
	cpu.parse_EA( mode, 4, reg, false);
	cpu.R[dn] = ADD_L( cpu.R[dn], cpu.ea_read());
}

OP(adda_l) {
	cpu.parse_EA( mode, 4, reg, false);
	cpu.R[8+dn] += (uint32_t)cpu.ea_read();
}

OP(add_m_b) {
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( ADD_B( cpu.R[dn], cpu.ea_read()));
}

OP(add_m_w) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( ADD_W( cpu.R[dn], cpu.ea_read()));
}

OP(add_m_l) {
	cpu.parse_EA( mode, 4, reg, true);
	cpu.ea_write( ADD_L( cpu.R[dn], cpu.ea_read()));
}

OP(addx_b) {
	bool oz = cpu.Z;
	cpu.setD_B(dn, ADD_B( cpu.R[dn], cpu.R[reg]+cpu.X));
	cpu.Z = cpu.Z && oz;
}

OP(addx_w) {
	bool oz = cpu.Z;
	cpu.setD_W(dn, ADD_W( cpu.R[dn], cpu.R[reg]+cpu.X));
	cpu.Z = cpu.Z && oz;
}

OP(addx_l) {
	bool oz = cpu.Z;
	cpu.R[dn] = ADD_L( cpu.R[dn], cpu.R[reg]+cpu.X);
	cpu.Z = cpu.Z && oz;
}

OP(addx_m_b) {
	int8_t v1 = read_b(--cpu.R[8+dn]);
	int8_t v2 = read_b(--cpu.R[8+reg]);
	write_b(cpu.R[8+dn], ADD_B( v1, v2+cpu.X));
}

OP(addx_m_w) {
	int16_t v1 = read_w(cpu.R[8+dn] -= 2);
	int16_t v2 = read_w(cpu.R[8+reg] -= 2);
	write_w(cpu.R[8+dn], ADD_W( v1, v2+cpu.X));
}

OP(addx_m_l) {
	int32_t v1 = read_l(cpu.R[8+dn] -= 4);
	int32_t v2 = read_l(cpu.R[8+reg] -= 4);
	write_l(cpu.R[8+dn], ADD_L( v1, v2+cpu.X));
}
