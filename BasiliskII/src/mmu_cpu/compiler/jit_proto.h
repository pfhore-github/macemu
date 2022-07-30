#include <asmjit/x86.h>
#include <stdint.h>
void jit_ori_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_ori_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_ori_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cmp2_chk2_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_andi_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_andi_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_andi_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cmp2_chk2_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_subi_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_subi_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_subi_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cmp2_chk2_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_addi_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_addi_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_addi_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_btst_imm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_bchg_imm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_bclr_imm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_bset_imm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_eori_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_eori_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_eori_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cas_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cmpi_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cmpi_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cmpi_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cas_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_moves_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_moves_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_moves_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cas_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_negx_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_negx_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_negx_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_move_from_sr(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_clr_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_clr_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_clr_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_move_from_ccr(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_neg_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_neg_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_neg_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_move_to_ccr(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_not_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_not_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_not_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_move_to_sr(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_nbcd(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_movem_to_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_movem_to_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_tst_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_tst_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_tst_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_tas(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_mul_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_div_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_movem_from_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_movem_from_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_0471(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_jsr(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_jmp(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_asr_ea(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_lsr_ea(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_roxr_ea(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_ror_ea(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;

void jit_bftst(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_bfchg(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_bfset(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_bfclr(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;

void jit_asl_ea(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_lsl_ea(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_roxl_ea(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_rol_ea(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;

void jit_bfextu(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_bfexts(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_bfffo(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_bfins(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;

void jit_btst_dm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_bchg_dm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_bclr_dm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_bset_dm(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_movea_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_movea_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_chk_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_chk_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_addq_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_addq_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_addq_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_subq_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_subq_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_subq_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);


void jit_or_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_or_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_or_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_divu_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_or_to_ea_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_or_to_ea_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_or_to_ea_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_divs_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_sub_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_sub_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_sub_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_suba_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_sub_to_ea_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_sub_to_ea_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_sub_to_ea_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_suba_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_cmp_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cmp_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cmp_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_cmpa_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_cmpa_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_eor_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_eor_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_eor_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_and_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_and_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_and_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_mulu_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_and_to_ea_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_and_to_ea_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_and_to_ea_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_add_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_add_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_add_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_adda_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_add_to_ea_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_add_to_ea_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_add_to_ea_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_adda_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_muls_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_shr_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_shr_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_shr_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_shl_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_shl_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_shl_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_moveq(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_move_b(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_move_w(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);
void jit_move_l(uint16_t op, int type, int reg, asmjit::x86::Assembler &a);

void jit_scc(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_bra(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_bsr(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
void jit_bcc(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;

void jit_move16(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) ;
