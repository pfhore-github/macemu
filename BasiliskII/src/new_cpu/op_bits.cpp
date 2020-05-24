#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "inst_cmn.hpp"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define OP(name_) extern "C" void op_##name_(uint16_t op, int dn, int mode, int reg)
#include "op.h"
#include "intel.h"


OP(ori_ccr) {
	uint8_t imm = fetch_w();
	set_cr( get_cr() | imm );
}

OP(ori_b) {
	uint8_t imm = fetch_w();
	cpu.parse_EA ( mode, 1, reg, true);
	cpu.ea_write(OR_B( cpu.ea_read(), imm));
}
OP(ori_sr) {
	uint16_t imm = fetch_w();
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	set_sr( get_sr() | imm );
	test_trace_branch();
}
OP(ori_w) {
	uint16_t imm = fetch_w();
	cpu.parse_EA ( mode, 2, reg, true);
	cpu.ea_write(OR_W(cpu.ea_read(), imm));
}

OP(ori_l) {
	uint32_t imm = fetch_l();
	cpu.parse_EA ( mode, 4, reg, true);
	cpu.ea_write(OR_L(cpu.ea_read(), imm));
}

OP(or_b) {
	cpu.parse_EA( mode, 1, reg, false);
	cpu.setD_B(dn, OR_B(cpu.R[dn], cpu.ea_read()));
}

OP(or_w) {
	cpu.parse_EA( mode, 2, reg, false);
	cpu.setD_W(dn, OR_W(cpu.R[dn], cpu.ea_read()));
}

OP(or_l) {
	cpu.parse_EA( mode, 4, reg, false);
	cpu.R[dn] = OR_L(cpu.R[dn], cpu.ea_read());
}

OP(or_m_b) {
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( OR_B( cpu.R[dn], cpu.ea_read()));
}

OP(or_m_w) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( OR_W( cpu.R[dn], cpu.ea_read()));
}

OP(or_m_l) {
	cpu.parse_EA( mode, 4, reg, true);
	cpu.ea_write( OR_L( cpu.R[dn], cpu.ea_read()));
}



OP(andi_ccr) {
	uint8_t imm = fetch_w();
	set_cr(  get_cr() & imm );
}

OP(andi_b) {
	uint8_t imm = fetch_w();
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write(AND_B( cpu.ea_read(), imm));
}

OP(andi_sr) {
	uint16_t imm = fetch_w();
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	set_sr( get_sr() & imm );
	test_trace_branch();
}

OP(andi_w) {
	uint16_t imm = fetch_w();
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write(AND_W( cpu.ea_read(), imm));
}


OP(andi_l) {
	uint32_t imm = fetch_l();
	cpu.parse_EA( mode, 4, reg, true);
	cpu.ea_write(AND_L( cpu.ea_read(), imm));
}

OP(and_b) {
	cpu.parse_EA( mode, 1, reg, false);
	cpu.setD_B(dn, AND_B( cpu.R[dn], cpu.ea_read()));
}

OP(and_w) {
	cpu.parse_EA( mode, 2, reg, false);
	cpu.setD_W(dn, AND_W( cpu.R[dn], cpu.ea_read()));
}

OP(and_l) {
	cpu.parse_EA( mode, 4, reg, false);
	cpu.R[dn] = AND_L( cpu.R[dn], cpu.ea_read());
}

OP(and_m_b) {
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( AND_B( cpu.R[dn], cpu.ea_read()));
}

OP(and_m_w) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( AND_W( cpu.R[dn], cpu.ea_read()));
}

OP(and_m_l) {
	cpu.parse_EA( mode, 4, reg, true);
	cpu.ea_write( AND_L( cpu.R[dn], cpu.ea_read()));
}


OP(eori_b) {
	uint8_t imm = fetch_w();
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( XOR_B( cpu.ea_read(), imm));
}

OP(eori_ccr) {
	uint8_t imm = fetch_w();
	set_cr(  get_cr() ^ imm );
}

OP( eori_w ) {
	uint16_t imm = fetch_w();
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( XOR_W( cpu.ea_read(), imm));
}

OP( eori_sr) {
	uint16_t imm = fetch_w();
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	set_sr(  get_sr() ^ imm );
	test_trace_branch();
}
OP(eori_l) {
	uint32_t imm = fetch_l();
	cpu.parse_EA( mode, 4, reg, true);
	cpu.ea_write( XOR_L( cpu.ea_read(), imm));
}

OP(eor_b) {
	cpu.parse_EA( mode, 1, reg, false);
	cpu.ea_write(XOR_B( cpu.R[dn], cpu.ea_read()));
}

OP(eor_w) {
	cpu.parse_EA( mode, 2, reg, false);
	cpu.ea_write(XOR_W( cpu.R[dn], cpu.ea_read()));
}

OP(eor_l) {
	cpu.parse_EA( mode, 4, reg, false);
	cpu.ea_write(XOR_L( cpu.R[dn], cpu.ea_read()));
}


OP(not_b) {
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( XOR_B( cpu.ea_read(), 0xff) );
}

OP(not_w) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( XOR_W( cpu.ea_read(), 0xffff) );
}

OP(not_l) {
	cpu.parse_EA( mode, 4, reg, true);
	cpu.ea_write( XOR_L( cpu.ea_read(), 0xffffffff) );
}

OP(btst_d_l) {
	uint32_t v = cpu.R[reg];
	cpu.Z = BTST(v, cpu.R[dn]);
}

OP(btst_d_b) {
	cpu.parse_EA( mode, 1, reg, false);
	cpu.Z = BTST(cpu.ea_read(), cpu.R[dn]);
}

OP(btst_i_l) {
	uint8_t bn = fetch_w();
	cpu.Z = BTST(cpu.R[reg], bn);
}

OP(btst_i_b) {
	uint8_t bn = fetch_w();
	cpu.parse_EA( mode, 1, reg, false);
	cpu.Z = BTST(cpu.ea_read(), bn);
}


OP(bchg_d_l) {
	uint32_t v = cpu.R[reg];
	cpu.R[reg] = BCHG(v, cpu.R[dn], &cpu.Z);
}

OP(bchg_d_b) {
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write(BCHG(cpu.ea_read(), cpu.R[dn], &cpu.Z));
}

OP(bchg_i_l) {
	uint8_t bn = fetch_w();
	cpu.R[reg] = BCHG(cpu.R[reg], bn, &cpu.Z);
}

OP(bchg_i_b) {
	uint8_t bn = fetch_w();
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( BCHG(cpu.ea_read(), bn, &cpu.Z));
}

OP(bclr_d_l) {
	uint32_t v = cpu.R[reg];
	cpu.R[reg] = BCLR(v, cpu.R[dn], &cpu.Z);
}

OP(bclr_d_b) {
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write(BCLR(cpu.ea_read(), cpu.R[dn], &cpu.Z));
}

OP(bclr_i_l) {
	uint8_t bn = fetch_w();
	cpu.R[reg] = BCLR(cpu.R[reg], bn, &cpu.Z);
}

OP(bclr_i_b) {
	uint8_t bn = fetch_w();
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( BCLR(cpu.ea_read(), bn, &cpu.Z) );
}

OP(bset_d_l) {
	uint32_t v = cpu.R[reg];
	cpu.R[reg] = BSET(v, cpu.R[dn], &cpu.Z);
}

OP(bset_d_b) {
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write(BSET(cpu.ea_read(), cpu.R[dn], &cpu.Z));
}

OP(bset_i_l) {
	uint8_t bn = fetch_w();
	cpu.R[reg] = BSET(cpu.R[reg], bn, &cpu.Z);
}

OP(bset_i_b) {
	uint8_t bn = fetch_w();
	cpu.parse_EA( mode, 1, reg, true);
	cpu.ea_write( BSET(cpu.ea_read(), bn, &cpu.Z));
}

OP(ext_l) {
	int16_t w = cpu.R[reg];
	int32_t ww = (int32_t)w;
	cpu.R[reg] = ww;
	TEST_L( ww);
	cpu.V = cpu.C = false;
}

OP(ext_w) {
	int8_t w = cpu.R[reg];
	int16_t ww = w;
	cpu.setD_W( reg, ww);
	TEST_W( ww);
	cpu.V = cpu.C = false;
}

OP(extb_l) {
	int8_t v = cpu.R[reg];
	int32_t w = v;
	TEST_L( w);
	cpu.V = cpu.C = false;
	cpu.R[reg] = w;
}

OP(tst_b) {
	cpu.parse_EA( mode, 1, reg, false);
	TEST_B( cpu.ea_read());
	cpu.V = cpu.C = false;
}

OP(tst_w) {
	cpu.parse_EA( mode, 2, reg, false);
	TEST_W( cpu.ea_read());
	cpu.V = cpu.C = false;
}

OP(tst_l) {
	cpu.parse_EA( mode, 4, reg, false);
	TEST_L( cpu.ea_read());
	cpu.V = cpu.C = false;
}

OP(tas) {
	cpu.parse_EA( mode, 1, reg, true);
	uint8_t v = cpu.ea_read();
	TEST_B( v);
	cpu.V = cpu.C = false;
	cpu.ea_write(v |0x80 );
}

OP(swap) {
	uint32_t v = cpu.R[reg];
	v = (v>>16)|(v<<16);
	cpu.N = v >> 31;
	cpu.Z = v == 0;
	cpu.V = cpu.C = false;
	cpu.R[reg] = v;
}

OP(asr_b_i) {
	cpu.setD_B(reg, ASR_B( cpu.R[reg], dn ? dn : 8));
}

OP(lsr_b_i) {
	cpu.setD_B(reg, LSR_B( cpu.R[reg], dn ? dn : 8));
}
OP(roxr_b_i) {
	cpu.setD_B(reg, ROXR_B( cpu.R[reg], dn ? dn : 8));
}
OP(ror_b_i) {
	cpu.setD_B(reg, ROR_B( cpu.R[reg], dn ? dn : 8));
}

OP(asr_b_d) {
	cpu.setD_B(reg, ASR_B( cpu.R[reg], cpu.R[dn] & 63));
}

OP(lsr_b_d) {
	cpu.setD_B(reg, LSR_B( cpu.R[reg], cpu.R[dn] & 63));
}
OP(roxr_b_d) {
	cpu.setD_B(reg, ROXR_B( cpu.R[reg], cpu.R[dn] & 63));
}
OP(ror_b_d) {
	cpu.setD_B(reg, ROR_B( cpu.R[reg], cpu.R[dn] & 63));
}


OP(asr_w_i) {
	cpu.setD_W(reg, ASR_W( cpu.R[reg], dn ? dn : 8));
}

OP(lsr_w_i) {
	cpu.setD_W(reg, LSR_W( cpu.R[reg], dn ? dn : 8));
}
OP(roxr_w_i) {
	cpu.setD_W(reg, ROXR_W( cpu.R[reg], dn ? dn : 8));
}
OP(ror_w_i) {
	cpu.setD_W(reg, ROR_W( cpu.R[reg], dn ? dn : 8));
}

OP(asr_w_d) {
	cpu.setD_W(reg, ASR_W( cpu.R[reg], cpu.R[dn] & 63));
}

OP(lsr_w_d) {
	cpu.setD_W(reg, LSR_W( cpu.R[reg], cpu.R[dn] & 63));
}
OP(roxr_w_d) {
	cpu.setD_W(reg, ROXR_W( cpu.R[reg], cpu.R[dn] & 63));
}
OP(ror_w_d) {
	cpu.setD_W(reg, ROR_W( cpu.R[reg], cpu.R[dn] & 63));
}

OP(asr_l_i) {
	cpu.R[reg] = ASR_L( cpu.R[reg], dn ? dn : 8);
}

OP(lsr_l_i) {
	cpu.R[reg] = LSR_L( cpu.R[reg], dn ? dn : 8);
}
OP(roxr_l_i) {
	cpu.R[reg] = ROXR_L( cpu.R[reg], dn ? dn : 8);
}
OP(ror_l_i) {
	cpu.R[reg] = ROR_L( cpu.R[reg], dn ? dn : 8);
}

OP(asr_l_d) {
	cpu.R[reg] = ASR_L( cpu.R[reg], cpu.R[dn] & 63);
}

OP(lsr_l_d) {
	cpu.R[reg] = LSR_L( cpu.R[reg], cpu.R[dn] & 63);
}
OP(roxr_l_d) {
	cpu.R[reg] = ROXR_L( cpu.R[reg], cpu.R[dn] & 63);
}
OP(ror_l_d) {
	cpu.R[reg] = ROR_L( cpu.R[reg], cpu.R[dn] & 63);
}

OP(asr_ea) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( ASR_W( cpu.ea_read(), 1));
}

OP(lsr_ea) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( LSR_W( cpu.ea_read(), 1));
}

OP(roxr_ea) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( ROXR_W( cpu.ea_read(), 1));
}

OP(ror_ea) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( ROR_W( cpu.ea_read(), 1));
}

void bf_op(uint16_t op, int* offset, int* width) {
	*offset = (op >> 6) & 0x1f; 
	*width = op & 0x1f;
	if( (op >> 11) & 1 )
		*offset = cpu.R[*offset];
	if( (op>>5) & 1 ) {
		*width = (cpu.R[*width]) & 31;
	}	
	if( *width == 0 )
		*width = 32;
}
static uint32_t mask_of(int n) {
	return n  ? ~0u >> (32-n) : 0;
}
template<class T>
static T get_bits(T v, int offset, int sz) {
	return v >> offset & mask_of(sz);
}
template<class T>
static T set_bits(T v, int offset, int sz, T x) {
	uint32_t mk = mask_of(sz);
	return (v &~ (mk << offset )) | ((x & mk) << offset);
}
uint32_t get_bf_m_u(uint32_t addr, int offset, int sz) {
	addr += offset>>3;
	offset &= 7;
	int bits = offset + sz;
	uint64_t v = 0;
	for(int i = 0; i <= (bits-1) / 8; ++i ) {
		uint8_t vv = read_b(addr+i);
		if( i == 0 ) {
			v = vv & mask_of(8-offset);
		} else {
			v = (v << 8) | vv;
		}
	}
	return v >> ((8-(bits&7))&7);
}

int32_t get_bf_m_s(uint32_t addr, int offset, int sz) {
	int32_t v = get_bf_m_u( addr, offset, sz);
	return (int32_t)(v << (32-sz)) >> (32-sz);
}

void set_bf_m(uint32_t addr, int offset, int sz, uint32_t v) {
	addr += offset>>3;
	offset &= 7;
	int ln = offset + sz;	
	int lr = 8 - offset;
	if( ln <= 8 ) {
		uint8_t q = read_b(addr);
		write_b(addr, set_bits<uint8_t>(q, 8-ln, sz, v));
	} else {
		int bs = (ln-1)/8;
		for(int i = bs; i >= 0; --i ) {
			uint8_t q = read_b(addr+i);
			if( i == bs ) {
				q = set_bits<uint8_t>(q, 8-(ln&7), ln & 7, v);
				v >>= (ln & 7);
			} else if( i == 0 ) {				
				q = set_bits<uint8_t>(q, 0, lr, v );
			} else {
				q = v & 0xff;
				v >>= 8;
			}
			write_b(addr+i, q);
		}
	}	
}

uint32_t get_bf_d_u(uint32_t v, int offset, int sz) {
	if( offset < 0 ) {
		offset += 32;
	}
	int ln = offset + sz;
	if( ln <= 32 ) {
		return (v >> (32-ln)) & mask_of(sz);
	} else {
		return ((v >> (64-ln)) & mask_of(ln-32)) |
			(( v & mask_of(32-offset) ) << (ln-32));
	}
}

int32_t get_bf_d_s(uint32_t v, int offset, int sz) {
	int32_t v2 = get_bf_d_u(v, offset, sz);
	return v2 << (32-sz) >> (32-sz);
}

uint32_t set_bf_d(uint32_t old, int offset, int sz, uint32_t v) {
	if( offset < 0 ) {
		offset += 32;
	}
	int ln = offset + sz;
	v &= mask_of(sz);
	if( ln < 32 ) {
		return (old &~ (mask_of(sz) << (32-ln)))
			| (v << (32-ln));
	} else {
		old = ROL(old, ln-32);
		old = (old &~ mask_of(sz)) | v;
		return ROR(old, ln-32);
	}
}

OP(bftst_m) {
	uint16_t op2 = fetch_w();
	int off, sz;
	bf_op( op2, &off, &sz);
	cpu.parse_EA( mode, 4, reg, false);
	uint32_t v = get_bf_m_u( cpu.ea_v, off, sz);

	cpu.V = cpu.C = false;
	cpu.Z = ! v;
	cpu.N = (v >> (sz-1)) & 1;
}

OP(bftst_d) {
	uint32_t v = cpu.R[ reg ];
	uint16_t op2 = fetch_w();
	int off, wt;
	bf_op( op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off&31, wt);
	cpu.V = cpu.C = false;
	cpu.Z = ! v2;
	cpu.N = (v2 >> (wt-1)) & 1;
}

OP(bfchg_m) {
	uint16_t op2 = fetch_w();
	int off, wt;
	bf_op( op2, &off, &wt);
	cpu.parse_EA( mode, 4, reg, false);
	uint32_t v = get_bf_m_u( cpu.ea_v, off, wt) ;
	set_bf_m( cpu.ea_v, off, wt, v ^ mask_of(wt));
	cpu.V = cpu.C = false;
	cpu.Z = ! v;
	cpu.N = (v >> (wt-1)) & 1;	
}

OP(bfchg_d) {
	uint32_t v = cpu.R[ reg ];
	uint16_t op2 = fetch_w();
	int off, wt;
	bf_op( op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off, wt) ;
	cpu.R[reg] = set_bf_d(v, off, wt, v2 ^ mask_of(wt));
	cpu.V = cpu.C = false;
	cpu.Z = ! v2;
	cpu.N = (v2 >> (wt-1)) & 1;
}

OP(bfclr_m) {
	uint16_t op2 = fetch_w();
	int off, wt;
	bf_op( op2, &off, &wt);
	cpu.parse_EA( mode, 4, reg, false);
	uint32_t v = get_bf_m_u( cpu.ea_v, off, wt) ;
	set_bf_m( cpu.ea_v, off, wt, 0);
	cpu.V = cpu.C = false;
	cpu.Z = ! v;
	cpu.N = (v >> (wt-1)) & 1;	
}

OP(bfclr_d) {
	uint32_t v = cpu.R[ reg ];
	uint16_t op2 = fetch_w();
	int off, wt;
	bf_op( op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off, wt) ;
	cpu.R[reg] = set_bf_d(v, off, wt, 0);
	cpu.V = cpu.C = false;
	cpu.Z = ! v2;
	cpu.N = (v2 >> (wt-1)) & 1;
}

OP(bfset_m) {
	uint16_t op2 = fetch_w();
	int off, wt;
	bf_op( op2, &off, &wt);
	cpu.parse_EA( mode, 4, reg, false);
	uint32_t v = get_bf_m_u( cpu.ea_v, off, wt) ;
	set_bf_m( cpu.ea_v, off, wt, mask_of(wt));
	cpu.V = cpu.C = false;
	cpu.Z = ! v;
	cpu.N = (v >> (wt-1)) & 1;	
}

OP(bfset_d) {
	uint32_t v = cpu.R[ reg ];
	uint16_t op2 = fetch_w();
	int off, wt;
	bf_op( op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off, wt) ;
	cpu.R[reg] = set_bf_d(v, off, wt, mask_of(wt));
	cpu.V = cpu.C = false;
	cpu.Z = ! v2;
	cpu.N = (v2 >> (wt-1)) & 1;
}



OP(asl_b_i) {
	cpu.setD_B(reg, ASL_B( cpu.R[reg], dn ? dn : 8));
}

OP(lsl_b_i) {
	cpu.setD_B(reg, LSL_B( cpu.R[reg], dn ? dn : 8));
}
OP(roxl_b_i) {
	cpu.setD_B(reg, ROXL_B( cpu.R[reg], dn ? dn : 8));
}
OP(rol_b_i) {
	cpu.setD_B(reg, ROL_B( cpu.R[reg], dn ? dn : 8));
}

OP(asl_b_d) {
	cpu.setD_B(reg, ASL_B( cpu.R[reg], cpu.R[dn] & 63));
}

OP(lsl_b_d) {
	cpu.setD_B(reg, LSL_B( cpu.R[reg], cpu.R[dn] & 63));
}
OP(roxl_b_d) {
	cpu.setD_B(reg, ROXL_B( cpu.R[reg], cpu.R[dn] & 63));
}
OP(rol_b_d) {
	cpu.setD_B(reg, ROL_B( cpu.R[reg], cpu.R[dn] & 63));
}


OP(asl_w_i) {
	cpu.setD_W(reg, ASL_W( cpu.R[reg], dn ? dn : 8));
}

OP(lsl_w_i) {
	cpu.setD_W(reg, LSL_W( cpu.R[reg], dn ? dn : 8));
}
OP(roxl_w_i) {
	cpu.setD_W(reg, ROXL_W( cpu.R[reg], dn ? dn : 8));
}
OP(rol_w_i) {
	cpu.setD_W(reg, ROL_W( cpu.R[reg], dn ? dn : 8));
}

OP(asl_w_d) {
	cpu.setD_W(reg, ASL_W( cpu.R[reg], cpu.R[dn] & 63));
}

OP(lsl_w_d) {
	cpu.setD_W(reg, LSL_W( cpu.R[reg], cpu.R[dn] & 63));
}
OP(roxl_w_d) {
	cpu.setD_W(reg, ROXL_W( cpu.R[reg], cpu.R[dn] & 63));
}
OP(rol_w_d) {
	cpu.setD_W(reg, ROL_W( cpu.R[reg], cpu.R[dn] & 63));
}

OP(asl_l_i) {
	cpu.R[reg] = ASL_L( cpu.R[reg], dn ? dn : 8);
}

OP(lsl_l_i) {
	cpu.R[reg] = LSL_L( cpu.R[reg], dn ? dn : 8);
}
OP(roxl_l_i) {
	cpu.R[reg] = ROXL_L( cpu.R[reg], dn ? dn : 8);
}
OP(rol_l_i) {
	cpu.R[reg] = ROL_L( cpu.R[reg], dn ? dn : 8);
}

OP(asl_l_d) {
	cpu.R[reg] = ASL_L( cpu.R[reg], cpu.R[dn] & 63);
}

OP(lsl_l_d) {
	cpu.R[reg] = LSL_L( cpu.R[reg], cpu.R[dn] & 63);
}
OP(roxl_l_d) {
	cpu.R[reg] = ROXL_L( cpu.R[reg], cpu.R[dn] & 63);
}
OP(rol_l_d) {
	cpu.R[reg] = ROL_L( cpu.R[reg], cpu.R[dn] & 63);
}

OP(asl_ea) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( ASL_W( cpu.ea_read(), 1));
}

OP(lsl_ea) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( LSL_W( cpu.ea_read(), 1));
}

OP(roxl_ea) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( ROXL_W( cpu.ea_read(), 1));
}

OP(rol_ea) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( ROL_W( cpu.ea_read(), 1));
}


OP(bfextu_m) {
	uint16_t op2 = fetch_w();
	int off, sz;
	int dn2 = (op2 >> 12) & 7;
	bf_op( op2, &off, &sz);
	cpu.parse_EA( mode, 4, reg, false);
	uint32_t v = get_bf_m_u( cpu.ea_v, off, sz);
	cpu.R[dn2] = v;
	cpu.V = cpu.C = false;
	cpu.Z = ! v;
	cpu.N = (v >> (sz-1)) & 1;
}

OP(bfextu_d) {
	uint32_t v = cpu.R[ reg ];
	uint16_t op2 = fetch_w();
	int dn2 = (op2 >> 12) & 7;
	int off, wt;
	bf_op( op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off&31, wt);
	cpu.R[dn2] = v2;
	cpu.V = cpu.C = false;
	cpu.Z = ! v2;
	cpu.N = (v2 >> (wt-1)) & 1;
}

OP(bfexts_m) {
	uint16_t op2 = fetch_w();
	int off, sz;
	int dn2 = (op2 >> 12) & 7;
	bf_op( op2, &off, &sz);
	cpu.parse_EA( mode, 4, reg, false);
	int32_t v = get_bf_m_s( cpu.ea_v, off, sz);
	cpu.R[dn2] = v;
	cpu.V = cpu.C = false;
	cpu.Z = ! v;
	cpu.N = (v >> (sz-1)) & 1;
}

OP(bfexts_d) {
	uint32_t v = cpu.R[ reg ];
	uint16_t op2 = fetch_w();
	int dn2 = (op2 >> 12) & 7;
	int off, wt;
	bf_op( op2, &off, &wt);
	int32_t v2 = get_bf_d_s(v, off&31, wt);
	cpu.R[dn2] = v2;
	cpu.V = cpu.C = false;
	cpu.Z = ! v2;
	cpu.N = (v2 >> (wt-1)) & 1;
}
static int bfffo(uint32_t v, int size) {
	int k = size;
	if( v ) {
		int clz = __builtin_clz(v);
		k -= (32-clz);
	}
	cpu.V = cpu.C = false;
	cpu.Z = ! v;
	cpu.N = (v >> (size-1)) & 1;
	return k;
}
OP(bfffo_m) {
	uint16_t op2 = fetch_w();
	int off, sz;
	int dn2 = (op2 >> 12) & 7;
	bf_op( op2, &off, &sz);
	cpu.parse_EA( mode, 4, reg, false);
	uint32_t v = get_bf_m_u( cpu.ea_v, off, sz);
	cpu.R[dn2] = bfffo(v, sz) + off;
}

OP(bfffo_d) {
	uint32_t v = cpu.R[ reg ];
	uint16_t op2 = fetch_w();
	int dn2 = (op2 >> 12) & 7;
	int off, wt;
	bf_op( op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off&31, wt);
	cpu.R[dn2] = bfffo(v2, wt) + off;
}

OP(bfins_m) {
	uint16_t op2 = fetch_w();
	int off, sz;
	int dn2 = (op2 >> 12) & 7;
	bf_op( op2, &off, &sz);
	cpu.parse_EA( mode, 4, reg, false);
	uint32_t v2 = cpu.R[dn2] & mask_of(sz);
	cpu.V = cpu.C = false;
	cpu.Z = ! v2;
	cpu.N = (v2 >> (sz-1)) & 1;
	set_bf_m( cpu.ea_v, off, sz, v2);
}

OP(bfins_d) {
	uint32_t v = cpu.R[ reg ];
	uint16_t op2 = fetch_w();
	int dn2 = (op2 >> 12) & 7;
	int off, wt;
	bf_op( op2, &off, &wt);
	uint32_t v2 = cpu.R[dn2] & mask_of(wt);
	cpu.V = cpu.C = false;
	cpu.Z = ! v2;
	cpu.N = (v2 >> (wt-1)) & 1;
	v = set_bf_d(v, off&31, wt, v2);
	cpu.R[reg] = v;
}
