OP(ori_b);
OP(ori_w);
OP(ori_l);
OP(andi_b);
OP(andi_w);
OP(andi_l);
OP(cmp2_chk2_b);
OP(cmp2_chk2_w);
OP(subi_b);
OP(subi_w);
OP(subi_l);
OP(cmp2_chk2_l);
OP(addi_b);
OP(addi_w);
OP(addi_l);
OP(btst_imm);
OP(bchg_imm);
OP(bclr_imm);
OP(bset_imm);
OP(eori_b);
OP(eori_w);
OP(eori_l);
OP(cas_b);
OP(cas_w);
OP(cas_l);
OP(cmpi_b);
OP(cmpi_w);
OP(cmpi_l);
OP(moves_b);
OP(moves_w);
OP(moves_l);
OP(btst_dm);
OP(bchg_dm);
OP(bclr_dm);
OP(bset_dm);
OP(move_b);
OP(move_w);
OP(move_l);
OP(movea_w);
OP(movea_l);
OP(negx_b);
OP(negx_w);
OP(negx_l);
OP(move_from_sr);
OP(clr_b);
OP(clr_w);
OP(clr_l);
OP(move_from_ccr);
OP(neg_b);
OP(neg_w);
OP(neg_l);
OP(move_to_ccr);
OP(not_b);
OP(not_w);
OP(not_l);
OP(move_to_sr);
OP(nbcd);
OP(0441);
OP(movem_to_w);
OP(movem_to_l);
OP(tst_b);
OP(tst_w);
OP(tst_l);
OP(tas);
OP(mul_l);
OP(div_l);
OP(movem_from_w);
OP(movem_from_l);
OP(0471);

OP(jsr);
OP(jmp);
OP(chk_w);
OP(chk_l);
OP(lea);
OP(addq_b);
OP(addq_w);
OP(addq_l);
OP(subq_b);
OP(subq_w);
OP(subq_l);
OP(scc);

OP(bra);
OP(bsr);
OP(bcc);
OP(moveq);
OP(or_b);
OP(or_w);
OP(or_l);
OP(or_to_ea_b);
OP(or_to_ea_w);
OP(or_to_ea_l);
OP(divu_w);
OP(divs_w);
OP(sub_b);
OP(sub_w);
OP(sub_l);
OP(suba_w);
OP(suba_l);
OP(sub_to_ea_b);
OP(sub_to_ea_w);
OP(sub_to_ea_l);
OP(cmp_b);
OP(cmp_w);
OP(cmp_l);
OP(cmpa_w);
OP(eor_b);
OP(eor_w);
OP(eor_l);
OP(cmpa_l);

OP(and_b);
OP(and_w);
OP(and_l);
OP(mulu_w);
OP(and_to_ea_b);
OP(and_to_ea_w);
OP(and_to_ea_l);
OP(muls_w);
OP(add_b);
OP(add_w);
OP(add_l);
OP(adda_w);
OP(adda_l);
OP(add_to_ea_b);
OP(add_to_ea_w);
OP(add_to_ea_l);
OP(shr_b);
OP(shr_w);
OP(shr_l);
OP(asr_ea);
OP(lsr_ea);
OP(roxr_ea);
OP(ror_ea);

OP(bftst);
OP(bfchg);
OP(bfclr);
OP(bfset);

OP(shl_b);
OP(shl_w);
OP(shl_l);

OP(asl_ea);
OP(lsl_ea);
OP(roxl_ea);
OP(rol_ea);
OP(bfextu);
OP(bfexts);
OP(bfffo);
OP(bfins);

OP(move16);

void op_movep_w_from(int dm, int reg);
void op_movep_l_from(int dm, int reg);
void op_movep_w_to(int dm, int reg);
void op_movep_l_to(int dm, int reg);
void op_link_l(int reg);

void op_swap(int reg);
void op_bkpt();
void op_pea(int type, int reg);

void op_ext_w(int reg);
void op_ext_l(int reg);

void op_trap(int vec);
void op_link_w(int reg);
void op_unlk(int reg);
void op_move_to_usp(int reg);
void op_move_from_usp(int reg);
void op_reset();
void op_nop();
void op_stop();
void op_rte();
void op_rtd();
void op_rts();
void op_trapv();
void op_rtr();
void op_movec_from();
void op_movec_to();

void op_extb(int reg) ;

void op_sbcd_d(int dm, int reg);
void op_sbcd_a(int dm, int reg);
void op_pack_d(int dm, int reg);
void op_pack_a(int dm, int reg);
void op_unpk_d(int dm, int reg);
void op_unpk_a(int dm, int reg);

void op_cmpm_b(int y, int x);
void op_cmpm_w(int y, int x);
void op_cmpm_l(int y, int x);

void op_abcd_d(int dm, int reg);
void op_abcd_a(int dm, int reg);

void op_exg_d(int dm, int reg);
void op_exg_a(int dm, int reg);
void op_exg_da(int dm, int reg);
