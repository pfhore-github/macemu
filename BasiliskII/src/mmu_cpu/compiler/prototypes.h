#include <asmjit/x86.h>
#include <stdint.h>
std::function<void()> compile_ori_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_ori_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_ori_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cmp2_chk2_b(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_andi_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_andi_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_andi_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cmp2_chk2_w(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_subi_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_subi_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_subi_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cmp2_chk2_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_addi_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_addi_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_addi_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_btst_imm(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bchg_imm(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bclr_imm(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bset_imm(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_eori_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_eori_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_eori_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cas_b(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_cmpi_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cmpi_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cmpi_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cas_w(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_moves_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_moves_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_moves_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cas_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_negx_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_negx_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_negx_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_move_from_sr(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_clr_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_clr_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_clr_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_move_from_ccr(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_neg_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_neg_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_neg_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_move_to_ccr(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_not_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_not_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_not_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_move_to_sr(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_nbcd(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_movem_to_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_movem_to_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_tst_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_tst_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_tst_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_tas(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_mul_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_div_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_movem_from_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_movem_from_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_0471(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_jsr(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_jmp(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_asr_ea(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_lsr_ea(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_roxr_ea(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_ror_ea(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_bftst(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bfchg(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bfset(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bfclr(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_asl_ea(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_lsl_ea(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_roxl_ea(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_rol_ea(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_bfextu(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bfexts(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bfffo(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bfins(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_btst_dm(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bchg_dm(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bclr_dm(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bset_dm(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_movea_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_movea_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_chk_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_chk_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_lea(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_addq_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_addq_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_addq_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_subq_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_subq_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_subq_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_or_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_or_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_or_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_divu_w(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_or_to_ea_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_or_to_ea_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_or_to_ea_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_divs_w(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_sub_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_sub_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_sub_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_suba_w(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_sub_to_ea_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_sub_to_ea_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_sub_to_ea_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_suba_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_cmp_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cmp_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cmp_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cmpa_w(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_eor_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_eor_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_eor_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_cmpa_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_and_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_and_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_and_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_mulu_w(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_and_to_ea_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_and_to_ea_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_and_to_ea_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_muls_w(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_add_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_add_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_add_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_adda_w(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_add_to_ea_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_add_to_ea_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_add_to_ea_l(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_adda_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_shr_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_shr_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_shr_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_shl_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_shl_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_shl_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_moveq(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_emul_op(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_move_b(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_move_w(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_move_l(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_scc(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bra(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_bsr(uint16_t op, int type, int reg, int dm);

std::function<void()> compile_bcc(uint16_t op, int type, int reg, int dm);
std::function<void()> compile_move16(uint16_t op, int type, int reg, int dm);
