#include "registers.hpp"
#include "common.hpp"
#include "exceptions.hpp"
#include "inst_cmn.hpp"
void exception_t::run(CPU* cpu, uint32_t pc) {
	sr = get_sr(cpu);
	save_sp(cpu);
	cpu->S = true;
	cpu->T = 0;
	load_sp(cpu);
	uint32_t nextaddr = cpu->mmu->read_l(cpu->VBR+v);
	save_stack(cpu, pc);
	cpu->PC = nextaddr;
}

void exception_t::save_stack0(CPU* cpu, uint32_t pc) {
	cpu->push16(v);
	cpu->push32( pc);
	cpu->push16( sr);
}


void exception_t::save_stack1(CPU* cpu, uint32_t pc) {
	cpu->push16( 1 << 12 | v);
	cpu->push32( pc);
	cpu->push16( sr);
}

void exception_t::save_stack2(CPU* cpu, uint32_t pc, uint32_t addr) {
	cpu->push32( addr);
	cpu->push16( 2 << 12 | v);
	cpu->push32( pc);
	cpu->push16( sr);
}

void exception_t::save_stack3(CPU* cpu, uint32_t ea) {
	cpu->push32( ea);
	cpu->push16( 3 << 12 | v);
	cpu->push32( cpu->PC);
	cpu->push16( sr);
}

void exception_t::save_stack4(CPU* cpu, uint32_t pc, uint32_t ea) {
	cpu->push32( pc);
	cpu->push32( ea);
	cpu->push16( 4 << 12 | v);
	cpu->push32( cpu->PC);
	cpu->push16( sr);
}

void Interrupt::save_stack(CPU* cpu, uint32_t) {
	if( cpu->M << 12 ) {
		save_stack1(cpu, cpu->PC);
	} else {
		save_stack0(cpu, cpu->PC);
	}
}

void FormatError::save_stack(CPU* cpu, uint32_t pc) {
	save_stack0(cpu, pc);
}

void TRAP_N::save_stack(CPU* cpu, uint32_t) {
	save_stack0(cpu, cpu->PC);
}

void IllegalInstruction::save_stack(CPU* cpu, uint32_t pc) {
	save_stack0(cpu, pc);
}


void AInstruction::save_stack(CPU* cpu, uint32_t pc) {
	save_stack0(cpu, pc);
}

void FInstruction::save_stack(CPU* cpu, uint32_t pc) {
	save_stack0(cpu, pc);
}

void PrivViolation::save_stack(CPU* cpu, uint32_t pc) {
	save_stack0(cpu, pc);
}

void FpuPreInst::save_stack(CPU* cpu, uint32_t pc) {
	save_stack0(cpu, pc);
}
void ChkTrap::save_stack(CPU* cpu, uint32_t pc) {
	save_stack2(cpu, cpu->PC, pc);
}
void Trapcc::save_stack(CPU* cpu, uint32_t pc) {
	save_stack2(cpu, cpu->PC, pc);
}
void DivByZero::save_stack(CPU* cpu, uint32_t pc) {
	save_stack2(cpu, cpu->PC, pc);
}

void TraceEx::save_stack(CPU* cpu, uint32_t pc) {
	save_stack2(cpu, cpu->PC, pc);
}
void AddressError::save_stack(CPU* cpu, uint32_t pc) {
	save_stack2(cpu, cpu->PC, ea);
}

void UnimplementedFp::save_stack(CPU* cpu, uint32_t pc) {
	save_stack2(cpu, cpu->PC, ea);
}

void AccessFault::save_stack(CPU* cpu, uint32_t pc) {
	// cache area
	cpu->push32( 0);
	cpu->push32( 0);
	cpu->push32( 0);
	cpu->push32( 0);
	cpu->push32( 0);
	cpu->push32( 0);
	cpu->push32( 0);
	cpu->push32( 0);
	cpu->push32( 0);
	cpu->push32( fa);
	cpu->push16( 0);
	cpu->push16( 0);
	cpu->push16( 0);
	cpu->push16( CP << 15 |
		   CU << 14 |
		   CT << 13 |
		   CM << 12 |
		   MA << 11 |
		   ATC << 10 |
		   LK << 9 |
		   RW <<8 |
		   SIZE <<5 |
		   TT << 3 |
		   TM );
	cpu->push32( ea);
	cpu->push16( 7 << 12 );
	cpu->push32( cpu->PC);
	cpu->push16( sr);
}
