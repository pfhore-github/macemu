#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#define OP(name_) extern "C" void op_##name_(CPU* cpu, uint16_t op __attribute__((unused)), int dn __attribute__((unused)), int mode __attribute__((unused)), int reg __attribute__((unused)))
#include "op.h"
#include "intel.h"


OP(addi_b) {
	int8_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, true);
	ea.write( ADD_B(cpu, ea.read(), imm) );
}


OP(addi_w) {
	uint16_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 2, reg, true);
	ea.write( ADD_W(cpu, ea.read(), imm) );
}

OP(addi_l) {
	uint32_t imm = cpu->fetch_l();
	EA ea(cpu, mode, 4, reg, true);
	ea.write( ADD_L(cpu, ea.read(), imm) );
}


OP(cas_b) {
	uint16_t nw = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, true);
	int u = (nw >> 6) & 7;
	int c = nw & 7;
	uint8_t v = ea.read();
	CMP_B(cpu, v, cpu->D[c]);
	if( cpu->Z ) {
		ea.write(cpu->D[u]);
	} else {
		cpu->D[c] = v;
	}
	test_trace_branch(cpu);
}


OP(cmpi_b) {
	uint8_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, false);
	CMP_B(cpu, ea.read(), imm);
}

OP(cmpi_w) {
	uint16_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 2, reg, false);
	CMP_W(cpu, ea.read(), imm);
}

OP(cmpi_l) {
	uint32_t imm = cpu->fetch_l();
	EA ea(cpu, mode, 4, reg, false);
	CMP_L(cpu, ea.read(), imm);
}

OP(cas2_w) {
	uint16_t nw = cpu->fetch_w();
	uint16_t nw2 = cpu->fetch_w();
	int s1 = (nw >> 12) & 15;
	int u1 = (nw >> 6) & 7;
	int c1 = nw & 7;
	int s2 = (nw2 >> 12) & 15;
	int u2 = (nw2 >> 6) & 7;
	int c2 = nw2 & 7;
	CMP_W(cpu, cpu->mmu->read_w(cpu->R[s1]), cpu->D[c1]);
	if( cpu->Z ) {
		CMP_W(cpu, cpu->mmu->read_w(cpu->R[s2]), cpu->D[c2]);
		if( cpu->Z ) {
			cpu->mmu->write_w(cpu->R[s1], cpu->D[u1]);
			cpu->mmu->write_w(cpu->R[s2], cpu->D[u2]);
			goto end;
		}
	}
	cpu->D[c1] = cpu->mmu->read_w(cpu->R[s1]);
	cpu->D[c2] = cpu->mmu->read_w(cpu->R[s2]);
  end:
	test_trace_branch(cpu);
}
OP(cas_w) {
	uint16_t nw = cpu->fetch_w();
	EA ea(cpu, mode, 2, reg, true);
	int u = (nw >> 6) & 7;
	int c = nw & 7;
	uint16_t v = ea.read();
	CMP_W(cpu, v, cpu->D[c]);
	if( cpu->Z ) {
		ea.write(cpu->D[u]);
	} else {
		cpu->D[c] = v;
	}
	test_trace_branch(cpu);
}

OP(addq_b) {
	EA ea(cpu, mode, 1, reg, true);	
	int8_t v = ea.read();
	v = ADD_B(cpu, v, dn ? dn : 8);
	ea.write( v );
}

OP(addq_w) {
	EA ea(cpu, mode, 2, reg, true);	
	int16_t v = ea.read();
	v = ADD_W(cpu, v, dn ? dn : 8);
	ea.write( v );
}

OP(addq_l) {
	EA ea(cpu, mode, 4, reg, true);	
	int32_t v = ea.read();
	v = ADD_L(cpu, v, dn ? dn : 8);
	ea.write( v );
}

OP(subq_b) {
	EA ea(cpu, mode, 1, reg, true);	
	int8_t v = ea.read();
	v = SUB_B(cpu, v, dn);
	ea.write( v );
}

OP(subq_w) {
	EA ea(cpu, mode, 2, reg, true);	
	int16_t v = ea.read();
	v = SUB_W(cpu, v, dn);
	ea.write( v );
}

OP(subq_l) {
	EA ea(cpu, mode, 4, reg, true);	
	int32_t v = ea.read();
	v = SUB_L(cpu, v, dn);
	ea.write( v );
}

OP(subi_b) {
	uint8_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, true);
	ea.write(SUB_B(cpu, ea.read(), imm));
}


OP(subi_w) {
	uint16_t imm = cpu->fetch_w();
	EA ea(cpu, mode, 2, reg, true);
	ea.write( SUB_W(cpu, ea.read(), imm));
}


OP(subi_l) {
	uint32_t imm = cpu->fetch_l();
	EA ea(cpu, mode, 4, reg, true);
	ea.write( SUB_L(cpu, ea.read(), imm) );
}

OP(cmp2_l) {
	int wd = cpu->fetch_w();
	int32_t v = cpu->R[wd >> 12];
	EA ea(cpu, mode, 4, reg, false);
	int32_t lw = cpu->mmu->read_l(ea.addr());
	int32_t hi = cpu->mmu->read_l(ea.addr()+4);
	cpu->Z = ( v == lw || v == hi );
	cpu->C = ( v < lw || v > hi );
	if( ((wd >> 11) & 1) && cpu->C ) {
		throw ChkTrap();
	}
}

OP(cas2_l) {
	uint16_t nw = cpu->fetch_w();
	uint16_t nw2 = cpu->fetch_w();
	int s1 = (nw >> 12) & 15;
	int u1 = (nw >> 6) & 7;
	int c1 = nw & 7;
	int s2 = (nw2 >> 12) & 15;
	int u2 = (nw2 >> 6) & 7;
	int c2 = nw2 & 7;
	CMP_L(cpu, cpu->mmu->read_l(cpu->R[s1]), cpu->D[c1]);
	if( cpu->Z ) {
		CMP_L(cpu, cpu->mmu->read_l(cpu->R[s2]), cpu->D[c2]);
		if( cpu->Z ) {
			cpu->mmu->write_l(cpu->R[s1], cpu->D[u1]);
			cpu->mmu->write_l(cpu->R[s2], cpu->D[u2]);
			goto done;
		}
	}
	cpu->D[c1] = cpu->mmu->read_l(cpu->R[s1]);
	cpu->D[c2] = cpu->mmu->read_l(cpu->R[s2]);
  done:
	test_trace_branch(cpu);
}
OP(cas_l) {
	uint16_t nw = cpu->fetch_w();
	EA ea(cpu, mode, 4, reg, true);
	int u = (nw >> 6) & 7;
	int c = nw & 7;
	uint32_t v = ea.read();
	CMP_L(cpu, v, cpu->D[c]);
	if( cpu->Z ) {
		ea.write(cpu->D[u]);
	} else {
		cpu->D[c] = v;
	}
	test_trace_branch(cpu);
}

OP(negx_b) { 
	EA ea(cpu, mode, 1, reg, true);
	uint8_t v = ea.read() + cpu->X;
	ea.write( SUB_B(cpu, 0, v) );
	cpu->X = cpu->C;
}

OP(negx_w) { 
	EA ea(cpu, mode, 2, reg, true);
	uint16_t v = ea.read() + cpu->X;
	ea.write( SUB_W(cpu, 0, v) );
	cpu->X = cpu->C;
}

OP(negx_l) { 
	EA ea(cpu, mode, 4, reg, true);
	uint32_t v = ea.read() + cpu->X;
	ea.write( SUB_L(cpu, 0, v) );
	cpu->X = cpu->C;
}

OP(neg_b) {
	EA ea(cpu, mode, 1, reg, true);
	uint8_t v = ea.read();
	ea.write( SUB_B(cpu, 0, v) );
	cpu->X = cpu->C;
}

OP(neg_w) {
	EA ea(cpu, mode, 2, reg, true);
	uint16_t v = ea.read();
	ea.write( SUB_W(cpu, 0, v) );
	cpu->X = cpu->C;
}

OP(neg_l) {
	EA ea(cpu, mode, 4, reg, true);
	uint32_t v = ea.read();
	ea.write( SUB_L(cpu, 0, v) );
	cpu->X = cpu->C;
}


OP(mul_l) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t nw = cpu->fetch_w();
	uint32_t src = ea.read();
	int di = ( nw >> 12 ) & 7;
	int dh = nw & 7;
	if( (nw >> 11) & 1 ) {
		if( (nw >> 10) & 1 ) {
			MUL_U(cpu, src, cpu->D[di], &cpu->D[dh], &cpu->D[di]);
		} else {
			uint32_t ret;
			cpu->V = __builtin_umul_overflow(src, cpu->D[di], &ret);
			cpu->C = false;
			set_nz(cpu, (int32_t)ret);
			cpu->D[di] = ret;
		}
	} else {
		if( (nw >> 10) & 1 ) {
			MUL_S(cpu, src, cpu->D[di], &cpu->D[dh], &cpu->D[di]);
		} else {
			int32_t ret;
			cpu->V = __builtin_smul_overflow(src, cpu->D[di], &ret);
			cpu->C = false;
			set_nz(cpu, ret);
			cpu->D[di] = ret;
		}
	}
}

OP(div_l) {
	EA ea(cpu, mode, 4, reg, false);
	uint16_t nw = cpu->fetch_w();
	uint32_t src = ea.read();
	if( src == 0 ) {
		throw DivByZero();
	}
	int dq = ( nw >> 12 ) & 7;
	int dr = nw & 7;
	if( (nw >> 11) & 1 ) {
		if( (nw >> 10) & 1 ) {
			uint64_t v = (uint64_t)cpu->D[dr] << 32 | cpu->D[dq];
			DIV_U(cpu, v, src, &cpu->D[dq], &cpu->D[dr]);
		} else {
			uint32_t v = cpu->D[dq];
			uint32_t q = v / src;
			uint32_t r = v % src;
			cpu->C = false;
			set_nz(cpu, (int32_t)q);
			cpu->V = false;
			cpu->D[dr] = r;
			cpu->D[dq] = q;
		}
	} else {
		int32_t srcs = src;
		if( (nw >> 10) & 1 ) {
			int64_t v = (int64_t)cpu->D[dr] << 32 | cpu->D[dq];
			DIV_S(cpu, v, src, &cpu->D[dq], dq == dr ? nullptr : &cpu->D[dr]);
		} else {
			if( cpu->D[dq] == 0x80000000 && srcs == -1 ) {
				cpu->V = true;
				return;
			}
			int32_t v = cpu->D[dq];
			int32_t q = v / srcs;
			int32_t r = v % srcs;
			cpu->C = false;
			set_nz(cpu, q);
			cpu->D[dr] = r;
			cpu->D[dq] = q;
		}
	}
}


OP(divu_w) {
	EA ea(cpu, mode, 2, reg, false);
	uint32_t d = cpu->D[dn];
	uint16_t s = ea.read();
	if( s == 0 ) {
		throw DivByZero();
	}
	uint32_t q = d / s;
	uint32_t r = d % s;
	cpu->V = q >> 16;
	cpu->N = ( q >> 15 ) & 1;
	cpu->Z = ! q;
	cpu->C = false;
	cpu->D[dn] = r << 16 | (uint16_t)q;
}

OP(divs_w) {
	EA ea(cpu, mode, 2, reg, false);
	int32_t d = cpu->D[dn];
	int16_t s = ea.read();
	if( s == 0 ) {
		throw DivByZero();
	}
	int32_t q = d / s;
	int32_t r = d % s;
	int16_t qq = q;
	cpu->V = qq != q;
	cpu->N = qq >> 15;
	cpu->Z = ! q;
	cpu->C = false;
	cpu->D[dn] = r << 16 | (uint16_t)qq;
}

OP(sub_b) {
	EA ea(cpu, mode, 1, reg, false);
	cpu->setD_B(dn, SUB_B(cpu, cpu->D[dn], ea.read()));
}

OP(sub_w) {
	EA ea(cpu, mode, 2, reg, false);
	cpu->setD_W(dn, SUB_W(cpu, cpu->D[dn], ea.read()));
}

OP(suba_w) {
	EA ea(cpu, mode, 2, reg, false);
	cpu->setA_W(dn, SUB_W(cpu, (int16_t)cpu->A[dn], ea.read()));
}

OP(sub_l) {
	EA ea(cpu, mode, 4, reg, false);
	cpu->D[dn] = SUB_L(cpu, cpu->D[dn], ea.read());
}

OP(suba_l) {
	EA ea(cpu, mode, 4, reg, false);
	cpu->A[dn] = SUB_L(cpu, cpu->A[dn], ea.read());
}

OP(sub_m_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write( SUB_B(cpu, cpu->D[dn], ea.read()));
}

OP(sub_m_w) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( SUB_W(cpu, cpu->D[dn], ea.read()));
}

OP(sub_m_l) {
	EA ea(cpu, mode, 4, reg, true);
	ea.write( SUB_L(cpu, cpu->D[dn], ea.read()));
}

OP(subx_b) {
	cpu->setD_B(dn, SUB_B(cpu, cpu->D[dn], cpu->D[reg]+cpu->X));
}

OP(subx_w) {
	cpu->setD_W(dn, SUB_W(cpu, cpu->D[dn], cpu->D[reg]+cpu->X));
}

OP(subx_l) {
	cpu->D[dn] = SUB_L(cpu, cpu->D[dn], cpu->D[reg]+cpu->X);
}

OP(subx_m_b) {
	int8_t v1 = cpu->mmu->read_b(--cpu->A[dn]);
	int8_t v2 = cpu->mmu->read_b(--cpu->A[reg]);
	cpu->mmu->write_b(cpu->A[dn], SUB_B(cpu, v1, v2+cpu->X));
}

OP(subx_m_w) {
	int16_t v1 = cpu->mmu->read_w(cpu->A[dn] -= 2);
	int16_t v2 = cpu->mmu->read_w(cpu->A[reg] -= 2);
	cpu->mmu->write_w(cpu->A[dn], SUB_W(cpu, v1, v2+cpu->X));
}

OP(subx_m_l) {
	int32_t v1 = cpu->mmu->read_l(cpu->A[dn] -= 4);
	int32_t v2 = cpu->mmu->read_l(cpu->A[reg] -= 4);
	cpu->mmu->write_l(cpu->A[dn], SUB_L(cpu, v1, v2+cpu->X));
}

OP(cmp_b) {
	EA ea(cpu, mode, 1, reg, false);
	CMP_B(cpu, cpu->D[dn], ea.read());
}

OP(cmp_w) {
	EA ea(cpu, mode, 2, reg, false);
	CMP_W(cpu, cpu->D[dn], ea.read());
}

OP(cmp_l) {
	EA ea(cpu, mode, 4, reg, false);
	CMP_L(cpu, cpu->D[dn], ea.read());
}

OP(cmpa_w) {
	EA ea(cpu, mode, 2, reg, false);
	CMP_W(cpu, (int16_t)cpu->A[dn], ea.read());
}

OP(cmpa_l) {
	EA ea(cpu, mode, 4, reg, false);
	CMP_L(cpu, cpu->A[dn], ea.read());
}

OP(cmpm_b) {
	int8_t v1 = cpu->mmu->read_b(cpu->A[dn]);
	int8_t v2 = cpu->mmu->read_b(cpu->A[reg]);
	CMP_B(cpu, v1, v2);
	cpu->A[dn] += 1;
	cpu->A[reg] += 1;
}

OP(cmpm_w) {
	int16_t v1 = cpu->mmu->read_w(cpu->A[dn]);
	int16_t v2 = cpu->mmu->read_w(cpu->A[reg]);
	CMP_W(cpu, v1, v2);
	cpu->A[dn] += 2;
	cpu->A[reg] += 2;
}

OP(cmpm_l) {
	int32_t v1 = cpu->mmu->read_l(cpu->A[dn]);
	int32_t v2 = cpu->mmu->read_l(cpu->A[reg]);
	CMP_L(cpu, v1, v2);
	cpu->A[dn] += 4;
	cpu->A[reg] += 4;
}

OP(mulu_w) {
	EA ea(cpu, mode, 2, reg, false);
	uint16_t src = ea.read();
	uint16_t v = cpu->D[dn];
	uint32_t x = (uint32_t)src * v;
	cpu->D[dn] = x;
	cpu->C = cpu->V = false;
	cpu->N = x >> 31;
	cpu->Z = ! x;
}

OP(muls_w) {
	EA ea(cpu, mode, 2, reg, false);
	int16_t src = ea.read();
	int16_t v = cpu->D[dn];
	int32_t x = (int32_t)src * v;
	cpu->D[dn] = x;
	cpu->C = cpu->V = false;
	cpu->N = x >> 31;
	cpu->Z = ! x;
}


OP(add_b) {
	EA ea(cpu, mode, 1, reg, false);
	cpu->setD_B(dn, ADD_B(cpu, cpu->D[dn], ea.read()));
}

OP(add_w) {
	EA ea(cpu, mode, 2, reg, false);
	cpu->setD_W(dn, ADD_W(cpu, cpu->D[dn], ea.read()));
}

OP(adda_w) {
	EA ea(cpu, mode, 2, reg, false);
	cpu->setA_W(dn, (int16_t)cpu->A[dn] + ea.read());
}

OP(add_l) {
	EA ea(cpu, mode, 4, reg, false);
	cpu->D[dn] = ADD_L(cpu, cpu->D[dn], ea.read());
}

OP(adda_l) {
	EA ea(cpu, mode, 4, reg, false);
	cpu->A[dn] += (uint32_t)ea.read();
}

OP(add_m_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write( ADD_B(cpu, cpu->D[dn], ea.read()));
}

OP(add_m_w) {
	EA ea(cpu, mode, 2, reg, true);
	ea.write( ADD_W(cpu, cpu->D[dn], ea.read()));
}

OP(add_m_l) {
	EA ea(cpu, mode, 4, reg, true);
	ea.write( ADD_L(cpu, cpu->D[dn], ea.read()));
}

OP(addx_b) {
	bool oz = cpu->Z;
	cpu->setD_B(dn, ADD_B(cpu, cpu->D[dn], cpu->D[reg]+cpu->X));
	cpu->Z = cpu->Z && oz;
}

OP(addx_w) {
	bool oz = cpu->Z;
	cpu->setD_W(dn, ADD_W(cpu, cpu->D[dn], cpu->D[reg]+cpu->X));
	cpu->Z = cpu->Z && oz;
}

OP(addx_l) {
	bool oz = cpu->Z;
	cpu->D[dn] = ADD_L(cpu, cpu->D[dn], cpu->D[reg]+cpu->X);
	cpu->Z = cpu->Z && oz;
}

OP(addx_m_b) {
	int8_t v1 = cpu->mmu->read_b(--cpu->A[dn]);
	int8_t v2 = cpu->mmu->read_b(--cpu->A[reg]);
	cpu->mmu->write_b(cpu->A[dn], ADD_B(cpu, v1, v2+cpu->X));
}

OP(addx_m_w) {
	int16_t v1 = cpu->mmu->read_w(cpu->A[dn] -= 2);
	int16_t v2 = cpu->mmu->read_w(cpu->A[reg] -= 2);
	cpu->mmu->write_w(cpu->A[dn], ADD_W(cpu, v1, v2+cpu->X));
}

OP(addx_m_l) {
	int32_t v1 = cpu->mmu->read_l(cpu->A[dn] -= 4);
	int32_t v2 = cpu->mmu->read_l(cpu->A[reg] -= 4);
	cpu->mmu->write_l(cpu->A[dn], ADD_L(cpu, v1, v2+cpu->X));
}
