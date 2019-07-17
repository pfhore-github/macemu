#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
#define OP(name_) extern "C" void op_##name_(CPU* cpu, uint16_t op __attribute__((unused)), int dn __attribute__((unused)), int mode __attribute__((unused)), int reg __attribute__((unused)))
#include "op.h"
#include "intel.h"

OP(moves_b) {
	if( ! cpu->S ) {
		PRIV_VIOLATION(cpu);
	}
	uint16_t nw = cpu->fetch_w();
	EA ea(cpu, mode, 1, reg, true);
	uint32_t addr = ea.addr();
	if( (nw >> 11) & 1 ) {
		// to EA
		uint8_t v = cpu->R[(nw>>12)];
		WRITE8( cpu->mmu->to_real(addr, 1, true, cpu->DFC), v);
	} else {
		// from EA
		cpu->R[(nw>>12)] = READ8( cpu->mmu->to_real(addr, 1, false, cpu->SFC) );
	}
	test_trace_branch(cpu);
}

OP(moves_w) {
	if( ! cpu->S ) {
		PRIV_VIOLATION(cpu);
	}
	uint16_t nw = cpu->fetch_w();
	EA ea(cpu, mode, 2, reg, true);
	uint32_t addr = ea.addr();
	if( (nw >> 11) & 1 ) {
		// to EA
		uint16_t v = cpu->R[(nw>>12)];
		WRITE16( cpu->mmu->to_real(addr, 2, true, cpu->DFC), v);
	} else {
		// from EA
		cpu->R[(nw>>12)] = READ16( cpu->mmu->to_real(addr, 2, false, cpu->SFC));
	}
	test_trace_branch(cpu);
}
OP(moves_l) {
	if( ! cpu->S ) {
		PRIV_VIOLATION(cpu);
	}
	uint16_t nw = cpu->fetch_w();
	EA ea(cpu, mode, 4, reg, true);
	uint32_t addr = ea.addr();
	if( (nw >> 11) & 1 ) {
		// to EA
		uint32_t v = cpu->R[(nw>>12)];
		WRITE32( cpu->mmu->to_real(addr, 4, true, cpu->DFC), v);
	} else {
		// from EA
		cpu->R[(nw>>12)] = READ32( cpu->mmu->to_real(addr, 4, false, cpu->SFC));
	}
	test_trace_branch(cpu);
}

OP(movep_m2r_w) {
	int16_t disp = cpu->fetch_w();
	uint32_t addr = cpu->A[reg] +disp;
	cpu->D[dn] = cpu->mmu->read_b(addr) << 8;
	cpu->D[dn] |= cpu->mmu->read_b(addr+2);
}

OP(movep_r2m_w) {
	int16_t disp = cpu->fetch_w();
	uint32_t addr = cpu->A[reg] +disp;
	cpu->mmu->write_b(addr, (cpu->D[dn] >> 8) & 0xff);
	cpu->mmu->write_b(addr + 2, cpu->D[dn] & 0xff);
}

OP(movep_m2r_l) {
	int16_t disp = cpu->fetch_w();
	uint32_t addr = cpu->A[reg] +disp;
	cpu->D[dn] = cpu->mmu->read_b(addr) << 24;
	cpu->D[dn] |= cpu->mmu->read_b(addr+2) << 16;
	cpu->D[dn] |= cpu->mmu->read_b(addr+4) << 8;
	cpu->D[dn] |= cpu->mmu->read_b(addr+6);
}

OP(movep_r2m_l) {
	int16_t disp = cpu->fetch_w();
	uint32_t addr = cpu->A[reg] +disp;
	cpu->mmu->write_b(addr, (cpu->D[dn] >> 24) & 0xff);
	cpu->mmu->write_b(addr + 2, (cpu->D[dn] >> 16) & 0xff);
	cpu->mmu->write_b(addr + 4, (cpu->D[dn] >> 8) & 0xff);
	cpu->mmu->write_b(addr + 6, cpu->D[dn] & 0xff);
}

OP(move_b) {
	EA src(cpu, mode, 1, reg, false);
	EA dst(cpu, (op>>6)&7, 1, dn, true);
	int8_t v = src.read();
	dst.write(v);
	set_nz(cpu, v);
	cpu->V = cpu->C = false;	
}
OP(move_w) {
	EA src(cpu, mode, 2, reg, false);
	EA dst(cpu, (op>>6)&7, 2, dn, true);
	int16_t v = src.read();
	dst.write(v);
	set_nz(cpu, v);
	cpu->V = cpu->C = false;	
}
OP(move_l) {
	EA src(cpu, mode, 4, reg, false);
	EA dst(cpu, (op>>6)&7, 4, dn, true);
	int32_t v = src.read();
	dst.write(v);
	set_nz(cpu, v);
	cpu->V = cpu->C = false;	
}
OP(movea_w) {
	EA src(cpu, mode, 2, reg, false);
	int16_t v = src.read();
	cpu->A[ dn ] = v;
}
OP(movea_l) {
	EA src(cpu, mode, 4, reg, false);
	cpu->A[ dn ] = src.read();
}



OP(move_from_sr) { 
	if( ! cpu->S ) {
		// not supervisand
		PRIV_VIOLATION(cpu);
	}
	EA ea(cpu, mode, 2, reg, true);
	ea.write( get_sr(cpu) );
}

OP(move_from_cr) {
	EA ea(cpu, mode, 1, reg, true);
	ea.write( get_cr(cpu) );
}

OP(move_to_cr) {
	EA ea(cpu, mode, 1, reg, false);
	set_cr( cpu, ea.read());
}


OP(move_to_sr) { 
	if( ! cpu->S ) {
		// not supervisand
		PRIV_VIOLATION(cpu);
	}
	EA ea(cpu, mode, 2, reg, false);
	set_sr( cpu, ea.read() );
	test_trace_branch(cpu);
}

struct rte_call {
	uint32_t ea;
};
void m68k_execute();
uint32_t move_write_w(CPU* cpu, uint32_t addr, int i) {
	try {
		cpu->mmu->write_w(addr, cpu->R[i]);
	} catch( AccessFault& e ) {
		e.CM = true;
		e.run(cpu, cpu->NPC);
		try {
			m68k_execute();
		} catch( rte_call& e2) {
			addr = e2.ea;
		}
	}
	return addr;
}
uint32_t move_read_l(CPU* cpu, uint32_t addr, int i) {
	try {
		cpu->R[i] = cpu->mmu->read_l(addr);
	} catch( AccessFault& e ) {
		e.CM = true;
		e.run(cpu, cpu->NPC);
		try {
			m68k_execute();
		} catch( rte_call& e2) {
			addr = e2.ea;
		}
	}
	return addr;
}
uint16_t move_read_w(CPU* cpu, uint32_t& addr, int i) {
	try {
		cpu->R[i] = cpu->mmu->read_w(addr);
	} catch( AccessFault& e ) {
		e.CM = true;
		e.run(cpu, cpu->NPC);
		try {
			m68k_execute();
		} catch( rte_call& e2) {
			addr = e2.ea;
		}
	}
	return addr;
}
uint32_t move_write_l(CPU* cpu, uint32_t addr, int i) {
	try {
		cpu->mmu->write_l(addr, cpu->R[i]);
	} catch( AccessFault& e ) {
		e.CM = true;
		e.run(cpu, cpu->NPC);
		try {
			m68k_execute();
		} catch( rte_call& e2) {
			addr = e2.ea;
		}
	}
	return addr;
}

OP(movem_w_to_decr) {
	uint32_t pc = cpu->PC;
	uint16_t mask = cpu->fetch_w();
	uint32_t addr = cpu->A[reg];
	for(int i = 15; i >= 0; --i ) {
		if( mask & (1 << i) ) {
			addr = move_write_w(cpu, addr - 2, i);
		}
	}
	cpu->A[reg] = addr;
}

OP(movem_w_to) {
	uint16_t mask = cpu->fetch_w();
	EA ea(cpu, mode, 2, reg, false);
	uint32_t addr = ea.addr();
	for(int i = 0; i < 16; ++i ) {
		if( mask & (1 << i) ) {
			addr = move_write_w(cpu, addr, i) + 2;
		}
	}
}


OP(movem_l_to_decr) {
	uint16_t mask = cpu->fetch_w();
	uint32_t addr = cpu->A[reg];
	for(int i = 15; i >= 0; --i ) {
		if( mask & (1 << i) ) {
			addr = move_write_l(cpu, addr - 4, i);
		}
	}
	cpu->A[reg] = addr;
}

OP(movem_l_to) {
	uint16_t mask = cpu->fetch_w();
	EA ea(cpu, mode, 4, reg, false);
	uint32_t addr = ea.addr();
	for(int i = 0; i < 16; ++i ) {
		if( mask & (1 << i) ) {
			addr = move_write_l(cpu, addr, i) + 4;
		}
	}
}



OP(movem_w_from_incr) {
	uint16_t mask = cpu->fetch_w();
	uint32_t addr = cpu->A[reg];	
	for(int i = 0; i < 16; ++i ) {
		if( mask & (1 << i) ) {
			addr = move_read_w(cpu, addr, i) + 2;
		}
	}
	cpu->A[reg] = addr;
}

OP(movem_w_from) {
	uint16_t mask = cpu->fetch_w();
	EA ea(cpu, mode, 2, reg, false);
	uint32_t addr = ea.addr();
	for(int i = 0; i < 16; ++i ) {
		if( mask & (1 << i) ) {
			addr = move_read_w(cpu, addr, i) + 2;
		}
	}
}

OP(movem_l_from_incr) {
	uint16_t mask = cpu->fetch_w();
	uint32_t addr = cpu->A[reg];
	for(int i = 0; i < 16; ++i ) {
		if( mask & (1 << i) ) {
			addr = move_read_w(cpu, addr, i) + 4;
		}
	}
	cpu->A[reg] = addr;
}

OP(movem_l_from) {
	uint16_t mask = cpu->fetch_w();
	EA ea(cpu, mode, 4, reg, false);
	uint32_t addr = ea.addr();
	for(int i = 0; i < 16; ++i ) {
		if( mask & (1 << i) ) {
			addr = move_read_w(cpu, addr, i) + 4;
		}
	}
}

OP(move_to_usp) {
	if( ! cpu->S ) {
		throw PrivViolation();
	}
	cpu->USP = cpu->A[ reg ];
	test_trace_branch(cpu);
}

OP(move_from_usp) {
	if( ! cpu->S ) {
		throw PrivViolation();
	}
	cpu->A[ reg ] = cpu->USP;
	test_trace_branch(cpu);
}
void m68k_reset (CPU* cpu);

OP(reset) {
	cpu->reset();
}

OP( nop ) {
	test_trace_branch(cpu);
}
extern bool quit_program;

OP(stop) {
	if( ! cpu->S ) {
		throw PrivViolation();
	}
	uint16_t v = cpu->fetch_w();
	set_sr(cpu, v);
	test_trace_branch(cpu);
	quit_program = true;
}

OP(rte) {
	if( ! cpu->S ) {
		throw PrivViolation();
	}
	uint16_t v = cpu->pop16();
	set_sr(cpu, v);
	cpu->PC = cpu->pop32();
	uint16_t nw = cpu->pop16();
	int st= nw >> 12;
	switch(st) {
	case 1 :
		break;
	case 2 :
	case 3 :
		cpu->A[7] += 4;
		break;
	case 7 : {
		uint32_t ea = cpu->pop32();
		uint16_t ssw = cpu->pop16();
		rte_call ee = {ea };
		test_trace_branch(cpu);
		throw rte_call( ee );
	}
	}	
	test_trace_branch(cpu);
}

OP(rtd) {
	int16_t disp =cpu->fetch_w();
	cpu->PC = cpu->pop32();
	cpu->A[7] += disp;
	test_trace_branch(cpu);
}

OP(rts) {
	cpu->PC = cpu->pop32();
	test_trace_branch(cpu);
}

OP(trapv) {
	if( cpu->V ) {
		throw Trapcc();
	}
}

OP(rtr) {
	uint8_t cr = cpu->pop16();
	set_cr(cpu, cr);
	cpu->PC = cpu->pop32();	
	test_trace_branch(cpu);
}

OP(movec_from) {
	uint16_t nw = cpu->fetch_w();
	cpu->R[ nw >> 12 ] = cpu->mmu->movec_from(nw & 0xfff);
	test_trace_branch(cpu);
}

OP(movec_to) {
	uint16_t nw = cpu->fetch_w();
	cpu->mmu->movec_to(nw & 0xfff, cpu->R[ nw >> 12 ]);
	test_trace_branch(cpu);
}

OP(exg_d) {
	uint32_t tp = cpu->D[reg];
	cpu->D[reg] = cpu->D[dn];
	cpu->D[dn] = tp;
}

OP(exg_a) {
	uint32_t tp = cpu->A[reg];
	cpu->A[reg] = cpu->A[dn];
	cpu->A[dn] = tp;
}

OP(exg_da) {
	uint32_t tp = cpu->A[reg];
	cpu->A[reg] = cpu->D[dn];
	cpu->D[dn] = tp;
}
OP(move16_inc_imm) {
	uint32_t imm = cpu->fetch_l();
	uint32_t adr = cpu->A[reg];
	for(int i = 0; i < 16; ++i ) {
		cpu->mmu->write_b(adr+i, cpu->mmu->read_b(imm+i));
	}
	cpu->A[reg] += 16;
}
OP(move16_imm_r) {
	uint32_t imm = cpu->fetch_l();
	uint32_t adr = cpu->A[reg];
	for(int i = 0; i < 16; ++i ) {
		cpu->mmu->write_b(imm+i, cpu->mmu->read_b(adr+i));
	}
}
OP(move16_r_imm) {
	uint32_t imm = cpu->fetch_l();
	uint32_t adr = cpu->A[reg];
	for(int i = 0; i < 16; ++i ) {
		cpu->mmu->write_b(adr+i, cpu->mmu->read_b(imm+i));
	}
}
OP(move16_imm_inc) {
	uint32_t imm = cpu->fetch_l();
	uint32_t adr = cpu->A[reg];
	for(int i = 0; i < 16; ++i ) {
		cpu->mmu->write_b(imm+i, cpu->mmu->read_b(adr+i));
	}
	cpu->A[reg] += 16;
}

OP(move16_inc_inc) {
	uint32_t imm = cpu->fetch_l();
	uint32_t adrr = (imm >> 12) & 7;
	uint32_t adr = cpu->A[adr];
	uint32_t adr2 = cpu->A[reg];
	for(int i = 0; i < 16; ++i ) {
		cpu->mmu->write_b(adr+i, cpu->mmu->read_b(adr2+i));
	}
	cpu->A[adrr] += 16;
	cpu->A[reg] += 16;
}
