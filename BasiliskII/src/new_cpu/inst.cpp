#include "registers.hpp"
#include "common.hpp"
#include <functional>
#include "inst_cmn.hpp"
#include "../rom/data.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <pthread.h>
namespace ROM {
std::unordered_map<uint32_t, std::function<void()>> rom_routines;
}
#define OP(name_) extern "C" void op_##name_(uint16_t op, int dn, int mode, int reg)
#include "op.h"
void callm(uint32_t, int) {
	throw Exception::IllegalInst();
}
void rtm(uint32_t) {
	throw Exception::IllegalInst();
}


OP(illegal) {
	throw Exception::IllegalInst();
}

OP(rtm_d) {
	rtm(cpu.R[reg]);
}
OP(rtm_a) {
	rtm(cpu.R[8+reg]);
}

OP(callm) {
	int ac = fetch_w();
	cpu.parse_EA(mode, 1, reg, false);
	callm(cpu.ea_v, ac);
}




OP(clr_b) {
	cpu.parse_EA(mode, 1, reg, true);	
	cpu.N = cpu.V = cpu.C = false;
	cpu.Z = true;
	cpu.ea_write( 0 );
}

OP(clr_w) {
	cpu.parse_EA(mode, 2, reg, true);
	cpu.N = cpu.V = cpu.C = false;
	cpu.Z = true;
	cpu.ea_write( 0 );
}

OP(clr_l) {
	cpu.parse_EA(mode, 4, reg, true);
	cpu.N = cpu.V = cpu.C = false;
	cpu.Z = true;
	cpu.ea_write( 0 );
}


OP(link_l) {
	int32_t disp = fetch_l();
	push32(cpu.R[8+reg]);
	cpu.R[8+reg] = cpu.R[15];
	cpu.R[15] += disp;
}



OP(bkpt) {
	int vc = reg;
	(void)vc;
	throw Exception::IllegalInst();
}

OP(pea) {
	cpu.parse_EA(mode, 4, reg, false);
	push32(cpu.ea_v);
}


OP(trap) {
	uint8_t v = op & 15;
	throw Exception::Trap(v);
}

OP(link_w) {
	int16_t offset = fetch_w();
	push32(cpu.R[8+reg]);
	cpu.R[8+reg] = cpu.R[15];
	cpu.R[15] += offset;	
}

OP(unlk) {
	cpu.R[15] = cpu.R[8+reg];
	uint32_t v = pop32();
	cpu.R[8+reg] = v;
}



OP(jsr) {
	push32(cpu.PC);
	cpu.parse_EA(mode, 2, reg, false);
	cpu.change_PC(cpu.ea_v);
}

OP(jmp) {
	cpu.parse_EA(mode, 2, reg, false);
	cpu.change_PC(cpu.ea_v);
}

OP(chk_l) {
	cpu.parse_EA(mode, 4, reg, false);
	int32_t mx = cpu.ea_read();
	int32_t v = cpu.R[dn];
	if( v < 0 ) {
		cpu.N = true;
		throw Exception::ChkError();
	} else if( v > mx ) {
		cpu.N = false;
		throw Exception::ChkError();
	}
}

OP(chk_w) {
	cpu.parse_EA(mode, 2, reg, false);
	int16_t mx = cpu.ea_read();
	int16_t v = cpu.R[dn];
	if( v < 0 ) {
		cpu.N = true;
		throw Exception::ChkError();
	} else if( v > mx ) {
		cpu.N = false;
		throw Exception::ChkError();
	}
}


OP(lea) {
	cpu.parse_EA(mode, 4, reg, false);
	uint32_t addr = cpu.ea_v;
	cpu.R[ 8+dn ] = addr;
}
#pragma GCC diagnostic pop

bool get_cond(int cd) {
	switch( cd ) {
	case 0 : return true;
	case 1 : return false;
	case 2 : return ! ( cpu.C || cpu.Z );
	case 3 : return ( cpu.C || cpu.Z );
	case 4 : return ! cpu.C;
	case 5 : return cpu.C; 
	case 6 : return ! cpu.Z;
	case 7 : return cpu.Z;
	case 8 : return ! cpu.V; 
	case 9 : return cpu.V; 
	case 10 : return ! cpu.N;
	case 11 : return cpu.N;
	case 12 : return cpu.N == cpu.V;
	case 13 : return cpu.N != cpu.V;
	case 14 : return cpu.N == cpu.V && ! cpu.Z;
	case 15 : return cpu.Z || cpu.N != cpu.V;		
	}
	return false;
}

OP(scc) {
	cpu.parse_EA(mode, 1, reg, true);
	cpu.ea_write( get_cond((op >> 8) & 15 ) ? 0xff : 0 );
}

OP(dbcc) {
	uint32_t now = cpu.PC;
	int16_t disp = fetch_w();
	int16_t u = cpu.R[ reg ];
	cpu.setD_W(reg, --u);
	if( ! get_cond((op >> 8) & 15 ) && u != -1 ) {
		cpu.change_PC(now+disp);
	}
}

OP(trapcc) {
	if( get_cond((op>>8) & 15)) {
		throw Exception::TrapCC();
	}
}
OP(trapcc_w) {
	cpu.PC += 2;
	if( get_cond((op>>8) & 15)) {
		throw Exception::TrapCC();
	}
}
OP(trapcc_l) {
	cpu.PC += 4;
	if( get_cond((op>>8) & 15)) {
		throw Exception::TrapCC();
	}
}
OP(bra) {
	int8_t disp = op & 0xff;
	uint32_t now = cpu.PC;
	if( disp == 0 ) {
		int16_t d16 = fetch_w();
		cpu.change_PC(now+d16);
	} else if( disp == -1 ) {
		int32_t d32 = fetch_l();
		cpu.change_PC(now+d32);
	} else {
		cpu.change_PC(now+disp);
	}
}
OP(bsr) {
	int8_t disp = op & 0xff;
	uint32_t now = cpu.PC;
	if( disp == 0 ) {
		int16_t d16 = fetch_w();
		push32(cpu.PC);
		cpu.change_PC(now+d16);
	} else if( disp == -1 ) {
		int32_t d32 = fetch_l();
		push32(cpu.PC);
		cpu.change_PC(now+d32);
	} else {
		push32(cpu.PC);
		cpu.change_PC(now+disp);
	}
}
OP(bcc) {
	int8_t disp = op & 0xff;
	uint32_t now = cpu.PC;
	if( disp == 0 ) {
		int16_t d16 = fetch_w();
		if( get_cond((op >> 8) & 15 ) ) {
			cpu.change_PC(now+d16);
		}
	} else if( disp == -1 ) {
		int32_t d32 = fetch_l();
		if( get_cond((op >> 8) & 15 ) ) {
			cpu.change_PC(now+d32);
		}
	} else {
		if( get_cond((op >> 8) & 15 ) ) {
			cpu.change_PC(now+disp);
		}
	}
}
OP(moveq) {
	int8_t v = op & 0xff;
	set_nz(v);
	cpu.V = cpu.C = false;
	cpu.R[dn] = v;
}
OP(fpu) {
	fpu->exec(op, mode, reg);
}
OP(mmu) {
	if( ! mmu ){
		throw Exception::FLine();
	} else {
		mmu->exec(op, mode, reg);
	}
}
struct M68kRegisters {
	uint32_t d[8];
	uint32_t a[8];
	uint16_t sr;
};

void EmulOp(uint16_t opcode, M68kRegisters *r);
OP(emul) {
	M68kRegisters r;
	memcpy(&r, cpu.R, sizeof(uint32_t)*16);
	r.sr = get_sr();
	EmulOp(op, &r);
	memcpy(cpu.R, &r, sizeof(uint32_t)*16);
	set_sr(r.sr);	
}
bool quit_program;
extern uint32_t ROMBaseMac;
using op_type = void (*)(uint16_t op, int dn, int mode, int reg);
op_type op_list[65536];


void CPU::reset ()
{
	S = true;
	M = false;
	restart = false;
	T = 0;
	IX = 0;
	intmask = 7;
	VBR = 0;
	in_ex = false;
	if(mmu)
		mmu->reset();
	R[15] = read_l(0);
	PC = read_l(4);
}

void CPU::irq(int level) {
	IX = level;
}

void CPU::do_irq() {
	uint16_t sr = get_sr();
	if( intmask > IX )
		return ;
	S = true;
	T = 0;
	intmask = IX;
	uint32_t nextaddr = read_l(VBR+(IX << 2));
	if( M ) {
		M = 0;
		MSP = R[15];
		R[15] = ISP;
		push16( 1 << 12 | IX << 2);
	} else {
		push16( IX << 2);
	}
	push32( PC);
	push16( sr);
	IX = 0;
	NPC = nextaddr;
	return ;
}
void CPU::dump() {
	printf("%08x:D:(", NPC);
	for(int i = 0; i < 8; ++i ) {
		printf("%x,",R[i]);
	}
	printf("), A:(");
	for(int i = 0; i < 6; ++i ) {
		printf("%x,",R[8+i]);
	}
	printf("),LR=%x,SP=%x;", R[14],R[15]);
	if(Z) { printf("Z,"); }
	if(N) { printf("N,"); }
	if(C) { printf("C,"); }
	if(V) { printf("V,"); }
	if(S) { printf("S,"); }
	printf("\n");
}
bool dump_ = true;
void CPU::change_PC(uint32_t new_pc) {
	if( new_pc & 1 ) {
		throw Exception::AddressError(new_pc);
	}
	auto routine = ROM::rom_routines.find(new_pc & 0xfffff);
	if( routine != ROM::rom_routines.end() ) {
		routine->second();
	} else {
		PC = new_pc;
		if( T == 1 ) {
			trace = true;
		}
	}
}
void CPU::do_op() {
	if( IX )
		do_irq();
	NPC = PC;
	try {
		uint16_t op = fetch_w();
		if( dump_ ) {
			dump();
		}
		if( T == 2 ) {
			trace = true;
		}
		op_list[op](op, (op>>9)&7, (op>>3)&7, op&7);
	} catch( Exception::AccessFault& e ) {
		if( trace ) {
			// Trace pending
			e.ssw |= 1 >> 13;
			trace = false;
		}
		e.exec();
	} catch( Exception::Base& e) {
		if( e.priority == 3 || e.priority == 4 ) {
			trace = false;
		}
		e.exec();
	}
	if( trace ) {
		trace = false;
		Exception::Trace().exec();
	}
}


void CPU::init() {
	// init run table
	for(int i = 0; i < 65535; ++i ) {
		op_list[i] = op_illegal;
	}
	for(int i = 0; i < 8; ++i ) {
		// DN
		op_list[ 000000 | i ] = op_ori_b;    // 0 000 000 000 000 ddd
		op_list[ 000100 | i ] = op_ori_w;    // 0 000 000 001 000 ddd
		op_list[ 000200 | i ] = op_ori_l;    // 0 000 000 010 000 ddd
		
		op_list[ 001000 | i ] = op_andi_b;   // 0 000 001 000 000 ddd
		op_list[ 001100 | i ] = op_andi_w;   // 0 000 001 001 000 ddd
		op_list[ 001200 | i ] = op_andi_l;   // 0 000 001 010 000 ddd

		op_list[ 002000 | i ] = op_subi_b;   // 0 000 010 000 000 ddd
		op_list[ 002100 | i ] = op_subi_w;   // 0 000 010 001 000 ddd
		op_list[ 002200 | i ] = op_subi_l;   // 0 000 010 010 000 ddd

		op_list[ 003000 | i ] = op_addi_b;   // 0 000 011 000 000 ddd
		op_list[ 003100 | i ] = op_addi_w;   // 0 000 011 001 000 ddd
		op_list[ 003200 | i ] = op_addi_l;   // 0 000 011 010 000 ddd

		op_list[ 003300 | i ] = op_rtm_d;    // 0 000 011 011 000 ddd 
		op_list[ 004000 | i ] = op_btst_i_l; // 0 000 100 000 000 ddd
		op_list[ 004100 | i ] = op_bchg_i_l; // 0 000 100 001 000 ddd
		op_list[ 004200 | i ] = op_bclr_i_l; // 0 000 100 010 000 ddd
		op_list[ 004300 | i ] = op_bset_i_l; // 0 000 100 011 000 ddd

		op_list[ 005000 | i ] = op_eori_b;   // 0 000 101 000 000 ddd
		op_list[ 005100 | i ] = op_eori_w;	 // 0 000 101 001 000 ddd
		op_list[ 005200 | i ] = op_eori_l;	 // 0 000 101 010 000 ddd

		op_list[ 006000 | i ] = op_cmpi_b;   // 0 000 110 000 000 ddd
		op_list[ 006100 | i ] = op_cmpi_w;   // 0 000 110 001 000 ddd
		op_list[ 006200 | i ] = op_cmpi_l;   // 0 000 110 010 000 ddd

		for(int j = 0; j < 8; ++j ) {
			int nm_v = j << 9 | i;
			op_list[ 000400 | nm_v ] = op_btst_d_l; // 0 000 nnn 100 000 mmm
			op_list[ 000500 | nm_v ] = op_bchg_d_l; // 0 000 nnn 101 000 mmm
			op_list[ 000600 | nm_v ] = op_bclr_d_l; // 0 000 nnn 110 000 mmm
			op_list[ 000700 | nm_v ] = op_bset_d_l; // 0 000 nnn 111 000 mmm
			for(int k = 0; k < 8; ++k ) {
				if( k == 1 ) continue;
				// 0 001 nnn kkk 000 mmm
				op_list[ 010000 | nm_v | k << 6 ] = op_move_b;
				// 0 010 nnn kkk 000 mmm
				op_list[ 020000 | nm_v | k << 6 ] = op_move_l;
				// 0 011 nnn kkk 000 mmm
				op_list[ 030000 | nm_v | k << 6 ] = op_move_w;
			}			
			op_list[ 020100 | nm_v ] = op_movea_l; // 0 010 nnn 001 000 mmm
			op_list[ 030100 | nm_v ] = op_movea_w; // 0 011 nnn 001 000 mmm

			op_list[ 040400 | nm_v ] = op_chk_l;   // 0 100 nnn 100 000 mmm
			op_list[ 040600 | nm_v ] = op_chk_w;   // 0 100 nnn 110 000 mmm

			op_list[ 050000 | nm_v ] = op_addq_b;  // 0 101 xxx 000 000 mmm
			op_list[ 050100 | nm_v ] = op_addq_w;  // 0 101 xxx 001 000 mmm
			op_list[ 050200 | nm_v ] = op_addq_l;  // 0 101 xxx 010 000 mmm
			op_list[ 050400 | nm_v ] = op_subq_b;  // 0 101 xxx 100 000 mmm
			op_list[ 050500 | nm_v ] = op_subq_w;  // 0 101 xxx 101 000 mmm
			op_list[ 050600 | nm_v ] = op_subq_l;  // 0 101 xxx 110 000 mmm

			op_list[ 0100000 | nm_v ] = op_or_b;   // 1 000 nnn 000 000 mmm 
			op_list[ 0100100 | nm_v ] = op_or_w;   // 1 000 nnn 001 000 mmm 
			op_list[ 0100200 | nm_v ] = op_or_l;   // 1 000 nnn 010 000 mmm 
			op_list[ 0100300 | nm_v ] = op_divu_w; // 1 000 nnn 011 000 mmm 

			op_list[ 0100400 | nm_v ] = op_sbcd_d; // 1 000 nnn 100 000 mmm 
			op_list[ 0100500 | nm_v ] = op_pack_d; // 1 000 nnn 101 000 mmm 
			op_list[ 0100600 | nm_v ] = op_unpk_d; // 1 000 nnn 110 000 mmm 
			op_list[ 0100700 | nm_v ] = op_divs_w; // 1 000 nnn 111 000 mmm 

			op_list[ 0110000 | nm_v ] = op_sub_b;  // 1 001 nnn 000 000 mmm
			op_list[ 0110100 | nm_v ] = op_sub_w;  // 1 001 nnn 001 000 mmm
			op_list[ 0110200 | nm_v ] = op_sub_l;  // 1 001 nnn 010 000 mmm
			op_list[ 0110300 | nm_v ] = op_suba_w; // 1 001 nnn 011 000 mmm
			
			op_list[ 0110400 | nm_v ] = op_subx_b; // 1 001 nnn 100 000 mmm
			op_list[ 0110500 | nm_v ] = op_subx_w; // 1 001 nnn 101 000 mmm
			op_list[ 0110600 | nm_v ] = op_subx_l; // 1 001 nnn 110 000 mmm
			op_list[ 0110700 | nm_v ] = op_suba_l; // 1 001 nnn 111 000 mmm
			
			op_list[ 0130000 | nm_v ] = op_cmp_b;  // 1 011 nnn 000 000 mmm
			op_list[ 0130100 | nm_v ] = op_cmp_w;  // 1 011 nnn 001 000 mmm
			op_list[ 0130200 | nm_v ] = op_cmp_l;  // 1 011 nnn 010 000 mmm
			op_list[ 0130300 | nm_v ] = op_cmpa_w; // 1 011 nnn 011 000 mmm

			op_list[ 0130400 | nm_v ] = op_eor_b;  // 1 011 nnn 100 000 mmm 
			op_list[ 0130500 | nm_v ] = op_eor_w;  // 1 011 nnn 101 000 mmm 
			op_list[ 0130600 | nm_v ] = op_eor_l;  // 1 011 nnn 110 000 mmm 
			op_list[ 0130700 | nm_v ] = op_cmpa_l; // 1 011 nnn 111 000 mmm

			op_list[ 0140000 | nm_v ] = op_and_b;  // 1 100 nnn 000 000 mmm 
			op_list[ 0140100 | nm_v ] = op_and_w;  // 1 100 nnn 001 000 mmm 
			op_list[ 0140200 | nm_v ] = op_and_l;  // 1 100 nnn 010 000 mmm 
			op_list[ 0140300 | nm_v ] = op_mulu_w; // 1 100 nnn 011 000 mmm

			op_list[ 0140400 | nm_v ] = op_abcd_d; // 1 100 nnn 100 000 mmm 
			op_list[ 0140500 | nm_v ] = op_exg_d;  // 1 100 nnn 101 000 mmm 
			op_list[ 0140700 | nm_v ] = op_muls_w; // 1 100 nnn 111 000 mmm

			op_list[ 0150000 | nm_v ] = op_add_b;  // 1 101 nnn 000 000 mmm
			op_list[ 0150100 | nm_v ] = op_add_w;  // 1 101 nnn 001 000 mmm
			op_list[ 0150200 | nm_v ] = op_add_l;  // 1 101 nnn 010 000 mmm
			op_list[ 0150300 | nm_v ] = op_adda_w; // 1 101 nnn 011 000 mmm
			
			op_list[ 0150400 | nm_v ] = op_addx_b; // 1 101 nnn 100 000 mmm
			op_list[ 0150500 | nm_v ] = op_addx_w; // 1 101 nnn 101 000 mmm
			op_list[ 0150600 | nm_v ] = op_addx_l; // 1 101 nnn 110 000 mmm
			op_list[ 0150700 | nm_v ] = op_adda_l; // 1 101 nnn 111 000 mmm

		}
		op_list[ 010700 | i ] = op_move_b; // 0 001 000 111 000 mmm
		op_list[ 011700 | i ] = op_move_b; // 0 001 001 111 000 mmm
		op_list[ 020700 | i ] = op_move_l; // 0 010 000 111 000 mmm
		op_list[ 021700 | i ] = op_move_l; // 0 010 001 111 000 mmm
		op_list[ 030700 | i ] = op_move_w; // 0 011 000 111 000 mmm
		op_list[ 031700 | i ] = op_move_w; // 0 011 001 111 000 mmm
						
		op_list[ 040000 | i ] = op_negx_b; // 0 100 000 000 000 ddd
		op_list[ 040100 | i ] = op_negx_w; // 0 100 000 001 000 ddd
		op_list[ 040200 | i ] = op_negx_l; // 0 100 000 010 000 ddd
		op_list[ 040300 | i ] = op_move_from_sr; // 0 100 000 011 000 ddd
		op_list[ 041000 | i ] = op_clr_b;  // 0 100 001 000 000 ddd
		op_list[ 041100 | i ] = op_clr_w;  // 0 100 001 001 000 ddd
		op_list[ 041200 | i ] = op_clr_l;  // 0 100 001 010 000 ddd
		op_list[ 041300 | i ] = op_move_from_cr; // 0 100 001 011 000 ddd
		op_list[ 042000 | i ] = op_neg_b; // 0 100 010 000 000 ddd
		op_list[ 042100 | i ] = op_neg_w; // 0 100 010 001 000 ddd
		op_list[ 042200 | i ] = op_neg_l; // 0 100 010 010 000 ddd
		op_list[ 042300 | i ] = op_move_to_cr; // 0 100 010 011 000 ddd
		op_list[ 043000 | i ] = op_not_b; // 0 100 011 000 000 ddd
		op_list[ 043100 | i ] = op_not_w; // 0 100 011 001 000 ddd
		op_list[ 043200 | i ] = op_not_l; // 0 100 011 010 000 ddd
		op_list[ 043300 | i ] = op_move_to_sr; // 0 100 011 011 000 ddd

		op_list[ 044000 | i ] = op_nbcd_d; // 0 100 100 000 000 nnn
		op_list[ 044100 | i ] = op_swap;   // 0 100 100 001 000 nnn 
		op_list[ 044200 | i ] = op_ext_w;  // 0 100 100 010 000 nnn 
		op_list[ 044300 | i ] = op_ext_l;  // 0 100 100 011 000 nnn 

		op_list[ 044700 | i ] = op_extb_l; // 0 100 100 111 000 mmm

		op_list[ 045000 | i ] = op_tst_b; // 0 100 101 000 000 ddd
		op_list[ 045100 | i ] = op_tst_w; // 0 100 101 000 001 ddd
		op_list[ 045200 | i ] = op_tst_l; // 0 100 101 000 010 ddd
		op_list[ 045300 | i ] = op_tas;	  // 0 100 101 011 000 ddd

		op_list[ 046000 | i ] = op_mul_l; // 0 100 110 000 000 ddd
		op_list[ 046100 | i ] = op_div_l; // 0 100 110 001 000 ddd

		op_list[ 0164300 | i ] = op_bftst_d; // 1 110 100 011 000 ddd
		op_list[ 0165300 | i ] = op_bfchg_d; // 1 110 101 011 000 ddd
		op_list[ 0166300 | i ] = op_bfclr_d; // 1 110 110 011 000 ddd
		op_list[ 0167300 | i ] = op_bfset_d; // 1 110 111 011 000 ddd

		op_list[ 0164700 | i ] = op_bfextu_d;// 1 110 100 111 000 ddd
		op_list[ 0165700 | i ] = op_bfexts_d;// 1 110 101 111 000 ddd
		op_list[ 0166700 | i ] = op_bfffo_d; // 1 110 110 111 000 ddd
		op_list[ 0167700 | i ] = op_bfins_d; // 1 110 111 111 000 ddd

		for( int c = 0; c < 16; ++c ) {
			op_list[ 050300 | c << 8 | i ] = op_scc; // 0 101 CCC C11 000 ddd
		}

		// AN
		op_list[ 003310 | i ] = op_rtm_a; // 0 000 011 100 001 nnn
		for(int j = 0; j < 8; ++j ) {
			int nm_v = j << 9 | i;		
			op_list[ 0410 | nm_v ] = op_movep_m2r_w; // 0 000 nnn 100 001 mmm
			op_list[ 0510 | nm_v ] = op_movep_m2r_l; // 0 000 nnn 101 001 mmm
			op_list[ 0610 | nm_v ] = op_movep_r2m_w; // 0 000 nnn 110 001 mmm
			op_list[ 0710 | nm_v ] = op_movep_r2m_l; // 0 000 nnn 111 001 mmm
			for(int k = 0; k < 8; ++k ) {
				if( k == 1 ) continue;
				// 0 010 nnn kkk 001 mmm
				op_list[ 020010 | nm_v | k << 6 ] = op_move_l;
				// 0 011 nnn kkk 001 mmm
				op_list[ 030010 | nm_v | k << 6 ] = op_move_w;
			}			
			op_list[ 020110 | nm_v ] = op_movea_l; // 0 010 nnn 001 001 mmm
			op_list[ 030110 | nm_v ] = op_movea_w; // 0 011 nnn 001 001 mmm
			
			op_list[ 050110 | nm_v ] = op_addq_a;  // 0 101 xxx 001 001 mmm
			op_list[ 050210 | nm_v ] = op_addq_a;  // 0 101 xxx 010 001 mmm
			op_list[ 050510 | nm_v ] = op_subq_a;  // 0 101 xxx 101 001 mmm
			op_list[ 050610 | nm_v ] = op_subq_a;  // 0 101 xxx 110 001 mmm

			op_list[ 0100410 | nm_v ] = op_sbcd_m; // 1 000 nnn 100 001 mmm 
			op_list[ 0100510 | nm_v ] = op_pack_m; // 1 000 nnn 101 001 mmm 
			op_list[ 0100610 | nm_v ] = op_unpk_m; // 1 000 nnn 110 001 mmm 

			op_list[ 0110110 | nm_v ] = op_sub_w;  // 1 001 nnn 001 001 mmm
			op_list[ 0110210 | nm_v ] = op_sub_l;  // 1 001 nnn 010 001 mmm
			op_list[ 0110310 | nm_v ] = op_suba_w; // 1 001 nnn 011 001 mmm
			
			op_list[ 0110410 | nm_v ] = op_subx_m_b; // 1 001 nnn 100 001 mmm
			op_list[ 0110510 | nm_v ] = op_subx_m_w; // 1 001 nnn 101 001 mmm
			op_list[ 0110610 | nm_v ] = op_subx_m_l; // 1 001 nnn 110 001 mmm
			op_list[ 0110710 | nm_v ] = op_suba_l; // 1 001 nnn 111 001 mmm

			op_list[ 0130010 | nm_v ] = op_cmp_b;  // 1 011 nnn 000 001 mmm
			op_list[ 0130110 | nm_v ] = op_cmp_w;  // 1 011 nnn 001 001 mmm
			op_list[ 0130210 | nm_v ] = op_cmp_l;  // 1 011 nnn 010 001 mmm
			op_list[ 0130310 | nm_v ] = op_cmpa_w; // 1 011 nnn 011 001 mmm

			op_list[ 0130410 | nm_v ] = op_cmpm_b; // 1 011 nnn 100 001 mmm
			op_list[ 0130510 | nm_v ] = op_cmpm_w; // 1 011 nnn 101 001 mmm
			op_list[ 0130610 | nm_v ] = op_cmpm_l; // 1 011 nnn 110 001 mmm
			op_list[ 0130710 | nm_v ] = op_cmpa_l; // 1 011 nnn 111 001 mmm

			op_list[ 0140410 | nm_v ] = op_abcd_m;// 1 100 nnn 100 001 mmm 
			op_list[ 0140510 | nm_v ] = op_exg_a; // 1 100 nnn 101 001 mmm 
			op_list[ 0140610 | nm_v ] = op_exg_da;// 1 100 nnn 110 001 mmm 

			op_list[ 0150110 | nm_v ] = op_add_w;  // 1 101 nnn 001 001 mmm
			op_list[ 0150210 | nm_v ] = op_add_l;  // 1 101 nnn 010 001 mmm
			op_list[ 0150310 | nm_v ] = op_adda_w; // 1 101 nnn 011 001 mmm
			
			op_list[ 0150410 | nm_v ] = op_addx_m_b; // 1 101 nnn 100 001 mmm
			op_list[ 0150510 | nm_v ] = op_addx_m_w; // 1 101 nnn 101 001 mmm
			op_list[ 0150610 | nm_v ] = op_addx_m_l; // 1 101 nnn 110 001 mmm
			op_list[ 0150710 | nm_v ] = op_adda_l; // 1 101 nnn 111 001 mmm

		}
		
		op_list[ 020710 | i ] = op_move_l; // 0 010 000 111 001 mmm
		op_list[ 021710 | i ] = op_move_l; // 0 010 001 111 001 mmm
		op_list[ 030710 | i ] = op_move_w; // 0 011 000 111 001 mmm
		op_list[ 031710 | i ] = op_move_w; // 0 011 001 111 001 mmm

		op_list[ 044010 | i ] = op_link_l; // 0 100 100 000 001 nnn
		op_list[ 044110 | i ] = op_bkpt;   // 0 100 100 001 001 vvv
		op_list[ 047120 | i ] = op_link_w; // 0 100 111 001 010 mmm		

		for(int c = 0; c < 16; ++c ) {
			op_list[ 050310 | c << 8 | i ] = op_dbcc; // 0 101 ccc c11 001 nnn
		}


		// memory
		for(int j = 2; j < 8; ++j ) {
			uint32_t ea_v = j << 3 | i;
			op_list[ 000000 | ea_v ] = op_ori_b;  // 0 000 000 000 <EA>
			op_list[ 000100 | ea_v ] = op_ori_w;  // 0 000 000 001 <EA>
			op_list[ 000200 | ea_v ] = op_ori_l;  // 0 000 000 010 <EA>
			
			op_list[ 001000 | ea_v ] = op_andi_b; // 0 000 001 000 <EA>
			op_list[ 001100 | ea_v ] = op_andi_w; // 0 000 001 001 <EA>
			op_list[ 001200 | ea_v ] = op_andi_l; // 0 000 001 010 <EA>
			
			op_list[ 002000 | ea_v ] = op_subi_b; // 0 000 010 000 <EA>
			op_list[ 002100 | ea_v ] = op_subi_w; // 0 000 010 001 <EA>
			op_list[ 002200 | ea_v ] = op_subi_l; // 0 000 010 010 <EA>
						
			op_list[ 003000 | ea_v ] = op_addi_b; // 0 000 011 000 <EA>
			op_list[ 003100 | ea_v ] = op_addi_w; // 0 000 011 001 <EA>
			op_list[ 003200 | ea_v ] = op_addi_l; // 0 000 011 010 <EA>

			op_list[ 004000 | ea_v ] = op_btst_i_b; // 0 000 100 000 <EA>
			op_list[ 004100 | ea_v ] = op_bchg_i_b; // 0 000 100 001 <EA>
			op_list[ 004200 | ea_v ] = op_bclr_i_b; // 0 000 100 010 <EA>
			op_list[ 004300 | ea_v ] = op_bset_i_b; // 0 000 100 011 <EA>

			op_list[ 005000 | ea_v ] = op_eori_b; // 0 000 101 000 <EA>
			op_list[ 005100 | ea_v ] = op_eori_w; // 0 000 101 001 <EA>
			op_list[ 005200 | ea_v ] = op_eori_l; // 0 000 101 010 <EA>
			op_list[ 005300 | ea_v ] = op_cas_b;   // 0 000 101 011 <EA>
			
			op_list[ 006000 | ea_v ] = op_cmpi_b; // 0 000 110 000 <EA>
			op_list[ 006100 | ea_v ] = op_cmpi_w; // 0 000 110 001 <EA>
			op_list[ 006200 | ea_v ] = op_cmpi_l; // 0 000 110 010 <EA>
			op_list[ 006300 | ea_v ] = op_cas_w;   // 0 000 110 011 <EA>

			op_list[ 007000 | ea_v ] = op_moves_b; // 0 000 111 000 <EA>
			op_list[ 007100 | ea_v ] = op_moves_w; // 0 000 111 001 <EA>
			op_list[ 007200 | ea_v ] = op_moves_l; // 0 000 111 010 <EA>
			op_list[ 007300 | ea_v ] = op_cas_l;   // 0 000 111 011 <EA>

			op_list[ 040000 | ea_v ] = op_negx_b; // 0 100 000 000 <EA>
			op_list[ 040100 | ea_v ] = op_negx_w; // 0 100 000 001 <EA>
			op_list[ 040200 | ea_v ] = op_negx_l; // 0 100 000 010 <EA>
			op_list[ 040300 | ea_v ] = op_move_from_sr; // 0 100 000 011 <EA>

			op_list[ 041000 | ea_v ] = op_clr_b;  // 0 100 001 000 <EA>
			op_list[ 041100 | ea_v ] = op_clr_w;  // 0 100 001 001 <EA>
			op_list[ 041200 | ea_v ] = op_clr_l;  // 0 100 001 010 <EA>
			op_list[ 041300 | ea_v ] = op_move_from_cr; // 0 100 001 011 <EA>

			op_list[ 042000 | ea_v ] = op_neg_b; // 0 100 010 000 <EA>
			op_list[ 042100 | ea_v ] = op_neg_w; // 0 100 010 001 <EA>
			op_list[ 042200 | ea_v ] = op_neg_l; // 0 100 010 010 <EA>
			op_list[ 042300 | ea_v ] = op_move_to_cr; // 0 100 010 011 <EA>

			op_list[ 043000 | ea_v ] = op_not_b; // 0 100 011 000 <EA>
			op_list[ 043100 | ea_v ] = op_not_w; // 0 100 011 001 <EA>
			op_list[ 043200 | ea_v ] = op_not_l; // 0 100 011 010 <EA>
			op_list[ 043300 | ea_v ] = op_move_to_sr; // 0 100 011 011 <EA>

			op_list[ 044000 | ea_v ] = op_nbcd_m; // 0 100 100 000 <EA>

			op_list[ 045000 | ea_v ] = op_tst_b; // 0 100 101 000 <EA>
			op_list[ 045100 | ea_v ] = op_tst_w; // 0 100 101 001 <EA>
			op_list[ 045200 | ea_v ] = op_tst_l; // 0 100 101 010 <EA>
			op_list[ 045300 | ea_v ] = op_tas; // 0 100 101 011 <EA>

			op_list[ 046000 | ea_v ] = op_mul_l; // 0 100 110 000 <EA>
			op_list[ 046100 | ea_v ] = op_div_l; // 0 100 110 001 <EA>

			op_list[ 0160300 | ea_v ] = op_asr_ea;  // 1 110 000 011 <EA>
			op_list[ 0161300 | ea_v ] = op_lsr_ea;  // 1 110 001 011 <EA>
			op_list[ 0162300 | ea_v ] = op_roxr_ea; // 1 110 010 011 <EA>
			op_list[ 0163300 | ea_v ] = op_ror_ea;  // 1 110 011 011 <EA>

			op_list[ 0164300 | ea_v ] = op_bftst_m; // 1 110 100 011 <EA>
			op_list[ 0165300 | ea_v ] = op_bfchg_m; // 1 110 101 011 <EA>
			op_list[ 0166300 | ea_v ] = op_bfclr_m; // 1 110 110 011 <EA>
			op_list[ 0167300 | ea_v ] = op_bfset_m; // 1 110 111 011 <EA>

			op_list[ 0160700 | ea_v ] = op_asl_ea;  // 1 110 000 111 <EA>
			op_list[ 0161700 | ea_v ] = op_lsl_ea;  // 1 110 001 111 <EA>
			op_list[ 0162700 | ea_v ] = op_roxl_ea; // 1 110 010 111 <EA>
			op_list[ 0163700 | ea_v ] = op_rol_ea;  // 1 110 011 111 <EA>

			op_list[ 0164700 | ea_v ] = op_bfextu_m;// 1 110 100 111 <EA>
			op_list[ 0165700 | ea_v ] = op_bfexts_m;// 1 110 101 111 <EA>
			op_list[ 0166700 | ea_v ] = op_bfffo_m; // 1 110 110 111 <EA>
			op_list[ 0167700 | ea_v ] = op_bfins_m; // 1 110 111 111 <EA>

			if( j == 2 || j >= 5 ) {
				op_list[ 000300 | ea_v ] = op_cmp2_b; // 0 000 000 011 <EA>
				op_list[ 001300 | ea_v ] = op_cmp2_w; // 0 000 001 011 <EA>
				op_list[ 002300 | ea_v ] = op_cmp2_l; // 0 000 010 011 <EA>
				op_list[ 003300 | ea_v ] = op_callm;  // 0 000 011 011 <EA>
				op_list[ 044100 | ea_v ] = op_pea;	   // 0 100 100 001 <EA> 
				op_list[ 044200 | ea_v ] = op_movem_w_to; // 0 100 100 010 <EA> 
				op_list[ 044300 | ea_v ] = op_movem_l_to; // 0 100 100 011 <EA> 
				op_list[ 046200 | ea_v ] = op_movem_w_from; // 0 100 110 010 <EA>
				op_list[ 046300 | ea_v ] = op_movem_l_from; // 0 100 110 011 <EA>
				op_list[ 047200 | ea_v ] = op_jsr; // 0 100 111 010 <EA>
				op_list[ 047300 | ea_v ] = op_jmp; // 0 100 111 010 <EA>
				for( int k = 0; k < 8; ++k ) {
					// 0 100 kkk 111 <ea> 
					op_list[ 040700 | k << 9 | ea_v ] = op_lea; 
				}
			}
			if( j == 3 ) {
				op_list[ 046200 | ea_v ] = op_movem_w_from_incr; 
				op_list[ 046300 | ea_v ] = op_movem_l_from_incr; 
			}
			if( j == 4 ) {
				op_list[ 044200 | ea_v ] = op_movem_w_to_decr; 
				op_list[ 044300 | ea_v ] = op_movem_l_to_decr; 
			}
			for( int k = 0; k < 8; ++k ) {
				int pm = k << 9 | ea_v;
				op_list[ 040400 | pm ] = op_chk_l;    // 0 100 kkk 100 <EA>
				op_list[ 040600 | pm ] = op_chk_w;	  // 0 100 kkk 110 <EA>
				op_list[ 000400 | pm ] = op_btst_d_b; // 0 000 nnn 100 <EA>
				op_list[ 000500 | pm ] = op_bchg_d_b; // 0 000 nnn 101 <EA>
				op_list[ 000600 | pm ] = op_bclr_d_b; // 0 000 nnn 110 <EA>
				op_list[ 000700 | pm ] = op_bset_d_b; // 0 000 nnn 111 <EA>

				op_list[ 050000 | pm ] = op_addq_b;	  // 0 101 xxx 000 <EA>
				op_list[ 050100 | pm ] = op_addq_w;	  // 0 101 xxx 001 <EA>
				op_list[ 050200 | pm ] = op_addq_l;	  // 0 101 xxx 010 <EA>

				op_list[ 050400 | pm ] = op_subq_b;	// 0 101 xxx 100 <EA>
				op_list[ 050500 | pm ] = op_subq_w;	// 0 101 xxx 101 <EA>
				op_list[ 050600 | pm ] = op_subq_l;	// 0 101 xxx 110 <EA>

				op_list[ 0100000 | pm ] = op_or_b;   // 1 000 nnn 000 <EA>
				op_list[ 0100100 | pm ] = op_or_w;   // 1 000 nnn 001 <EA>
				op_list[ 0100200 | pm ] = op_or_l;   // 1 000 nnn 010 <EA>
				op_list[ 0100300 | pm ] = op_divu_w; // 1 000 nnn 011 <EA>

				op_list[ 0100400 | pm ] = op_or_m_b; // 1 000 nnn 100 <EA>
				op_list[ 0100500 | pm ] = op_or_m_w; // 1 000 nnn 101 <EA>
				op_list[ 0100600 | pm ] = op_or_m_l; // 1 000 nnn 110 <EA>
				op_list[ 0100700 | pm ] = op_divs_w; // 1 000 nnn 111 <EA>

				op_list[ 0110000 | pm ] = op_sub_b;  // 1 001 nnn 000 <EA>
				op_list[ 0110100 | pm ] = op_sub_w;  // 1 001 nnn 001 <EA>
				op_list[ 0110200 | pm ] = op_sub_l;  // 1 001 nnn 010 <EA>
				op_list[ 0110300 | pm ] = op_suba_w; // 1 001 nnn 011 <EA>

				op_list[ 0110400 | pm ] = op_sub_m_b;// 1 001 nnn 100 <EA>
				op_list[ 0110500 | pm ] = op_sub_m_w;// 1 001 nnn 101 <EA>
				op_list[ 0110600 | pm ] = op_sub_m_l;// 1 001 nnn 110 <EA>
				op_list[ 0110700 | pm ] = op_suba_l; // 1 001 nnn 111 <EA>

				op_list[ 0130000 | pm ] = op_cmp_b;  // 1 011 nnn 000 <EA>
				op_list[ 0130100 | pm ] = op_cmp_w;  // 1 011 nnn 001 <EA>
				op_list[ 0130200 | pm ] = op_cmp_l;  // 1 011 nnn 010 <EA>
				op_list[ 0130300 | pm ] = op_cmpa_w; // 1 011 nnn 011 <EA>
				
				op_list[ 0130400 | pm ] = op_eor_b;  // 1 011 nnn 100 <EA>
				op_list[ 0130500 | pm ] = op_eor_w;  // 1 011 nnn 101 <EA>
				op_list[ 0130600 | pm ] = op_eor_l;  // 1 011 nnn 110 <EA>
				op_list[ 0130700 | pm ] = op_cmpa_l; // 1 011 nnn 111 <EA>

				op_list[ 0140000 | pm ] = op_and_b;  // 1 100 nnn 000 <EA>
				op_list[ 0140100 | pm ] = op_and_w;  // 1 100 nnn 001 <EA>
				op_list[ 0140200 | pm ] = op_and_l;  // 1 100 nnn 010 <EA>
				op_list[ 0140300 | pm ] = op_mulu_w; // 1 100 nnn 011 <EA>
			
				op_list[ 0140400 | pm ] = op_and_m_b;// 1 100 nnn 100 <EA>
				op_list[ 0140500 | pm ] = op_and_m_w;// 1 100 nnn 101 <EA>
				op_list[ 0140600 | pm ] = op_and_m_l;// 1 100 nnn 110 <EA>
				op_list[ 0140700 | pm ] = op_muls_w; // 1 100 nnn 111 <EA>

				op_list[ 0150000 | pm ] = op_add_b;  // 1 101 nnn 000 <EA>
				op_list[ 0150100 | pm ] = op_add_w;  // 1 101 nnn 001 <EA>
				op_list[ 0150200 | pm ] = op_add_l;  // 1 101 nnn 010 <EA>
				op_list[ 0150300 | pm ] = op_adda_w; // 1 101 nnn 011 <EA>

				op_list[ 0150400 | pm ] = op_add_m_b;// 1 101 nnn 100 <EA>
				op_list[ 0150500 | pm ] = op_add_m_w;// 1 101 nnn 101 <EA>
				op_list[ 0150600 | pm ] = op_add_m_l;// 1 101 nnn 110 <EA>
				op_list[ 0150700 | pm ] = op_adda_l; // 1 101 nnn 111 <EA>

				for( int l = 0; l < 8; ++l ) {
					if( l != 1 ) {
						uint32_t ea2 =  ea_v | (k << 9) | (l << 6);
						op_list[ 010000 | ea2 ] = op_move_b; // 0 001 nnn lll <EA>
						op_list[ 020000 | ea2 ] = op_move_l; // 0 010 nnn lll <EA>
						op_list[ 030000 | ea2 ] = op_move_w; // 0 011 nnn lll <EA>
					} else {
						uint32_t ea2 =  ea_v | (k << 9);
						op_list[ 020100 | ea2 ] = op_movea_l; // 0 010 nnn 001 <EA>
						op_list[ 030100 | ea2 ] = op_movea_w; // 0 011 nnn 001 <EA>
					}
				}
			}
			for( int c = 0; c < 16; ++c ) {
				// 0 101 CCC C11 <EA>
				op_list[ 050300 | c << 8 | ea_v ] = op_scc;
			}
		}

	}
	for(int i = 0; i <16; ++i ) {
		op_list[ 047100 | i ] = op_trap;
		op_list[ 050372 | i << 8 ] = op_trapcc_w;
		op_list[ 050373 | i << 8 ] = op_trapcc_l;
		op_list[ 050374 | i << 8 ] = op_trapcc;
	}
	for(int i = 0; i < 8; ++i ) {
		op_list[ 044240 | i ] = op_movem_w_to_decr; // 0 100 100 010 100 nnn 
		op_list[ 044340 | i ] = op_movem_l_to_decr; // 0 100 100 011 100 nnn 
		op_list[ 046230 | i ] = op_movem_w_from_incr; // 0 100 110 010 011 nnn 
		op_list[ 046330 | i ] = op_movem_l_from_incr; // 0 100 110 011 011 nnn

		op_list[ 047130 | i ] = op_unlk; // 0 100 111 001 011 mmm
		op_list[ 047140 | i ] = op_move_to_usp; // 0 100 111 001 100 mmm
		op_list[ 047150 | i ] = op_move_from_usp; // 0 100 111 001 101 mmm

		op_list[ 0173000 | i ] = op_move16_inc_imm; // 1 111 011 000 mmm
		op_list[ 0173010 | i ] = op_move16_imm_inc; // 1 111 011 001 mmm
		op_list[ 0173020 | i ] = op_move16_r_imm;   // 1 111 011 010 mmm
		op_list[ 0173030 | i ] = op_move16_imm_r;   // 1 111 011 011 mmm
		op_list[ 0173040 | i ] = op_move16_inc_inc; // 1 111 011 100 mmm

		for(int j = 0; j < 8; ++j ) {
			int opx = i << 9 | j;
			op_list[ 0160000 | opx ] = op_asr_b_i; // 1 110 mmm 000 000 yyy
			op_list[ 0160010 | opx ] = op_lsr_b_i; // 1 110 mmm 000 001 yyy
			op_list[ 0160020 | opx ] = op_roxr_b_i;// 1 110 mmm 000 010 yyy
			op_list[ 0160030 | opx ] = op_ror_b_i; // 1 110 mmm 000 011 yyy

			op_list[ 0160040 | opx ] = op_asr_b_d; // 1 110 mmm 000 100 yyy
			op_list[ 0160050 | opx ] = op_lsr_b_d; // 1 110 mmm 000 101 yyy
			op_list[ 0160060 | opx ] = op_roxr_b_d;// 1 110 mmm 000 110 yyy
			op_list[ 0160070 | opx ] = op_ror_b_d; // 1 110 mmm 000 111 yyy

			op_list[ 0160100 | opx ] = op_asr_w_i; // 1 110 mmm 001 000 yyy
			op_list[ 0160110 | opx ] = op_lsr_w_i; // 1 110 mmm 001 001 yyy
			op_list[ 0160120 | opx ] = op_roxr_w_i;// 1 110 mmm 001 010 yyy
			op_list[ 0160130 | opx ] = op_ror_w_i; // 1 110 mmm 001 011 yyy

			op_list[ 0160140 | opx ] = op_asr_w_d; // 1 110 mmm 001 100 yyy
			op_list[ 0160150 | opx ] = op_lsr_w_d; // 1 110 mmm 001 101 yyy
			op_list[ 0160160 | opx ] = op_roxr_w_d;// 1 110 mmm 001 110 yyy
			op_list[ 0160170 | opx ] = op_ror_w_d; // 1 110 mmm 001 111 yyy

			op_list[ 0160200 | opx ] = op_asr_l_i; // 1 110 mmm 010 000 yyy
			op_list[ 0160210 | opx ] = op_lsr_l_i; // 1 110 mmm 010 001 yyy
			op_list[ 0160220 | opx ] = op_roxr_l_i;// 1 110 mmm 010 010 yyy
			op_list[ 0160230 | opx ] = op_ror_l_i; // 1 110 mmm 010 011 yyy

			op_list[ 0160240 | opx ] = op_asr_l_d; // 1 110 mmm 010 100 yyy
			op_list[ 0160250 | opx ] = op_lsr_l_d; // 1 110 mmm 010 101 yyy
			op_list[ 0160260 | opx ] = op_roxr_l_d;// 1 110 mmm 010 110 yyy
			op_list[ 0160270 | opx ] = op_ror_l_d; // 1 110 mmm 010 111 yyy

			
			op_list[ 0160400 | opx ] = op_asl_b_i; // 1 110 mmm 100 000 yyy
			op_list[ 0160410 | opx ] = op_lsl_b_i; // 1 110 mmm 100 001 yyy
			op_list[ 0160420 | opx ] = op_roxl_b_i;// 1 110 mmm 100 010 yyy
			op_list[ 0160430 | opx ] = op_rol_b_i; // 1 110 mmm 100 011 yyy

			op_list[ 0160440 | opx ] = op_asl_b_d; // 1 110 mmm 100 100 yyy
			op_list[ 0160450 | opx ] = op_lsl_b_d; // 1 110 mmm 100 101 yyy
			op_list[ 0160460 | opx ] = op_roxl_b_d;// 1 110 mmm 100 110 yyy
			op_list[ 0160470 | opx ] = op_rol_b_d; // 1 110 mmm 100 111 yyy

			op_list[ 0160500 | opx ] = op_asl_w_i; // 1 110 mmm 101 000 yyy
			op_list[ 0160510 | opx ] = op_lsl_w_i; // 1 110 mmm 101 001 yyy
			op_list[ 0160520 | opx ] = op_roxl_w_i;// 1 110 mmm 101 010 yyy
			op_list[ 0160530 | opx ] = op_rol_w_i; // 1 110 mmm 101 011 yyy

			op_list[ 0160540 | opx ] = op_asl_w_d; // 1 110 mmm 101 100 yyy
			op_list[ 0160550 | opx ] = op_lsl_w_d; // 1 110 mmm 101 101 yyy
			op_list[ 0160560 | opx ] = op_roxl_w_d;// 1 110 mmm 101 110 yyy
			op_list[ 0160570 | opx ] = op_rol_w_d; // 1 110 mmm 101 111 yyy

			op_list[ 0160600 | opx ] = op_asl_l_i; // 1 110 mmm 110 000 yyy
			op_list[ 0160610 | opx ] = op_lsl_l_i; // 1 110 mmm 110 001 yyy
			op_list[ 0160620 | opx ] = op_roxl_l_i;// 1 110 mmm 110 010 yyy
			op_list[ 0160630 | opx ] = op_rol_l_i; // 1 110 mmm 110 011 yyy

			op_list[ 0160640 | opx ] = op_asl_l_d; // 1 110 mmm 110 100 yyy
			op_list[ 0160650 | opx ] = op_lsl_l_d; // 1 110 mmm 110 101 yyy
			op_list[ 0160660 | opx ] = op_roxl_l_d;// 1 110 mmm 110 110 yyy
			op_list[ 0160670 | opx ] = op_rol_l_d; // 1 110 mmm 110 111 yyy

		}
	}
	op_list[ 000074 ] = op_ori_ccr;  // 0 000 000 000 111 011
	op_list[ 000174 ] = op_ori_sr;   // 0 000 000 001 111 011
	op_list[ 001074 ] = op_andi_ccr; // 0 000 001 000 111 011
	op_list[ 001174 ] = op_andi_sr;  // 0 000 001 001 111 011
	op_list[ 005074 ] = op_eori_ccr; // 0 000 101 000 111 011
	op_list[ 005174 ] = op_eori_sr;  // 0 000 101 001 111 011

	op_list[ 006374 ] = op_cas2_w;   // 0 000 110 011 111 011
	op_list[ 007374 ] = op_cas2_l;   // 0 000 111 011 111 011

	op_list[ 047160 ] = op_reset;
	op_list[ 047161 ] = op_nop;
	op_list[ 047162 ] = op_stop;
	op_list[ 047163 ] = op_rte;
	op_list[ 047164 ] = op_rtd;
	op_list[ 047165 ] = op_rts;
	op_list[ 047166 ] = op_trapv;
	op_list[ 047167 ] = op_rtr;
	op_list[ 047172 ] = op_movec_from;
	op_list[ 047173 ] = op_movec_to;
	for(int k = 0; k < 0x100; ++k ) {
		op_list[ 060000 | k ] = op_bra;
		op_list[ 060400 | k ] = op_bsr;
		for( int c = 2; c < 16; ++c ) {
			op_list[060000 |c << 8 | k ] = op_bcc;
		}
		for(int i = 0; i < 8; ++i ) {
			op_list[070000 | i << 9 | k ] = op_moveq;
		}
	}
	for(int i = 0; i < 0777; ++i ) {
		op_list[0171000 | i ] = op_fpu; // 1 111 001 xxx <EA>
		op_list[0172000 | i ] = op_mmu; // 1 111 010 xxx <EA>
	}
	for(int i =0; i < 0x100; ++i ) {
		op_list[0x7100 | i ] = op_emul;
	}
}
