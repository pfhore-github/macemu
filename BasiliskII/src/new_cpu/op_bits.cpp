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
	EA ea(cpu, mode, 1, reg, true);
	cpu->Z = BTST(ea.read(), bn);
}


OP(bchg_d_l) {
	uint32_t v = cpu->D[reg];
	cpu->D[reg] = BCHG(v, cpu->D[dn], &cpu->Z);
}

OP(bchg_d_b) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write(BCHG(ea.read(), dn, &cpu->Z));
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
	cpu->setD_B(reg, ASR_B(cpu, cpu->D[reg], dn));
}

OP(lsr_b_i) {
	cpu->setD_B(reg, LSR_B(cpu, cpu->D[reg], dn));
}
OP(roxr_b_i) {
	cpu->setD_B(reg, ROXR_B(cpu, cpu->D[reg], dn));
}
