#include <stdint.h>
#include "registers.hpp"
#include "common.hpp"
#include "inst_cmn.hpp"
#define OP(name_) extern "C" void op_##name_(uint16_t op , int dn , int mode , int reg )
#include "op.h"
#include "intel.h"

OP(moves_b) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	uint16_t nw = fetch_w();
	cpu.parse_EA( mode, 1, reg, true);
	uint32_t addr = cpu.ea_v;
	if( (nw >> 11) & 1 ) {
		// to EA		
		uint8_t v = cpu.R[(nw>>12)];
		int attr = ATTR_FC(cpu.DFC)|ATTR_SZ_B;
		bus_write(mmu->to_real(addr, attr), attr, &v);
	} else {
		// from EA
		uint8_t v;
		int attr = ATTR_FC(cpu.SFC)|ATTR_SZ_B;
		bus_read(mmu->to_real(addr, attr), attr, &v);
		cpu.R[(nw>>12)] = v;
	}
	test_trace_branch();
}

OP(moves_w) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	uint16_t nw = fetch_w();
	cpu.parse_EA( mode, 2, reg, true);
	uint32_t addr = cpu.ea_v;
	if( (nw >> 11) & 1 ) {
		// to EA
		uint16_t v = cpu.R[(nw>>12)];
		int attr = ATTR_FC(cpu.DFC)|ATTR_SZ_W;
		bus_write(mmu->to_real(addr, attr), attr, &v);
	} else {
		// from EA
		uint16_t v;
		int attr = ATTR_FC(cpu.SFC)|ATTR_SZ_W;
		bus_read(mmu->to_real(addr, attr), attr, &v);
		cpu.R[(nw>>12)] = v;
	}
	test_trace_branch();
}
OP(moves_l) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	uint16_t nw = fetch_w();
	cpu.parse_EA( mode, 4, reg, true);
	uint32_t addr = cpu.ea_v;
	if( (nw >> 11) & 1 ) {
		// to EA
		uint32_t v = cpu.R[(nw>>12)];
		int attr = ATTR_FC(cpu.DFC)|ATTR_SZ_L;
		bus_write(mmu->to_real(addr, attr), attr, &v);
	} else {
		// from EA
		uint32_t v;
		int attr = ATTR_FC(cpu.SFC)|ATTR_SZ_L;
		bus_read(mmu->to_real(addr, attr), attr, &v);
		cpu.R[(nw>>12)] = v;
	}
	test_trace_branch();
}

OP(movep_m2r_w) {
	int16_t disp = fetch_w();
	uint32_t addr = cpu.R[8+reg] +disp;
	cpu.R[dn] = read_b(addr) << 8;
	cpu.R[dn] |= read_b(addr+2);
}

OP(movep_r2m_w) {
	int16_t disp = fetch_w();
	uint32_t addr = cpu.R[8+reg] +disp;
	write_b(addr, (cpu.R[dn] >> 8) & 0xff);
	write_b(addr + 2, cpu.R[dn] & 0xff);
}

OP(movep_m2r_l) {
	int16_t disp = fetch_w();
	uint32_t addr = cpu.R[8+reg] +disp;
	cpu.R[dn] = read_b(addr) << 24;
	cpu.R[dn] |= read_b(addr+2) << 16;
	cpu.R[dn] |= read_b(addr+4) << 8;
	cpu.R[dn] |= read_b(addr+6);
}

OP(movep_r2m_l) {
	int16_t disp = fetch_w();
	uint32_t addr = cpu.R[8+reg] +disp;
	uint32_t v = cpu.R[dn];
	write_b(addr, (v >> 24) & 0xff);
	write_b(addr + 2, (v >> 16) & 0xff);
	write_b(addr + 4, (v >> 8) & 0xff);
	write_b(addr + 6, v & 0xff);
}

OP(move_b) {
	cpu.parse_EA( mode, 1, reg, false);
	int8_t v = cpu.ea_read();
	cpu.parse_EA( (op>>6)&7, 1, dn, true);
	cpu.ea_write(v);
	set_nz( v);
	cpu.V = cpu.C = false;	
}
OP(move_w) {
	cpu.parse_EA( mode, 2, reg, false);
	int16_t v = cpu.ea_read();
	cpu.parse_EA( (op>>6)&7, 2, dn, true);
	cpu.ea_write(v);
	set_nz( v);
	cpu.V = cpu.C = false;	
}
OP(move_l) {
	cpu.parse_EA( mode, 4, reg, false);
	int32_t v = cpu.ea_read();
	cpu.parse_EA( (op>>6)&7, 4, dn, true);
	cpu.ea_write(v);
	set_nz( v);
	cpu.V = cpu.C = false;	
}
OP(movea_w) {
	cpu.parse_EA( mode, 2, reg, false);
	int16_t v = cpu.ea_read();
	cpu.R[8+ dn ] = v;
}
OP(movea_l) {
	cpu.parse_EA( mode, 4, reg, false);
	cpu.R[8+ dn ] = cpu.ea_read();
}



OP(move_from_sr) { 
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( get_sr() );
}

OP(move_from_cr) {
	cpu.parse_EA( mode, 2, reg, true);
	cpu.ea_write( get_cr() );
}

OP(move_to_cr) {
	cpu.parse_EA( mode, 2, reg, false);
	set_cr(  cpu.ea_read());
}


OP(move_to_sr) { 
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	cpu.parse_EA( mode, 2, reg, false);
	set_sr(  cpu.ea_read() );
	test_trace_branch();
}

OP(movem_w_to_decr) {
	uint16_t mask = fetch_w();
	if( ! cpu.restart ) {
		cpu.ea_v = cpu.R[8+reg];		
	}
	uint32_t addr = cpu.ea_v;
	try {
		for(int i = 0; i < 16 ; ++i ) {
			if( mask & (1 << i) ) {
				write_w( addr -= 2, cpu.R[15-i]);
			}
		}
	} catch( Exception::AccessFault& e) {
		e.ssw |= 1 << 13; // CM
		throw e;
	}
	cpu.R[8+reg] = addr;
	cpu.restart = false;
}

OP(movem_w_to) {
	uint16_t mask = fetch_w();
	if( ! cpu.restart ) {
		cpu.parse_EA( mode, 2, reg, false);
	}
	uint32_t addr = cpu.ea_v;
	try {
		for(int i = 0; i < 16; ++i ) {
			if( mask & (1 << i) ) {
				write_w( addr, cpu.R[i]);
				addr += 2;
			}
		}
	} catch( Exception::AccessFault& e) {
		e.ssw |= 1 << 13; // CM
		throw e;
	}
	cpu.restart = false;
}


OP(movem_l_to_decr) {
	uint16_t mask = fetch_w();
	if( ! cpu.restart ) {
		cpu.ea_v = cpu.R[8+reg];
	}
	uint32_t addr = cpu.ea_v;
	try {
		for(int i = 0; i < 16 ; ++i ) {
			if( mask & (1 << i) ) {
				write_l( addr -= 4, cpu.R[15-i]);
			}
		}		
		cpu.R[8+reg] = addr;
		cpu.restart = false;
	} catch( Exception::AccessFault& e) {
		e.ssw |= 1 << 13; // CM
		throw e;
	}
}

OP(movem_l_to) {
	uint16_t mask = fetch_w();
	if( ! cpu.restart ) {
		cpu.parse_EA( mode, 4, reg, false);
	}
	uint32_t addr = cpu.ea_v;
	try {
		for(int i = 0; i < 16; ++i ) {
			if( mask & (1 << i) ) {
				write_l( addr, cpu.R[i]);
				addr += 4;
			}
		}
		cpu.restart = false;
	} catch( Exception::AccessFault& e) {
		e.ssw |= 1 << 13; // CM
		throw e;
	}
}



OP(movem_w_from_incr) {
	uint16_t mask = fetch_w();
	if( ! cpu.restart ) {
		cpu.ea_v = cpu.R[8+reg];
	}
	uint32_t addr = cpu.ea_v;
	try {
		for(int i = 0; i < 16; ++i ) {
			if( mask & (1 << i) ) {
				cpu.R[i] = read_w( addr);
				addr += 2;
			}
		}
		cpu.R[8+reg] = addr;
	} catch( Exception::AccessFault& e) {
		e.ssw |= 1 << 13; // CM
		throw e;
	}
}

OP(movem_w_from) {
	uint16_t mask = fetch_w();
	if( ! cpu.restart ) {
		cpu.parse_EA( mode, 2, reg, false);
	}
	uint32_t addr = cpu.ea_v;
	try {
		for(int i = 0; i < 16; ++i ) {
			if( mask & (1 << i) ) {
				cpu.R[i] = read_w( addr);
				addr += 2;
			}
		}
		cpu.restart = false;
	} catch( Exception::AccessFault& e) {
		e.ssw |= 1 << 13; // CM
		throw e;
	}
}

OP(movem_l_from_incr) {
	uint16_t mask = fetch_w();
	if( ! cpu.restart ) {
		cpu.ea_v = cpu.R[8+reg];
	}
	uint32_t addr = cpu.ea_v;
	try {
		for(int i = 0; i < 16; ++i ) {
			if( mask & (1 << i) ) {
				cpu.R[i] = read_l( addr);
				addr += 4;
			}
		}
		cpu.R[8+reg] = addr;
		cpu.restart = false;
	} catch( Exception::AccessFault& e) {
		e.ssw |= 1 << 13; // CM
		throw e;
	}
}

OP(movem_l_from) {
	uint16_t mask = fetch_w();
	if( ! cpu.restart ) {
		cpu.parse_EA( mode, 4, reg, false);
	}
	uint32_t addr = cpu.ea_v;
	try {
		for(int i = 0; i < 16; ++i ) {
			if( mask & (1 << i) ) {
				cpu.R[i] = read_l( addr);
				addr += 4;
			}
		}
		cpu.restart = false;
	} catch( Exception::AccessFault& e) {
		e.ssw |= 1 << 13; // CM
		throw e;
	}
}

OP(move_to_usp) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	cpu.USP = cpu.R[8+ reg ];
	test_trace_branch();
}

OP(move_from_usp) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	cpu.R[8+ reg ] = cpu.USP;
	test_trace_branch();
}
void m68k_reset (CPU* cpu);
void emul_reset(uint32_t* d, uint32_t* a);

OP(reset) {
	emul_reset(cpu.R, cpu.R+8);
}

OP( nop ) {
	test_trace_branch();
}
extern bool quit_program;

OP(stop) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	uint16_t v = fetch_w();
	set_sr( v);
	test_trace_branch();
	quit_program = true;
}

OP(rte) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	cpu.return_from_exception();
	test_trace_branch();
}

OP(rtd) {
	int16_t disp = fetch_w();
	uint32_t ret = pop32();
	cpu.R[8+7] += disp;
	cpu.change_PC(ret);
}

OP(rts) {
	cpu.change_PC(pop32());
}

OP(trapv) {
	if( cpu.V ) {
		throw Exception::TrapCC();
	}
}

OP(rtr) {
	uint8_t cr = pop16();
	set_cr( cr);
	cpu.change_PC(pop32());
}

OP(movec_from) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	uint16_t nw = fetch_w();
	cpu.R[ nw >> 12 ] = mmu->movec_from(nw & 0xfff);
	test_trace_branch();
}

OP(movec_to) {
	if( ! cpu.S ) {
		throw Exception::PrivilegeViolation();
	}
	uint16_t nw = fetch_w();
	mmu->movec_to(nw & 0xfff, cpu.R[ nw >> 12 ]);
	test_trace_branch();
}

OP(exg_d) {
	uint32_t tp = cpu.R[reg];
	cpu.R[reg] = cpu.R[dn];
	cpu.R[dn] = tp;
}

OP(exg_a) {
	uint32_t tp = cpu.R[8+reg];
	cpu.R[8+reg] = cpu.R[8+dn];
	cpu.R[8+dn] = tp;
}

OP(exg_da) {
	uint32_t tp = cpu.R[8+reg];
	cpu.R[8+reg] = cpu.R[dn];
	cpu.R[dn] = tp;
}
OP(move16_inc_imm) {
	uint16_t imm = fetch_w();
	uint8_t buf[16];
	read_16(imm, buf);
	write_16(cpu.R[8+reg], buf);
	cpu.R[8+reg] += 16;
}
OP(move16_imm_r) {
	uint16_t imm = fetch_w();
	uint8_t buf[16];
	read_16(cpu.R[8+reg], buf);
	write_16(imm, buf);
}
OP(move16_r_imm) {
	uint16_t imm = fetch_w();
	uint8_t buf[16];
	read_16(imm, buf);
	write_16(cpu.R[8+reg], buf);
}
OP(move16_imm_inc) {
	uint16_t imm = fetch_w();
	uint8_t buf[16];
	read_16(cpu.R[8+reg], buf);
	write_16(imm, buf);
	cpu.R[8+reg] += 16;
}

OP(move16_inc_inc) {
	uint16_t imm = fetch_w();
	uint32_t adrr = (imm >> 12) & 7;
	uint8_t buf[16];
	read_16(cpu.R[8+reg], buf);
	write_16(cpu.R[8+adrr], buf);
	cpu.R[8+adrr] += 16;
	cpu.R[8+reg] += 16;
}
