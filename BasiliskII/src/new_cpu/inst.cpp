#include "registers.hpp"
#include "memory.hpp"
#include "common.hpp"
#include "inst_cmn.hpp"
void ILLEGAL(REGS* regs) __attribute__((noreturn));
void callm(REGS* regs, uint32_t, int) {
	ILLEGAL(regs);
}
void rtm(REGS* regs,uint32_t) {
	ILLEGAL(regs);
}
void ill_op(REGS* regs, uint16_t d) {
	ILLEGAL(regs);
}
template<class D>
void op_ori_b(REGS* regs, uint16_t d) {
	uint8_t imm = FETCH16();
	if( d & 077 == 074 ) {
		// ORI to CCR
		regs->X |= (imm >> 4) & 1;
		regs->N |= (imm >> 3) & 1;
		regs->Z |= (imm >> 2) & 1;
		regs->V |= (imm >> 1) & 1;
		regs->C |= imm & 1;
		return;
	} else {
		D ea (regs, d&7);
		ea.writeb(op_or_cmn<int8_t>(regs, ea.readb(), imm));
	}
}

template<class D>
void op_ori_w(REGS* regs, uint16_t d) {
	uint16_t imm = FETCH16();
	if( d & 077 == 074 ) {
		// ORI to SR
		if( ! regs->S ) {
			// not supervisor
			ILLEGAL(regs);
		}
		regs->X |= (imm >> 4) & 1;
		regs->N |= (imm >> 3) & 1;
		regs->Z |= (imm >> 2) & 1;
		regs->V |= (imm >> 1) & 1;
		regs->C |= imm & 1;

		regs->IX |= (imm >> 8) & 3;
		regs->M |= (imm >> 12) & 1;
		regs->S |= (imm >> 13) & 1;
		regs->T |= (imm >> 14) & 3;
		return;
	} else {
		D ea (regs, d&7);
		ea.writew(op_or_cmn<int16_t>(regs, ea.readw(), imm));
	}
}

template<class D>
void op_ori_l(REGS* regs, uint16_t d) {
	uint32_t imm = FETCH32();
	D ea (regs, d&7);
	ea.writel(op_or_cmn<int32_t>(regs, ea.readl(), imm));
}



void btst_d_dn(REGS* regs, uint16_t d) {
	uint32_t v = regs->D[d&7];
	btst_d_N(regs, (d>>9)&7, v);
}

template<class D>
void btst_d_b(REGS* regs, uint16_t d) {
	D ea(regs, d&7);
	btst_d_N(regs, (d>>9)&7, ea.readb());
}


void bchg_d_dn(REGS* regs, uint16_t d) {
	uint32_t v = regs->D[d&7];
	regs->D[d&7] = bchg_d_N(regs, (d>>9)&7, v);
}
template<class D>
void bchg_d_b(REGS* regs, uint16_t d) {
	D ea(regs, d&7);
	ea.writeb(bchg_d_N(regs, (d>>9)&7, ea.readb()));
}

void bclr_d_dn(REGS* regs, uint16_t d) {
	uint32_t v = regs->D[d&7];
	regs->D[d&7] = bclr_d_N(regs, (d>>9)&7, v);
}
template<class D>
void bclr_d_b(REGS* regs, uint16_t d) {
	D ea(regs, d&7);
	ea.writeb(bclr_d_N(regs, (d>>9)&7, ea.readb()));
}


void bset_d_dn(REGS* regs, uint16_t d) {
	uint32_t v = regs->D[d&7];
	regs->D[d&7] = bset_d_N(regs, (d>>9)&7, v);
}
template<class D>
void bset_d_b(REGS* regs, uint16_t d) {
	D ea(regs, d&7);
	ea.writeb(bset_d_N(regs, (d>>9)&7, ea.readb()));
}


template<class D>
void op_andi_b(REGS* regs, uint16_t d) {
	uint8_t imm = FETCH16();
	if( d & 077 == 074 ) {
		// ANDI to CCR
		regs->X &= (imm >> 4) & 1;
		regs->N &= (imm >> 3) & 1;
		regs->Z &= (imm >> 2) & 1;
		regs->V &= (imm >> 1) & 1;
		regs->C &= imm & 1;
	} else {
		D ea(regs, d&7);
		ea.writeb(op_and_cmn<int8_t>(regs, ea.readb(), imm));
	}
}


template<class D>
void op_andi_w(REGS* regs, uint16_t d) {
	uint16_t imm = FETCH16();
	if( d & 077 == 074 ) {
		// ANDI to SR
		if( ! regs->S ) {
			// not supervisand
			ILLEGAL(regs);
		}
		regs->X &= (imm >> 4) & 1;
		regs->N &= (imm >> 3) & 1;
		regs->Z &= (imm >> 2) & 1;
		regs->V &= (imm >> 1) & 1;
		regs->C &= imm & 1;

		regs->IX &= (imm >> 8) & 3;
		regs->M &= (imm >> 12) & 1;
		regs->S &= (imm >> 13) & 1;
		regs->T &= (imm >> 14) & 3;
	} else {
		D ea(regs, d&7);
		ea.writew(op_and_cmn<int16_t>(regs, ea.readw(), imm));
	}
}


template<class D>
void op_andi_l(REGS* regs, uint16_t d) {	
	uint32_t imm = FETCH32();
	D ea(regs, d&7);
	ea.writel(op_and_cmn<int32_t>(regs, ea.readl(), imm));
}


template<class D>
void op_subi_b(REGS* regs, uint16_t d) {
	uint8_t imm = FETCH16();
	D ea(regs, d&7);
	ea.writeb(op_subb(regs, ea.readb(), imm));
}


template<class D>
void op_subi_w(REGS* regs, uint16_t d) {
	uint16_t imm = FETCH16();
	D ea(regs, d&7);
	ea.writew( op_subw(regs, ea.readw(), imm));
}


template<class D>
void op_subi_l(REGS* regs, uint16_t d) {
	uint32_t imm = FETCH32();
	D ea(regs, d&7);
	ea.writel( op_subl(regs, ea.readl(), imm) );
}


template<class D>
void op_addi_b(REGS* regs, uint16_t d) {
	int8_t imm = FETCH16();
	D ea(regs, d&7);
	ea.writeb( op_addb(regs, ea.readb(), imm) );
}


template<class D>
void op_addi_w(REGS* regs, uint16_t d) {
	uint16_t imm = FETCH16();
	D ea(regs, d&7);
	ea.writew( op_addw(regs, ea.readw(), imm) );
}

template<class D>
void op_addi_l(REGS* regs, uint16_t d) {
	uint32_t imm = FETCH32();
	D ea(regs, d&7);
	ea.writel( op_addl(regs, ea.readl(), imm) );
}

void btst_i_d(REGS* regs, uint16_t d) {
	uint8_t bn = FETCH16();
	btst_i_N(regs, bn, regs->D[d&7]);
}

template<class D>
void btst_i_b(REGS* regs, uint16_t d) {
	uint8_t bn = FETCH16();
	D ea(regs, d&7);
	btst_i_N(regs, bn, ea.readb());
}

void rtm_d(REGS* regs, uint16_t d) {
	rtm(regs, regs->D[d]);
}
void rtm_a(REGS* regs, uint16_t d) {
	rtm(regs, regs->A[d]);
}

template<class D>
void op_callm(REGS* regs, uint16_t d) {
	int ac = FETCH16();
	D ea(regs, d&7);
	callm(regs, ea.addr(), ac);
}

void bchg_i_d(REGS* regs, uint16_t d) {
	uint8_t bn = FETCH16();
	regs->D[d] = bchg_i_N(regs, bn, regs->D[d]);
}

template<class D>
void bchg_i_b(REGS* regs, uint16_t d) {
	uint8_t bn = FETCH16();
	D ea(regs, d&7);
	ea.writeb( bchg_i_N(regs, bn, ea.readb()));
}


void bclr_i_d(REGS* regs, uint16_t d) {
	uint8_t bn = FETCH16();
	regs->D[d] = bclr_i_N(regs, bn, regs->D[d]);
}

template<class D>
void bclr_i_b(REGS* regs, uint16_t d) {
	uint8_t bn = FETCH16();
	D ea(regs, d&7);
	ea.writeb( bclr_i_N(regs, bn, ea.readb()));
}


void bset_i_d(REGS* regs, uint16_t d) {
	uint8_t bn = FETCH16();
	regs->D[d] = bset_i_N(regs, bn, regs->D[d]);
}

template<class D>
void bset_i_b(REGS* regs, uint16_t d) {
	uint8_t bn = FETCH16();
	D ea(regs, d&7);
	ea.writeb( bset_i_N(regs, bn, ea.readb()));
}
template<class D>
void op_eori_b(REGS* regs, uint16_t d) {
	uint8_t imm = FETCH16();
	if( d & 077 == 074 ) {
		// EORI to CCR
		regs->X ^= (imm >> 4) & 1;
		regs->N ^= (imm >> 3) & 1;
		regs->Z ^= (imm >> 2) & 1;
		regs->V ^= (imm >> 1) & 1;
		regs->C ^= imm & 1;
	} else {
		D ea(regs, d&7);
		ea.writeb( op_xor_cmn<int8_t>(regs, ea.readb(), imm));
	}
}


template<class D>
void op_eori_w(REGS* regs, uint16_t d) {
	uint16_t imm = FETCH16();
	if( d & 077 == 074 ) {
		// EORI to SR
		if( ! regs->S ) {
			// not supervisor
			ILLEGAL(regs);
		}
		regs->X ^= (imm >> 4) & 1;
		regs->N ^= (imm >> 3) & 1;
		regs->Z ^= (imm >> 2) & 1;
		regs->V ^= (imm >> 1) & 1;
		regs->C ^= imm & 1;

		regs->IX ^= (imm >> 8) & 3;
		regs->M ^= (imm >> 12) & 1;
		regs->S ^= (imm >> 13) & 1;
		regs->T ^= (imm >> 14) & 3;
	} else {
		D ea(regs, d&7);
		ea.writew( op_xor_cmn<int16_t>(regs, ea.readw(), imm));
	}
}

template<class D>
void op_eori_l(REGS* regs, uint16_t d) {
	uint32_t imm = FETCH32();
	D ea(regs, d&7);
	ea.writel( op_xor_cmn<int32_t>(regs, ea.readl(), imm));
}
template<class D>
void op_cmp2_b(REGS* regs, uint16_t d) {
	int wd = FETCH16();
	int8_t v = regs->R[wd >> 12];
	D ea(regs, d&7);
	int8_t lw = read_b(ea.addr());
	int8_t hi = read_b(ea.addr()+1);
	regs->Z = ( v == lw || v == hi );
	regs->C = ( v < lw || v > hi );
	if( ((wd >> 11) & 1) && regs->C ) {
		raise_ex(regs, 6);
	}
}

template<class D>
void op_cmp2_w(REGS* regs, uint16_t d) {
	int wd = FETCH16() >> 12;
	int16_t v = regs->R[wd];
	D ea(regs, d&7);
	int16_t lw = read_w(ea.addr());
	int16_t hi = read_w(ea.addr()+2);
	regs->Z = ( v == lw || v == hi );
	regs->C = ( v < lw || v > hi );
}


template<class D>
void op_cmp2_l(REGS* regs, uint16_t d) {
	int wd = FETCH16();
	int32_t v = regs->R[wd >> 12];
	D ea(regs, d&7);
	int32_t lw = read_l(ea.addr());
	int32_t hi = read_l(ea.addr()+4);
	regs->Z = ( v == lw || v == hi );
	regs->C = ( v < lw || v > hi );
	if( ((wd >> 11) & 1) && regs->C ) {
		raise_ex(regs, 6);
	}
}

template<class D>
void op_cmpi_b(REGS* regs, uint16_t d) {
	uint8_t imm = FETCH16();
	D ea(regs, d&7);
	op_subb(regs, ea.readb(), imm);
}

template<class D>
void op_cmpi_w(REGS* regs, uint16_t d) {
	uint16_t imm = FETCH16();
	D ea(regs, d&7);
	op_subw(regs, ea.readw(), imm);
}

template<class D>
void op_cmpi_l(REGS* regs, uint16_t d) {
	uint16_t imm = FETCH16();
	D ea(regs, d&7);
	op_subw(regs, ea.readl(), imm);
}


template<class D>
void op_cas_b(REGS* regs, uint16_t d) {
	uint16_t nw = FETCH16();
	if( d & 077 == 074 ) {
		// CAS2
		uint16_t nw2 = FETCH16();
		int s1 = (nw >> 12) & 15;
		int u1 = (nw >> 6) & 7;
		int c1 = nw & 7;
		int s2 = (nw2 >> 12) & 15;
		int u2 = (nw2 >> 6) & 7;
		int c2 = nw2 & 7;
		op_subb(regs, regs->R[s1], regs->D[c1]);
		if( regs.Z ) {
			op_subb(regs, regs->R[s2], regs->D[c2]);
			if( regs.Z ) {
				regs->R[s1] = regs->D[u1];
				regs->R[s2] = regs->D[
				return;
			}
		}
	} else {
		D ea(regs, d&7);
		int u = (nw >> 6) & 7;
		int c = nw & 7;
		uint8_t v = ea.readb();
		op_subb(regs, v, regs->D[c]);
		if( regs.Z ) {
			ea.writeb(regs->D[u]);
		} else {
			refs->D[c] = v;
		}
	}
}

bool quit_program;
extern REGS regs;
extern uint32_t ROMBaseMac;
using op_type = void (*)(REGS*, uint16_t);
op_type op_list[8192] = {
	// 0000 0000 00 xxx
	op_ori_b<EA_DN>,
	ill_op,
	op_ori_b<EA_MEM>,
	op_ori_b<EA_INCR<1>>,
	op_ori_b<EA_DECR<1>>,
	op_ori_b<EA_D16>,
	op_ori_b<EA_EX>,
	op_ori_b<EA_EX2_RW>,

	// 0000 0000 01 xxx
	op_ori_w<EA_DN>,
	ill_op,
	op_ori_w<EA_MEM>,
	op_ori_w<EA_INCR<2>>,
	op_ori_w<EA_DECR<2>>,
	op_ori_w<EA_D16>,
	op_ori_w<EA_EX>,
	op_ori_w<EA_EX2_RW>,
	
	// 0000 0000 10 xxx
	op_ori_l<EA_DN>,
	ill_op,
	op_ori_l<EA_MEM>,
	op_ori_l<EA_INCR<4>>,
	op_ori_l<EA_DECR<4>>,
	op_ori_l<EA_D16>,
	op_ori_l<EA_EX>,
	op_ori_l<EA_EX2_RW>,

	// 0000 0000 11 xxx
	ill_op,
	ill_op,	
	op_cmp2_b<EA_MEM>,
	ill_op,
	ill_op,
	op_cmp2_b<EA_D16>,
	op_cmp2_b<EA_EX>,
	op_cmp2_b<EA_EX2_C>,

	// 0000 0001 00 xxx
	btst_d_dn,
	ill_op,
	btst_d_b<EA_MEM>,
	btst_d_b<EA_INCR<1>>,
	btst_d_b<EA_DECR<1>>,
	btst_d_b<EA_D16>,
	btst_d_b<EA_EX>,
	btst_d_b<EA_EX2_R_B>,

	// 0000 0001 01 xxx
	bchg_d_dn,
	ill_op,
	bchg_d_b<EA_MEM>,
	bchg_d_b<EA_INCR<1>>,
	bchg_d_b<EA_DECR<1>>,
	bchg_d_b<EA_D16>,
	bchg_d_b<EA_EX>,
	bchg_d_b<EA_EX2_RW>,

	// 0000 0001 10 xxx
	bclr_d_dn,
	ill_op,
	bclr_d_b<EA_MEM>,
	bclr_d_b<EA_INCR<1>>,
	bclr_d_b<EA_DECR<1>>,
	bclr_d_b<EA_D16>,
	bclr_d_b<EA_EX>,
	bclr_d_b<EA_EX2_RW>,

	// 0000 0001 11 xxx
	bset_d_dn,
	ill_op,
	bset_d_b<EA_MEM>,
	bset_d_b<EA_INCR<1>>,
	bset_d_b<EA_DECR<1>>,
	bset_d_b<EA_D16>,
	bset_d_b<EA_EX>,
	bset_d_b<EA_EX2_RW>,

	// 0000 0010 00 xxx
	op_andi_b<EA_DN>,
	ill_op,
	op_andi_b<EA_MEM>,
	op_andi_b<EA_INCR<1>>,
	op_andi_b<EA_DECR<1>>,
	op_andi_b<EA_D16>,
	op_andi_b<EA_EX>,
	op_andi_b<EA_EX2_RW>,

	// 0000 0010 01 xxx
	op_andi_w<EA_DN>,
	ill_op,
	op_andi_w<EA_MEM>,
	op_andi_w<EA_INCR<2>>,
	op_andi_w<EA_DECR<2>>,
	op_andi_w<EA_D16>,
	op_andi_w<EA_EX>,
	op_andi_w<EA_EX2_RW>,
	
	// 0000 0010 10 xxx
	op_andi_l<EA_DN>,
	ill_op,
	op_andi_l<EA_MEM>,
	op_andi_l<EA_INCR<4>>,
	op_andi_l<EA_DECR<4>>,
	op_andi_l<EA_D16>,
	op_andi_l<EA_EX>,
	op_andi_l<EA_EX2_RW>,

	// 0000 0010 11 xxx
	ill_op,
	ill_op,
	op_cmp2_w<EA_MEM>,
	ill_op,
	ill_op,
	op_cmp2_w<EA_D16>,
	op_cmp2_w<EA_EX>,
	op_cmp2_w<EA_EX2_C>,

	// 0000 0011 00 xxx
	btst_d_dn,
	ill_op,
	btst_d_b<EA_MEM>,
	btst_d_b<EA_INCR<1>>,
	btst_d_b<EA_DECR<1>>,
	btst_d_b<EA_D16>,
	btst_d_b<EA_EX>,
	btst_d_b<EA_EX2_R_B>,

	// 0000 0011 01 xxx
	bchg_d_dn,
	ill_op,
	bchg_d_b<EA_MEM>,
	bchg_d_b<EA_INCR<1>>,
	bchg_d_b<EA_DECR<1>>,
	bchg_d_b<EA_D16>,
	bchg_d_b<EA_EX>,
	bchg_d_b<EA_EX2_RW>,

	// 0000 0011 10 xxx
	bclr_d_dn,
	ill_op,	
	bclr_d_b<EA_MEM>,
	bclr_d_b<EA_INCR<1>>,
	bclr_d_b<EA_DECR<1>>,
	bclr_d_b<EA_D16>,
	bclr_d_b<EA_EX>,
	bclr_d_b<EA_EX2_RW>,

	// 0000 0011 11 xxx
	bset_d_dn,
	ill_op,
	bset_d_b<EA_MEM>,
	bset_d_b<EA_INCR<1>>,
	bset_d_b<EA_DECR<1>>,
	bset_d_b<EA_D16>,
	bset_d_b<EA_EX>,
	bset_d_b<EA_EX2_RW>,

	// 0000 0100 00 xxx
	op_subi_b<EA_DN>,
	ill_op,
	op_subi_b<EA_MEM>,
	op_subi_b<EA_INCR<1>>,
	op_subi_b<EA_DECR<1>>,
	op_subi_b<EA_D16>,
	op_subi_b<EA_EX>,
	op_subi_b<EA_EX2_RW>,

	// 0000 0100 01 xxx
	op_subi_w<EA_DN>,
	ill_op,
	op_subi_w<EA_MEM>,
	op_subi_w<EA_INCR<2>>,
	op_subi_w<EA_DECR<2>>,
	op_subi_w<EA_D16>,
	op_subi_w<EA_EX>,
	op_subi_w<EA_EX2_RW>,
	
	// 0000 0100 10 xxx
	op_subi_l<EA_DN>,
	ill_op,
	op_subi_l<EA_MEM>,
	op_subi_l<EA_INCR<1>>,
	op_subi_l<EA_DECR<1>>,
	op_subi_l<EA_D16>,
	op_subi_l<EA_EX>,
	op_subi_l<EA_EX2_RW>,

	// 0000 0100 11 xxx
	ill_op,
	ill_op,
	op_cmp2_l<EA_MEM>,
	ill_op,
	ill_op,
	op_cmp2_l<EA_D16>,
	op_cmp2_l<EA_EX>,
	op_cmp2_l<EA_EX2_C>,
	
	// 0000 0101 00 xxx
	btst_d_dn,
	ill_op,
	btst_d_b<EA_MEM>,
	btst_d_b<EA_INCR<1>>,
	btst_d_b<EA_DECR<1>>,
	btst_d_b<EA_D16>,
	btst_d_b<EA_EX>,
	btst_d_b<EA_EX2_R_B>,

	// 0000 0101 01 xxx
	bchg_d_dn,
	ill_op,
	bchg_d_b<EA_MEM>,
	bchg_d_b<EA_INCR<1>>,
	bchg_d_b<EA_DECR<1>>,
	bchg_d_b<EA_D16>,
	bchg_d_b<EA_EX>,
	bchg_d_b<EA_EX2_RW>,

	// 0000 0101 10 xxx
	bclr_d_dn,
	ill_op,
	bclr_d_b<EA_MEM>,
	bclr_d_b<EA_INCR<1>>,
	bclr_d_b<EA_DECR<1>>,
	bclr_d_b<EA_D16>,
	bclr_d_b<EA_EX>,
	bclr_d_b<EA_EX2_RW>,

	// 0000 0101 11 xxx
	bset_d_dn,
	ill_op,
	bset_d_b<EA_MEM>,
	bset_d_b<EA_INCR<1>>,
	bset_d_b<EA_DECR<1>>,
	bset_d_b<EA_D16>,
	bset_d_b<EA_EX>,
	bset_d_b<EA_EX2_RW>,

	// 0000 0110 00 xxx
	op_addi_b<EA_DN>,
	ill_op,
	op_addi_b<EA_MEM>,
	op_addi_b<EA_INCR<1>>,
	op_addi_b<EA_DECR<1>>,
	op_addi_b<EA_D16>,
	op_addi_b<EA_EX>,
	op_addi_b<EA_EX2_RW>,

	// 0000 0110 01 xxx
	op_addi_w<EA_DN>,
	ill_op,
	op_addi_w<EA_MEM>,
	op_addi_w<EA_INCR<2>>,
	op_addi_w<EA_DECR<2>>,
	op_addi_w<EA_D16>,
	op_addi_w<EA_EX>,
	op_addi_w<EA_EX2_RW>,
	
	// 0000 0110 10 xxx
	op_addi_l<EA_DN>,
	ill_op,
	op_addi_l<EA_MEM>,
	op_addi_l<EA_INCR<4>>,
	op_addi_l<EA_DECR<4>>,
	op_addi_l<EA_D16>,
	op_addi_l<EA_EX>,
	op_addi_l<EA_EX2_RW>,

	// 0000 0110 11 xxx
	rtm_d,
	rtm_a,
	op_callm<EA_MEM>,
	ill_op,
	ill_op,
	op_callm<EA_D16>,
	op_callm<EA_EX>,
	op_callm<EA_EX2_C>,

	// 0000 0111 00 xxx
	btst_d_dn,
	ill_op,
	btst_d_b<EA_MEM>,
	btst_d_b<EA_INCR<1>>,
	btst_d_b<EA_DECR<1>>,
	btst_d_b<EA_D16>,
	btst_d_b<EA_EX>,
	btst_d_b<EA_EX2_R_B>,

	// 0000 0111 01 xxx
	bchg_d_dn,
	ill_op,
	bchg_d_b<EA_MEM>,
	bchg_d_b<EA_INCR<1>>,
	bchg_d_b<EA_DECR<1>>,
	bchg_d_b<EA_D16>,
	bchg_d_b<EA_EX>,
	bchg_d_b<EA_EX2_RW>,

	// 0000 0111 10 xxx
	bclr_d_dn,
	ill_op,
	bclr_d_b<EA_MEM>,
	bclr_d_b<EA_INCR<1>>,
	bclr_d_b<EA_DECR<1>>,
	bclr_d_b<EA_D16>,
	bclr_d_b<EA_EX>,
	bclr_d_b<EA_EX2_RW>,

	// 0000 0111 11 xxx
	bset_d_dn,
	ill_op,
	bset_d_b<EA_MEM>,
	bset_d_b<EA_INCR<1>>,
	bset_d_b<EA_DECR<1>>,
	bset_d_b<EA_D16>,
	bset_d_b<EA_EX>,
	bset_d_b<EA_EX2_RW>,

	// 0000 1000 00 xxx
	btst_i_d,
	ill_op,
	btst_i_b<EA_MEM>,
	btst_i_b<EA_INCR<1>>,
	btst_i_b<EA_DECR<1>>,
	btst_i_b<EA_D16>,
	btst_i_b<EA_EX>,
	btst_i_b<EA_EX2_R_B>,

	// 0000 1000 01 xxx
	bchg_i_d,
	ill_op,
	bchg_i_b<EA_MEM>,
	bchg_i_b<EA_INCR<1>>,
	bchg_i_b<EA_DECR<1>>,
	bchg_i_b<EA_D16>,
	bchg_i_b<EA_EX>,
	bchg_i_b<EA_EX2_RW>,

	// 0000 1000 10 xxx
	bclr_i_d,
	ill_op,
	bclr_i_b<EA_MEM>,
	bclr_i_b<EA_INCR<1>>,
	bclr_i_b<EA_DECR<1>>,
	bclr_i_b<EA_D16>,
	bclr_i_b<EA_EX>,
	bclr_i_b<EA_EX2_RW>,

	// 0000 1000 11 xxx
	bset_i_d,
	ill_op,
	bset_i_b<EA_MEM>,
	bset_i_b<EA_INCR<1>>,
	bset_i_b<EA_DECR<1>>,
	bset_i_b<EA_D16>,
	bset_i_b<EA_EX>,
	bset_i_b<EA_EX2_RW>,

	// 0000 1001 00 xxx
	btst_d_dn,
	ill_op,
	btst_d_b<EA_MEM>,
	btst_d_b<EA_INCR<1>>,
	btst_d_b<EA_DECR<1>>,
	btst_d_b<EA_D16>,
	btst_d_b<EA_EX>,
	btst_d_b<EA_EX2_R_B>,

	// 0000 1001 01 xxx
	bchg_d_dn,
	ill_op,
	bchg_d_b<EA_MEM>,
	bchg_d_b<EA_INCR<1>>,
	bchg_d_b<EA_DECR<1>>,
	bchg_d_b<EA_D16>,
	bchg_d_b<EA_EX>,
	bchg_d_b<EA_EX2_RW>,

	// 0000 1001 10 xxx
	bclr_d_dn,
	ill_op,
	bclr_d_b<EA_MEM>,
	bclr_d_b<EA_INCR<1>>,
	bclr_d_b<EA_DECR<1>>,
	bclr_d_b<EA_D16>,
	bclr_d_b<EA_EX>,
	bclr_d_b<EA_EX2_RW>,

	// 0000 1001 11 xxx
	bset_d_dn,
	ill_op,
	bset_d_b<EA_MEM>,
	bset_d_b<EA_INCR<1>>,
	bset_d_b<EA_DECR<1>>,
	bset_d_b<EA_D16>,
	bset_d_b<EA_EX>,
	bset_d_b<EA_EX2_RW>,

	// 0000 1010 00 xxx
	op_eori_b<EA_DN>,
	ill_op,
	op_eori_b<EA_MEM>,
	op_eori_b<EA_INCR<1>>,
	op_eori_b<EA_DECR<1>>,
	op_eori_b<EA_D16>,
	op_eori_b<EA_EX>,
	op_eori_b<EA_EX2_RW>,

	// 0000 1010 01 xxx
	op_eori_w<EA_DN>,
	ill_op,
	op_eori_w<EA_MEM>,
	op_eori_w<EA_INCR<2>>,
	op_eori_w<EA_DECR<2>>,
	op_eori_w<EA_D16>,
	op_eori_w<EA_EX>,
	op_eori_w<EA_EX2_RW>,
	
	// 0000 1010 10 xxx
	op_eori_l<EA_DN>,
	ill_op,
	op_eori_l<EA_MEM>,
	op_eori_l<EA_INCR<4>>,
	op_eori_l<EA_DECR<4>>,
	op_eori_l<EA_D16>,
	op_eori_l<EA_EX>,
	op_eori_l<EA_EX2_RW>,

	// 0000 1010 11 xxx
	ill_op,
	ill_op,
	op_cas_b<EA_MEM>,
	op_cas_b<EA_INCR<1>>,
	op_cas_b<EA_DECR<1>>,
	op_cas_b<EA_D16>,
	op_cas_b<EA_EX>,
	op_cas_b<EA_EX2_RW>,

	// 0000 1011 00 xxx
	btst_d_dn,
	ill_op,
	btst_d_b<EA_MEM>,
	btst_d_b<EA_INCR<1>>,
	btst_d_b<EA_DECR<1>>,
	btst_d_b<EA_D16>,
	btst_d_b<EA_EX>,
	btst_d_b<EA_EX2_R_B>,

	// 0000 1011 01 xxx
	bchg_d_dn,
	ill_op,
	bchg_d_b<EA_MEM>,
	bchg_d_b<EA_INCR<1>>,
	bchg_d_b<EA_DECR<1>>,
	bchg_d_b<EA_D16>,
	bchg_d_b<EA_EX>,
	bchg_d_b<EA_EX2_RW>,

	// 0000 1011 10 xxx
	bclr_d_dn,
	ill_op,
	bclr_d_b<EA_MEM>,
	bclr_d_b<EA_INCR<1>>,
	bclr_d_b<EA_DECR<1>>,
	bclr_d_b<EA_D16>,
	bclr_d_b<EA_EX>,
	bclr_d_b<EA_EX2_RW>,

	// 0000 1011 11 xxx
	bset_d_dn,
	ill_op,
	bset_d_b<EA_MEM>,
	bset_d_b<EA_INCR<1>>,
	bset_d_b<EA_DECR<1>>,
	bset_d_b<EA_D16>,
	bset_d_b<EA_EX>,
	bset_d_b<EA_EX2_RW>,

	// 0000 1100 00 xxx
	op_cmpi_b<EA_DN>,
	ill_op,
	op_cmpi_b<EA_MEM>,
	op_cmpi_b<EA_INCR<1>>,
	op_cmpi_b<EA_DECR<1>>,
	op_cmpi_b<EA_D16>,
	op_cmpi_b<EA_EX>,
	op_cmpi_b<EA_EX2_R_B>,

	// 0000 1100 01 xxx
	op_cmpi_w<EA_DN>,
	ill_op,
	op_cmpi_w<EA_MEM>,
	op_cmpi_w<EA_INCR<2>>,
	op_cmpi_w<EA_DECR<2>>,
	op_cmpi_w<EA_D16>,
	op_cmpi_w<EA_EX>,
	op_cmpi_w<EA_EX2_R_W>,

	// 0000 1100 10 xxx
	op_cmpi_l<EA_DN>,
	ill_op,
	op_cmpi_l<EA_MEM>,
	op_cmpi_l<EA_INCR<4>>,
	op_cmpi_l<EA_DECR<4>>,
	op_cmpi_l<EA_D16>,
	op_cmpi_l<EA_EX>,
	op_cmpi_l<EA_EX2_R_L>,

	// 0000 1100 11 xxx
	ill_op,
	ill_op,
	ill_op,
	ill_op,
	ill_op,
	ill_op,
	ill_op,
	ill_op,

};
void m68k_reset (REGS* regs);
void m68k_execute() {
	m68k_reset(&regs);
	while(! quit_program) {
		uint16_t op = fetch_w(regs.PC);
		op_list[op>>3](&regs, op);
	}
}
void m68k_reset (REGS* regs)
{
	regs->ISP = regs->A[7] = 0x2000;
	regs->PC = ROMBaseMac + 0x2a;
	regs->S = true;
	regs->M = false;
	regs->T = 0;
	regs->Z = false;
	regs->X = false;
	regs->C = false;
	regs->V = false;
	regs->N = false;
//	SPCFLAGS_INIT( 0 );
//	regs.intmask = 7;
//	regs.vbr = regs.sfc = regs.dfc = 0;
//	fpu_reset();

}
