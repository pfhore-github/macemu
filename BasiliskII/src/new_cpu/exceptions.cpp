#include "registers.hpp"
#include "common.hpp"
#include "inst_cmn.hpp"
#include <functional>
namespace Exception {
void Base::exec() {
	uint16_t sr = get_sr();
	save_sp();
	cpu.S = true;
	cpu.T = 0;
	load_sp();
	uint32_t nextaddr = read_l(cpu.VBR+(vec<<2));
	if( nextaddr & 1 ) {
		throw "double fault...";
	}
	save_stack(sr);
	cpu.PC = nextaddr;
}
AccessFault::AccessFault(uint32_t addr, uint16_t ssw, bool is_inst)
	:Base(2, is_inst ? 7 : 1), addr(addr), ssw(ssw) {}

AddressError::AddressError(uint32_t addr)
	:Format2(3, addr-1, cpu.NPC, 5) {}

IllegalInst::IllegalInst()
	:Format0(4, cpu.NPC) {
	cpu.trace = false;
}

DivBy0::DivBy0()
	:Format2(5, cpu.NPC, cpu.PC) {}

ChkError::ChkError()
	:Format2(6, cpu.NPC, cpu.PC) {}

TrapCC::TrapCC()
	:Format2(7, cpu.NPC, cpu.PC) {}

PrivilegeViolation::PrivilegeViolation()
	:Format0(8, cpu.NPC) {}

Trace::Trace()
	:Format2(9, cpu.NPC, cpu.PC, 6) {}

ALine::ALine()
	:Format0(10, cpu.NPC) {}

FLine::FLine()
	:Format0(11, cpu.NPC) {}

FormatError::FormatError()
	:Format0(14, cpu.NPC) {}

Trap::Trap(int i)
	:Format0(32+i, cpu.PC) {}

FP_unimplemented_inst::FP_unimplemented_inst()
	:Format2(11, cpu.ea_v, cpu.PC, 3) {
	cpu.trace = false;
}

FP_BSUN::FP_BSUN()
	:Format0(48, cpu.PC) {}

FP_INEXACT::FP_INEXACT()
	:Format0(49, cpu.PC) {}

FP_DIV_BY_0::FP_DIV_BY_0() 
	:Format0(50, cpu.PC) {}

FP_UNDERFLOW::FP_UNDERFLOW()
	:Format0(51, cpu.PC) {}

FP_OPERR::FP_OPERR()
	:Format0(52, cpu.PC) {}

FP_OVERFLOW::FP_OVERFLOW()
	:Format0(53, cpu.PC) {}

FP_SNAN::FP_SNAN()
	:Format0(54, cpu.PC) {}

FP_unimplemented_type_pre::FP_unimplemented_type_pre()
	:Format0(55, cpu.PC) {}

FP_unimplemented_type_post::FP_unimplemented_type_post()
	:Format3(55, cpu.PC) {}

void Format0::save_stack(uint16_t sr) {
	push16(vec << 2);
	push32(nextpc);
	push16(sr);		
}

void Format2::save_stack(uint16_t sr) {
	push32(error_address);
	push16(2 << 12 | vec << 2);
	push32(nextpc);
	push16(sr);		
}
void Format3::save_stack(uint16_t sr) {
	push32(cpu.ea_v);
	push16(3 << 12 | vec << 2);
	push32(cpu.PC);
	push16(sr);		
}


void AccessFault::save_stack(uint16_t sr) {
	cpu.R[15] += 4*9;
	push32( addr);
	cpu.R[15] += 2*3;
	push16( ssw );
	push32( cpu.ea_v);
	push16( 7 << 12 | vec << 2);
	push32( cpu.NPC);
	push16( sr);
}
}
void CPU::return_from_exception() {
	uint16_t sr = pop16();
	uint32_t pc = pop32();
	uint16_t vx = pop16();
	switch( vx >> 12 ) {
	case 0 :
		break;
	case 1 :
		set_sr(sr);
		return return_from_exception();
	case 2 :
	case 3 :
		pop32();
		break;
	case 7 : {
		uint32_t ea = pop32();
		uint16_t ssw = pop16();
		cpu.R[15] += 2*3;
		cpu.R[15] += 4*10;
		if( ssw & SSW_CP ) {
			set_sr(sr);
			cpu.PC = pc;
			throw Exception::FP_unimplemented_type_post();
		}
		if( ssw & SSW_CU ) {
			set_sr(sr);
			cpu.PC = pc;
			throw Exception::FP_unimplemented_inst();
		}
		if( ssw & SSW_CT) {
			set_sr(sr);
			cpu.PC = pc;
			throw Exception::Trace();
		}
		if( ssw & SSW_CM ) {
			set_sr(sr);
			cpu.PC = pc;
			cpu.ea_v = ea;
			cpu.restart = true;
		}
		break;		
	}
	default:
		throw Exception::FormatError();
	}
	set_sr(sr);
	cpu.in_ex = false;
	cpu.change_PC(pc);
}
