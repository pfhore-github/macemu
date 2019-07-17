#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#define OP(name_) extern "C" void op_##name_(CPU* cpu, uint16_t op __attribute__((unused)), int dn __attribute__((unused)), int mode __attribute__((unused)), int reg __attribute__((unused)))
#include "op.h"
#include "intel.h"


OP(ori_ccr) {
	uint8_t imm = cpu->fetch_w();
	set_cr( cpu, get_cr(cpu) | imm );
}

OP(ori_b) {
	uint8_t imm = cpu->fetch_w();
	EA ea (cpu, mode, 1, reg, true);
	ea.write(OR_B(cpu, ea.read(), imm));
}
OP(ori_sr) {
	uint16_t imm = cpu->fetch_w();
	if( ! cpu->S ) {
		ILLEGAL(cpu);
	}
	set_sr( cpu, get_sr(cpu) | imm );
	test_trace_branch(cpu);
}
OP(ori_w) {
	uint16_t imm = cpu->fetch_w();
	EA ea (cpu, mode, 2, reg, true);
	ea.write(OR_W(cpu, ea.read(), imm));
}

OP(ori_l) {
	uint32_t imm = cpu->fetch_l();
	EA ea (cpu, mode, 4, reg, true);
	ea.write(OR_L(cpu, ea.read(), imm));
}

OP(or_b) {
	EA ea(cpu, mode, 1, reg, false);
	cpu->setD_B(dn, OR_B(cpu, cpu->D[dn], ea.read()));
}

OP(or_w) {
	EA ea(cpu, mode, 2, reg, false);
	cpu->setD_W(dn, OR_W(cpu, cpu->D[dn], ea.read()));
}

OP(or_l) {
	EA ea(cpu, mode, 4, reg, false);
	cpu->D[dn] = OR_L(cpu, cpu->D[dn], ea.read());
}

OP(or_m_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write( OR_B(cpu, cpu->D[dn], ea.read()));
}

OP(or_m_w) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( OR_W(cpu, cpu->D[dn], ea.read()));
}

OP(or_m_l) {
	EA ea(cpu, mode, 4, reg, true);
	ea.write( OR_L(cpu, cpu->D[dn], ea.read()));
}



OP(andi_ccr) {
	uint8_t imm = cpu->fetch_w();
	set_cr( cpu, get_cr(cpu) & imm );
}

OP(andi_b) {
	uint8_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, true);
	ea.write(AND_B(cpu, ea.read(), imm));
}

OP(andi_sr) {
	uint16_t imm = cpu->fetch_w();
	if( ! cpu->S ) {
		PRIV_VIOLATION(cpu);
	}
	set_sr( cpu, get_sr(cpu) & imm );
	if( cpu->T == 1 ) {
		TraceEx e;
		e.run(cpu, cpu->NPC);
	}
}

OP(andi_w) {
	uint16_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 2, reg, true);
	ea.write(AND_W(cpu, ea.read(), imm));
}


OP(andi_l) {
	uint32_t imm = cpu->fetch_l();
	EA ea(cpu, mode, 4, reg, true);
	ea.write(AND_L(cpu, ea.read(), imm));
}

OP(and_b) {
	EA ea(cpu, mode, 1, reg, false);
	cpu->setD_B(dn, AND_B(cpu, cpu->D[dn], ea.read()));
}

OP(and_w) {
	EA ea(cpu, mode, 2, reg, false);
	cpu->setD_W(dn, AND_W(cpu, cpu->D[dn], ea.read()));
}

OP(and_l) {
	EA ea(cpu, mode, 4, reg, false);
	cpu->D[dn] = AND_L(cpu, cpu->D[dn], ea.read());
}

OP(and_m_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write( AND_B(cpu, cpu->D[dn], ea.read()));
}

OP(and_m_w) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( AND_W(cpu, cpu->D[dn], ea.read()));
}

OP(and_m_l) {
	EA ea(cpu, mode, 4, reg, true);
	ea.write( AND_L(cpu, cpu->D[dn], ea.read()));
}


OP(eori_b) {
	uint8_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, true);
	ea.write( XOR_B(cpu, ea.read(), imm));
}

OP(eori_ccr) {
	uint8_t imm = cpu->fetch_w();
	set_cr( cpu, get_cr(cpu) ^ imm );
}

OP( eori_w ) {
	uint16_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 2, reg, true);
	ea.write( XOR_W(cpu, ea.read(), imm));
}

OP( eori_sr) {
	uint16_t imm = cpu->fetch_w();
	if( ! cpu->S ) {
		PRIV_VIOLATION(cpu);
	}
	set_sr( cpu, get_sr(cpu) ^ imm );
	test_trace_branch(cpu);
}
OP(eori_l) {
	uint32_t imm = cpu->fetch_l();
	EA ea(cpu, mode, 4, reg, true);
	ea.write( XOR_L(cpu, ea.read(), imm));
}

OP(eor_b) {
	EA ea(cpu, mode, 1, reg, false);
	cpu->setD_B(dn, XOR_B(cpu, cpu->D[dn], ea.read()));
}

OP(eor_w) {
	EA ea(cpu, mode, 2, reg, false);
	cpu->setD_W(dn, XOR_W(cpu, cpu->D[dn], ea.read()));
}

OP(eor_l) {
	EA ea(cpu, mode, 4, reg, false);
	cpu->D[dn] = XOR_L(cpu, cpu->D[dn], ea.read());
}

OP(eor_m_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write( XOR_B(cpu, cpu->D[dn], ea.read()));
}

OP(eor_m_w) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( XOR_W(cpu, cpu->D[dn], ea.read()));
}

OP(eor_m_l) {
	EA ea(cpu, mode, 4, reg, true);
	ea.write( XOR_L(cpu, cpu->D[dn], ea.read()));
}

OP(not_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write( XOR_B(cpu, ea.read(), 0xff) );
}

OP(not_w) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( XOR_W(cpu, ea.read(), 0xffff) );
}

OP(not_l) {
	EA ea(cpu, mode, 4, reg, true);
	ea.write( XOR_L(cpu, ea.read(), 0xffffffff) );
}

OP(btst_d_l) {
	uint32_t v = cpu->D[reg];
	cpu->Z = BTST(v, cpu->D[dn]);
}

OP(btst_d_b) {
	EA ea(cpu, mode, 1, reg, false);
	cpu->Z = BTST(ea.read(), cpu->D[dn]);
}

OP(btst_i_l) {
	uint8_t bn = cpu->fetch_w();
	cpu->Z = BTST(cpu->D[reg], bn);
}

OP(btst_i_b) {
	uint8_t bn = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, false);
	cpu->Z = BTST(ea.read(), bn);
}


OP(bchg_d_l) {
	uint32_t v = cpu->D[reg];
	cpu->D[reg] = BCHG(v, cpu->D[dn], &cpu->Z);
}

OP(bchg_d_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write(BCHG(ea.read(), cpu->D[dn], &cpu->Z));
}

OP(bchg_i_l) {
	uint8_t bn = cpu->fetch_w();
	cpu->D[reg] = BCHG(cpu->D[reg], bn, &cpu->Z);
}

OP(bchg_i_b) {
	uint8_t bn = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, true);
	ea.write( BCHG(ea.read(), bn, &cpu->Z));
}

OP(bclr_d_l) {
	uint32_t v = cpu->D[reg];
	cpu->D[reg] = BCLR(v, cpu->D[dn], &cpu->Z);
}

OP(bclr_d_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write(BCLR(ea.read(), cpu->D[dn], &cpu->Z));
}

OP(bclr_i_l) {
	uint8_t bn = cpu->fetch_w();
	cpu->D[reg] = BCLR(cpu->D[reg], bn, &cpu->Z);
}

OP(bclr_i_b) {
	uint8_t bn = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, true);
	ea.write( BCLR(ea.read(), bn, &cpu->Z) );
}

OP(bset_d_l) {
	uint32_t v = cpu->D[reg];
	cpu->D[reg] = BSET(v, cpu->D[dn], &cpu->Z);
}

OP(bset_d_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write(BSET(ea.read(), cpu->D[dn], &cpu->Z));
}

OP(bset_i_l) {
	uint8_t bn = cpu->fetch_w();
	cpu->D[reg] = BSET(cpu->D[reg], bn, &cpu->Z);
}

OP(bset_i_b) {
	uint8_t bn = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, true);
	ea.write( BSET(ea.read(), bn, &cpu->Z));
}

OP(ext_l) {
	int16_t w = cpu->D[reg];
	int32_t ww = (int32_t)w;
	cpu->D[reg] = ww;
	TEST_L(cpu, ww);
	cpu->V = cpu->C = false;
}

OP(ext_w) {
	int8_t w = cpu->D[reg];
	int16_t ww = w;
	cpu->setD_W( reg, ww);
	TEST_W(cpu, ww);
	cpu->V = cpu->C = false;
}

OP(extb_l) {
	int8_t v = cpu->D[dn];
	int32_t w = v;
	TEST_W(cpu, w);
	cpu->V = cpu->C = false;
	cpu->D[reg] = w;
}

OP(tst_b) {
	EA ea(cpu, mode, 1, reg, false);
	TEST_B(cpu, ea.read());
	cpu->V = cpu->C = false;
}

OP(tst_w) {
	EA ea(cpu, mode, 2, reg, false);
	TEST_W(cpu, ea.read());
	cpu->V = cpu->C = false;
}

OP(tst_l) {
	EA ea(cpu, mode, 4, reg, false);
	TEST_L(cpu, ea.read());
	cpu->V = cpu->C = false;
}

OP(tas) {
	EA ea(cpu, mode, 1, reg, true);
	uint8_t v = ea.read();
	TEST_B(cpu, v);
	cpu->V = cpu->C = false;
	ea.write(v |0x80 );
}

OP(swap) {
	uint32_t v = cpu->D[reg];
	v = (v>>16)|(v<<16);
	cpu->N = v >> 31;
	cpu->Z = v == 0;
	cpu->V = cpu->C = false;
	cpu->D[reg] = (v>>16)|(v<<16);
}

OP(asr_b_i) {
	cpu->setD_B(reg, ASR_B(cpu, cpu->D[reg], dn ? dn : 8));
}

OP(lsr_b_i) {
	cpu->setD_B(reg, LSR_B(cpu, cpu->D[reg], dn ? dn : 8));
}
OP(roxr_b_i) {
	cpu->setD_B(reg, ROXR_B(cpu, cpu->D[reg], dn ? dn : 8));
}
OP(ror_b_i) {
	cpu->setD_B(reg, ROR_B(cpu, cpu->D[reg], dn ? dn : 8));
}

OP(asr_b_d) {
	cpu->setD_B(reg, ASR_B(cpu, cpu->D[reg], cpu->D[dn] & 63));
}

OP(lsr_b_d) {
	cpu->setD_B(reg, LSR_B(cpu, cpu->D[reg], cpu->D[dn] & 63));
}
OP(roxr_b_d) {
	cpu->setD_B(reg, ROXR_B(cpu, cpu->D[reg], cpu->D[dn] & 63));
}
OP(ror_b_d) {
	cpu->setD_B(reg, ROR_B(cpu, cpu->D[reg], cpu->D[dn] & 63));
}


OP(asr_w_i) {
	cpu->setD_W(reg, ASR_W(cpu, cpu->D[reg], dn ? dn : 8));
}

OP(lsr_w_i) {
	cpu->setD_W(reg, LSR_W(cpu, cpu->D[reg], dn ? dn : 8));
}
OP(roxr_w_i) {
	cpu->setD_W(reg, ROXR_W(cpu, cpu->D[reg], dn ? dn : 8));
}
OP(ror_w_i) {
	cpu->setD_W(reg, ROR_W(cpu, cpu->D[reg], dn ? dn : 8));
}

OP(asr_w_d) {
	cpu->setD_W(reg, ASR_W(cpu, cpu->D[reg], cpu->D[dn] & 63));
}

OP(lsr_w_d) {
	cpu->setD_W(reg, LSR_W(cpu, cpu->D[reg], cpu->D[dn] & 63));
}
OP(roxr_w_d) {
	cpu->setD_W(reg, ROXR_W(cpu, cpu->D[reg], cpu->D[dn] & 63));
}
OP(ror_w_d) {
	cpu->setD_W(reg, ROR_W(cpu, cpu->D[reg], cpu->D[dn] & 63));
}

OP(asr_l_i) {
	cpu->D[reg] = ASR_L(cpu, cpu->D[reg], dn ? dn : 8);
}

OP(lsr_l_i) {
	cpu->D[reg] = LSR_L(cpu, cpu->D[reg], dn ? dn : 8);
}
OP(roxr_l_i) {
	cpu->D[reg] = ROXR_L(cpu, cpu->D[reg], dn ? dn : 8);
}
OP(ror_l_i) {
	cpu->D[reg] = ROR_L(cpu, cpu->D[reg], dn ? dn : 8);
}

OP(asr_l_d) {
	cpu->D[reg] = ASR_L(cpu, cpu->D[reg], cpu->D[dn] & 63);
}

OP(lsr_l_d) {
	cpu->D[reg] = LSR_L(cpu, cpu->D[reg], cpu->D[dn] & 63);
}
OP(roxr_l_d) {
	cpu->D[reg] = ROXR_L(cpu, cpu->D[reg], cpu->D[dn] & 63);
}
OP(ror_l_d) {
	cpu->D[reg] = ROR_L(cpu, cpu->D[reg], cpu->D[dn] & 63);
}

OP(asr_ea) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( ASR_W(cpu, ea.read(), 1));
}

OP(lsr_ea) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( LSR_W(cpu, ea.read(), 1));
}

OP(roxr_ea) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( ROXR_W(cpu, ea.read(), 1));
}

OP(ror_ea) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( ROR_W(cpu, ea.read(), 1));
}

void bf_op(CPU* cpu, uint16_t op, int* offset, int* width) {
	*offset = (op >> 6) & 0x1f; 
	*width = op & 0x1f;
	if( (op >> 11) & 1 )
		*offset = cpu->D[*offset];
	if( (op>>5) & 1 ) {
		*width = (cpu->D[*width]) & 31;
	}	
	if( *width == 0 )
		*width = 32;
}
static uint32_t mask_of(int n) {
	return ~0u >> (32-n);
}
template<class T>
static T get_bits(T v, int offset, int sz) {
	return v >> offset & mask_of(sz);
}
template<class T>
static T set_bits(T v, int offset, int sz, T x) {
	uint32_t mk = mask_of(sz);
	return v &~ (mk << offset ) | ((x & mk) << offset);
}
uint32_t get_bf_m_u(CPU* cpu, uint32_t addr, int offset, int sz) {
	addr += offset>>3;
	offset &= 7;
	uint64_t tmp = 0;
	int bits = offset + sz;
	uint64_t v = 0;
	for(int i = 0; i <= (bits-1) / 8; ++i ) {
		uint8_t vv = cpu->mmu->read_b(addr+i);
		if( i == 0 ) {
			v = vv & mask_of(8-offset);
		} else {
			v = (v << 8) | vv;
		}
	}
	return v >> ((8-(bits&7))&7);
}

int32_t get_bf_m_s(CPU* cpu, uint32_t addr, int offset, int sz) {
	int32_t v = get_bf_m_u(cpu, addr, offset, sz);
	return (int32_t)(v << (32-sz)) >> (32-sz);
}

void set_bf_m(CPU* cpu, uint32_t addr, int offset, int sz, uint32_t v) {
	addr += offset>>3;
	offset &= 7;
	int ln = offset + sz;	
	int lr = 8 - offset;
	if( ln <= 8 ) {
		uint8_t q = cpu->mmu->read_b(addr);
		cpu->mmu->write_b(addr, set_bits<uint8_t>(q, 8-ln, sz, v));
	} else {
		int bs = (ln-1)/8;
		for(int i = bs; i >= 0; --i ) {
			uint8_t q = cpu->mmu->read_b(addr+i);
			if( i == bs ) {
				q = set_bits<uint8_t>(q, 8-(ln&7), ln & 7, v);
				v >>= (ln & 7);
			} else if( i == 0 ) {				
				q = set_bits<uint8_t>(q, 0, 8-offset, v );
			} else {
				q = v & 0xff;
				v >>= 8;
			}
			cpu->mmu->write_b(addr+i, q);
		}
	}	
}

uint32_t get_bf_d_u(uint32_t v, int offset, int sz) {
	if( offset < 0 ) {
		offset += 32;
	}
	int ln = offset + sz;
	if( ln < 32 ) {
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
		return old &~ (mask_of(sz) << (32-ln))
			| v << (32-ln);
	} else {
		old = ROL(old, ln-32);
		old = old &~ mask_of(sz) | v;
		return ROR(old, ln-32);
	}
}

OP(bftst_m) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t op2 = cpu->fetch_w();
	int off, sz;
	bf_op(cpu, op2, &off, &sz);
	uint32_t v = get_bf_m_u(cpu, ea.addr(), off, sz);

	cpu->V = cpu->C = false;
	cpu->Z = ! v;
	cpu->N = (v >> (sz-1)) & 1;
}

OP(bftst_d) {
	uint32_t v = cpu->D[ reg ];
	uint16_t op2 = cpu->fetch_w();
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off&31, wt);
	cpu->V = cpu->C = false;
	cpu->Z = ! v2;
	cpu->N = (v2 >> (wt-1)) & 1;
}

OP(bfchg_m) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t op2 = cpu->fetch_w();
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v = get_bf_m_u(cpu, ea.addr(), off, wt) ;
	set_bf_m(cpu, ea.addr(), off, wt, v ^ mask_of(wt));
	cpu->V = cpu->C = false;
	cpu->Z = ! v;
	cpu->N = (v >> (wt-1)) & 1;	
}

OP(bfchg_d) {
	uint32_t v = cpu->D[ reg ];
	uint16_t op2 = cpu->fetch_w();
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off, wt) ;
	cpu->D[reg] = set_bf_d(v, off, wt, v2 ^ mask_of(wt));
	cpu->V = cpu->C = false;
	cpu->Z = ! v2;
	cpu->N = (v2 >> (wt-1)) & 1;
}

OP(bfclr_m) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t op2 = cpu->fetch_w();
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v = get_bf_m_u(cpu, ea.addr(), off, wt) ;
	set_bf_m(cpu, ea.addr(), off, wt, 0);
	cpu->V = cpu->C = false;
	cpu->Z = ! v;
	cpu->N = (v >> (wt-1)) & 1;	
}

OP(bfclr_d) {
	uint32_t v = cpu->D[ reg ];
	uint16_t op2 = cpu->fetch_w();
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off, wt) ;
	cpu->D[reg] = set_bf_d(v, off, wt, 0);
	cpu->V = cpu->C = false;
	cpu->Z = ! v2;
	cpu->N = (v2 >> (wt-1)) & 1;
}

OP(bfset_m) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t op2 = cpu->fetch_w();
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v = get_bf_m_u(cpu, ea.addr(), off, wt) ;
	set_bf_m(cpu, ea.addr(), off, wt, mask_of(wt));
	cpu->V = cpu->C = false;
	cpu->Z = ! v;
	cpu->N = (v >> (wt-1)) & 1;	
}

OP(bfset_d) {
	uint32_t v = cpu->D[ reg ];
	uint16_t op2 = cpu->fetch_w();
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off, wt) ;
	cpu->D[reg] = set_bf_d(v, off, wt, mask_of(wt));
	cpu->V = cpu->C = false;
	cpu->Z = ! v2;
	cpu->N = (v2 >> (wt-1)) & 1;
}



OP(asl_b_i) {
	cpu->setD_B(reg, ASL_B(cpu, cpu->D[reg], dn ? dn : 8));
}

OP(lsl_b_i) {
	cpu->setD_B(reg, LSL_B(cpu, cpu->D[reg], dn ? dn : 8));
}
OP(roxl_b_i) {
	cpu->setD_B(reg, ROXL_B(cpu, cpu->D[reg], dn ? dn : 8));
}
OP(rol_b_i) {
	cpu->setD_B(reg, ROL_B(cpu, cpu->D[reg], dn ? dn : 8));
}

OP(asl_b_d) {
	cpu->setD_B(reg, ASL_B(cpu, cpu->D[reg], cpu->D[dn] & 63));
}

OP(lsl_b_d) {
	cpu->setD_B(reg, LSL_B(cpu, cpu->D[reg], cpu->D[dn] & 63));
}
OP(roxl_b_d) {
	cpu->setD_B(reg, ROXL_B(cpu, cpu->D[reg], cpu->D[dn] & 63));
}
OP(rol_b_d) {
	cpu->setD_B(reg, ROL_B(cpu, cpu->D[reg], cpu->D[dn] & 63));
}


OP(asl_w_i) {
	cpu->setD_W(reg, ASL_W(cpu, cpu->D[reg], dn ? dn : 8));
}

OP(lsl_w_i) {
	cpu->setD_W(reg, LSL_W(cpu, cpu->D[reg], dn ? dn : 8));
}
OP(roxl_w_i) {
	cpu->setD_W(reg, ROXL_W(cpu, cpu->D[reg], dn ? dn : 8));
}
OP(rol_w_i) {
	cpu->setD_W(reg, ROL_W(cpu, cpu->D[reg], dn ? dn : 8));
}

OP(asl_w_d) {
	cpu->setD_W(reg, ASL_W(cpu, cpu->D[reg], cpu->D[dn] & 63));
}

OP(lsl_w_d) {
	cpu->setD_W(reg, LSL_W(cpu, cpu->D[reg], cpu->D[dn] & 63));
}
OP(roxl_w_d) {
	cpu->setD_W(reg, ROXL_W(cpu, cpu->D[reg], cpu->D[dn] & 63));
}
OP(rol_w_d) {
	cpu->setD_W(reg, ROL_W(cpu, cpu->D[reg], cpu->D[dn] & 63));
}

OP(asl_l_i) {
	cpu->D[reg] = ASL_L(cpu, cpu->D[reg], dn ? dn : 8);
}

OP(lsl_l_i) {
	cpu->D[reg] = LSL_L(cpu, cpu->D[reg], dn ? dn : 8);
}
OP(roxl_l_i) {
	cpu->D[reg] = ROXL_L(cpu, cpu->D[reg], dn ? dn : 8);
}
OP(rol_l_i) {
	cpu->D[reg] = ROL_L(cpu, cpu->D[reg], dn ? dn : 8);
}

OP(asl_l_d) {
	cpu->D[reg] = ASL_L(cpu, cpu->D[reg], cpu->D[dn] & 63);
}

OP(lsl_l_d) {
	cpu->D[reg] = LSL_L(cpu, cpu->D[reg], cpu->D[dn] & 63);
}
OP(roxl_l_d) {
	cpu->D[reg] = ROXL_L(cpu, cpu->D[reg], cpu->D[dn] & 63);
}
OP(rol_l_d) {
	cpu->D[reg] = ROL_L(cpu, cpu->D[reg], cpu->D[dn] & 63);
}

OP(asl_ea) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( ASL_W(cpu, ea.read(), 1));
}

OP(lsl_ea) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( LSL_W(cpu, ea.read(), 1));
}

OP(roxl_ea) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( ROXL_W(cpu, ea.read(), 1));
}

OP(rol_ea) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( ROL_W(cpu, ea.read(), 1));
}


OP(bfextu_m) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t op2 = cpu->fetch_w();
	int off, sz;
	int dn2 = (op2 >> 12) & 7;
	bf_op(cpu, op2, &off, &sz);
	uint32_t v = get_bf_m_u(cpu, ea.addr(), off, sz);
	cpu->D[dn2] = v;
	cpu->V = cpu->C = false;
	cpu->Z = ! v;
	cpu->N = (v >> (sz-1)) & 1;
}

OP(bfextu_d) {
	uint32_t v = cpu->D[ reg ];
	uint16_t op2 = cpu->fetch_w();
	int dn2 = (op2 >> 12) & 7;
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off&31, wt);
	cpu->D[dn2] = v2;
	cpu->V = cpu->C = false;
	cpu->Z = ! v2;
	cpu->N = (v2 >> (wt-1)) & 1;
}

OP(bfexts_m) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t op2 = cpu->fetch_w();
	int off, sz;
	int dn2 = (op2 >> 12) & 7;
	bf_op(cpu, op2, &off, &sz);
	int32_t v = get_bf_m_s(cpu, ea.addr(), off, sz);
	cpu->D[dn2] = v;
	cpu->V = cpu->C = false;
	cpu->Z = ! v;
	cpu->N = (v >> (sz-1)) & 1;
}

OP(bfexts_d) {
	uint32_t v = cpu->D[ reg ];
	uint16_t op2 = cpu->fetch_w();
	int dn2 = (op2 >> 12) & 7;
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	int32_t v2 = get_bf_d_s(v, off&31, wt);
	cpu->D[dn2] = v2;
	cpu->V = cpu->C = false;
	cpu->Z = ! v2;
	cpu->N = (v2 >> (wt-1)) & 1;
}
static int bfffo(uint32_t v, int size, CPU* cpu) {
	int k = size;
	if( v ) {
		int clz = __builtin_clz(v);
		k -= (32-clz);
	}
	cpu->V = cpu->C = false;
	cpu->Z = ! v;
	cpu->N = (v >> (size-1)) & 1;
	return k;
}
OP(bfffo_m) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t op2 = cpu->fetch_w();
	int off, sz;
	int dn2 = (op2 >> 12) & 7;
	bf_op(cpu, op2, &off, &sz);
	uint32_t v = get_bf_m_u(cpu, ea.addr(), off, sz);
	cpu->D[dn2] = bfffo(v, sz, cpu) + off;
}

OP(bfffo_d) {
	uint32_t v = cpu->D[ reg ];
	uint16_t op2 = cpu->fetch_w();
	int dn2 = (op2 >> 12) & 7;
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v2 = get_bf_d_u(v, off&31, wt);
	cpu->D[dn2] = bfffo(v2, wt, cpu) + off;
}

OP(bfins_m) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t op2 = cpu->fetch_w();
	int off, sz;
	int dn2 = (op2 >> 12) & 7;
	bf_op(cpu, op2, &off, &sz);
	uint32_t v2 = cpu->D[dn2] & mask_of(sz);
	cpu->V = cpu->C = false;
	cpu->Z = ! v2;
	cpu->N = (v2 >> (sz-1)) & 1;
	set_bf_m(cpu, ea.addr(), off, sz, v2);
}

OP(bfins_d) {
	uint32_t v = cpu->D[ reg ];
	uint16_t op2 = cpu->fetch_w();
	int dn2 = (op2 >> 12) & 7;
	int off, wt;
	bf_op(cpu, op2, &off, &wt);
	uint32_t v2 = cpu->D[dn2] & mask_of(wt);
	cpu->V = cpu->C = false;
	cpu->Z = ! v2;
	cpu->N = (v2 >> (wt-1)) & 1;
	v = set_bf_d(v, off&31, wt, v2);
	cpu->D[reg] = v;
}
