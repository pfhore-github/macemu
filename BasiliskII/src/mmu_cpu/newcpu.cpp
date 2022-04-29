/*
 * UAE - The Un*x Amiga Emulator
 *
 * MC68000 emulation
 *
 * (c) 1995 Bernd Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "cpu_emulation.h"
#include "emul_op.h"
#include "intop.h"
#include "main.h"
#include "sysdeps.h"
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <unordered_map>
#include <vector>

#include "ea.h"
#include "exception.h"
#include "fpu/fpu_ieee.h"
#include "newcpu.h"
#include "op.h"
m68k_reg regs;
void SAVE_SP() {
    if(!regs.S) {
        regs.usp = regs.a[7];
    } else if(!regs.M) {
        regs.isp = regs.a[7];
    } else {
        regs.msp = regs.a[7];
    }
}
void LOAD_SP() {
    if(!regs.S) {
        regs.a[7] = regs.usp;
    } else if(!regs.M) {
        regs.a[7] = regs.isp;
    } else {
        regs.a[7] = regs.msp;
    }
}

struct sr_t {
    bool C : 1;
    bool V : 1;
    bool Z : 1;
    bool N : 1;
    bool X : 1;
    int : 3;
    unsigned IM : 3;
    int : 1;
    bool M : 1;
    bool S : 1;
    unsigned T : 2;
    unsigned : 16;
};


uint8_t GET_CCR() {
    return regs.c | regs.v << 1 | regs.z << 2 | regs.n << 3 | regs.x << 4;
}

void SET_CCR(uint8_t v) {
    regs.c = v & 1;
    regs.v = v >> 1 & 1;
    regs.z = v >> 2 & 1;
    regs.n = v >> 3 & 1;
    regs.x = v >> 4 & 1;
}


uint16_t GET_SR() {
    return GET_CCR() |
    regs.IM << 8 | regs.M << 12 | regs.S << 13 | regs.T << 14;
}

void SET_SR(uint16_t v) {
    SAVE_SP();
    SET_CCR(v);
    regs.IM = v >> 8 & 7;
    regs.M = v >> 12 & 1;
    regs.S = v >> 13 & 1;
    regs.T = v >> 14 & 3;
    LOAD_SP();
}

OP(pea) {
    regs.i_ea = EA_Addr(type, reg, 0, false);
    PUSH32(regs.i_ea);
}

OP(illegal) { ILLEGAL_INST(); }

OP(nop) {}

OP(lea) {
    regs.i_ea = EA_Addr(type, reg, 0, false);
    regs.a[dm] = regs.i_ea;
}

OP(aline) { ALINE_EXCEPTION(); }
void EmulOp(uint16 opcode, M68kRegisters *r);
OP(emul_op) {
    M68kRegisters rr;
    memcpy(&rr.d, &regs.d, sizeof(uint32_t) * 8);
    memcpy(&rr.a, &regs.a, sizeof(uint32_t) * 8);
    rr.sr = GET_SR();
    EmulOp(xop, &rr);
    SET_SR(rr.sr);
    memcpy(&regs.d, &rr.d, sizeof(uint32_t) * 8);
    memcpy(&regs.a, &rr.a, sizeof(uint32_t) * 8);
}
using op_t = void (*)(uint16_t, int, int, int);
op_t opc_map[65536];
static void build_cpufunctbl() {

    // EA=DR
    for(int i = 0; i < 8; ++i) {
        opc_map[0000000 | i] = op_ori_b;
        opc_map[0000100 | i] = op_ori_w;
        opc_map[0000200 | i] = op_ori_l;
        opc_map[0001000 | i] = op_andi_b;
        opc_map[0001100 | i] = op_andi_w;
        opc_map[0001200 | i] = op_andi_l;
        opc_map[0002000 | i] = op_subi_b;
        opc_map[0002100 | i] = op_subi_w;
        opc_map[0002200 | i] = op_subi_l;
        opc_map[0003000 | i] = op_addi_b;
        opc_map[0003100 | i] = op_addi_w;
        opc_map[0003200 | i] = op_addi_l;

        opc_map[0004000 | i] = op_btst_l_imm;
        opc_map[0004100 | i] = op_bchg_l_imm;
        opc_map[0004200 | i] = op_bclr_l_imm;
        opc_map[0004300 | i] = op_bset_l_imm;

        opc_map[0005000 | i] = op_eori_b;
        opc_map[0005100 | i] = op_eori_w;
        opc_map[0005200 | i] = op_eori_l;

        opc_map[0006000 | i] = op_cmpi_b;
        opc_map[0006100 | i] = op_cmpi_w;
        opc_map[0006200 | i] = op_cmpi_l;

        for(int dn = 0; dn < 8; ++dn) {
            opc_map[0000400 | dn << 9 | i] = op_btst_l_dm;
            opc_map[0000500 | dn << 9 | i] = op_bchg_l_dm;
            opc_map[0000600 | dn << 9 | i] = op_bclr_l_dm;
            opc_map[0000700 | dn << 9 | i] = op_bset_l_dm;

            opc_map[0040400 | dn << 9 | i] = op_chk_l;
            opc_map[0040600 | dn << 9 | i] = op_chk_w;
            opc_map[0040700 | dn << 9 | i] = op_extb;

            opc_map[0050000 | dn << 9 | i] = op_addq_b;
            opc_map[0050100 | dn << 9 | i] = op_addq_w;
            opc_map[0050200 | dn << 9 | i] = op_addq_l;

            opc_map[0050400 | dn << 9 | i] = op_subq_b;
            opc_map[0050500 | dn << 9 | i] = op_subq_w;
            opc_map[0050600 | dn << 9 | i] = op_subq_l;

            for(int xg = 0; xg < 8; ++xg) {
                if(xg != 1) {
                    opc_map[0010000 | xg << 6 | dn << 9 | i] = op_move_b;
                    opc_map[0020000 | xg << 6 | dn << 9 | i] = op_move_l;
                    opc_map[0030000 | xg << 6 | dn << 9 | i] = op_move_w;
                } else {
                    opc_map[0020100 | dn << 9 | i] = op_movea_l;
                    opc_map[0030100 | dn << 9 | i] = op_movea_w;
                }
            }

            opc_map[0100000 | dn << 9 | i] = op_or_b;
            opc_map[0100100 | dn << 9 | i] = op_or_w;
            opc_map[0100200 | dn << 9 | i] = op_or_l;
            opc_map[0100300 | dn << 9 | i] = op_divu_w;

            opc_map[0100400 | dn << 9 | i] = op_sbcd_d;
            opc_map[0100500 | dn << 9 | i] = op_pack_d;
            opc_map[0100600 | dn << 9 | i] = op_unpk_d;
            opc_map[0100700 | dn << 9 | i] = op_divs_w;

            opc_map[0110000 | dn << 9 | i] = op_sub_b;
            opc_map[0110100 | dn << 9 | i] = op_sub_w;
            opc_map[0110200 | dn << 9 | i] = op_sub_l;
            opc_map[0110300 | dn << 9 | i] = op_suba_w;

            opc_map[0110400 | dn << 9 | i] = op_subx_b_d;
            opc_map[0110500 | dn << 9 | i] = op_subx_w_d;
            opc_map[0110600 | dn << 9 | i] = op_subx_l_d;
            opc_map[0110700 | dn << 9 | i] = op_suba_l;

            opc_map[0130000 | dn << 9 | i] = op_cmp_b;
            opc_map[0130100 | dn << 9 | i] = op_cmp_w;
            opc_map[0130200 | dn << 9 | i] = op_cmp_l;
            opc_map[0130300 | dn << 9 | i] = op_cmpa_w;

            opc_map[0130400 | dn << 9 | i] = op_eor_b;
            opc_map[0130500 | dn << 9 | i] = op_eor_w;
            opc_map[0130600 | dn << 9 | i] = op_eor_l;
            opc_map[0130700 | dn << 9 | i] = op_cmpa_l;

            opc_map[0140000 | dn << 9 | i] = op_and_b;
            opc_map[0140100 | dn << 9 | i] = op_and_w;
            opc_map[0140200 | dn << 9 | i] = op_and_l;
            opc_map[0140300 | dn << 9 | i] = op_mulu_w;
            opc_map[0140400 | dn << 9 | i] = op_abcd_d;
            opc_map[0140500 | dn << 9 | i] = op_exg_d;
            opc_map[0140700 | dn << 9 | i] = op_muls_w;

            opc_map[0150000 | dn << 9 | i] = op_add_b;
            opc_map[0150100 | dn << 9 | i] = op_add_w;
            opc_map[0150200 | dn << 9 | i] = op_add_l;
            opc_map[0150300 | dn << 9 | i] = op_adda_w;

            opc_map[0150400 | dn << 9 | i] = op_addx_b_d;
            opc_map[0150500 | dn << 9 | i] = op_addx_w_d;
            opc_map[0150600 | dn << 9 | i] = op_addx_l_d;
            opc_map[0150700 | dn << 9 | i] = op_adda_l;

            opc_map[0160000 | dn << 9 | i] = op_asr_b_imm;
            opc_map[0160010 | dn << 9 | i] = op_lsr_b_imm;
            opc_map[0160020 | dn << 9 | i] = op_roxr_b_imm;
            opc_map[0160030 | dn << 9 | i] = op_ror_b_imm;
            opc_map[0160040 | dn << 9 | i] = op_asr_b_reg;
            opc_map[0160050 | dn << 9 | i] = op_lsr_b_reg;
            opc_map[0160060 | dn << 9 | i] = op_roxr_b_reg;
            opc_map[0160070 | dn << 9 | i] = op_ror_b_reg;

            opc_map[0160100 | dn << 9 | i] = op_asr_w_imm;
            opc_map[0160110 | dn << 9 | i] = op_lsr_w_imm;
            opc_map[0160120 | dn << 9 | i] = op_roxr_w_imm;
            opc_map[0160130 | dn << 9 | i] = op_ror_w_imm;
            opc_map[0160140 | dn << 9 | i] = op_asr_w_reg;
            opc_map[0160150 | dn << 9 | i] = op_lsr_w_reg;
            opc_map[0160160 | dn << 9 | i] = op_roxr_w_reg;
            opc_map[0160170 | dn << 9 | i] = op_ror_w_reg;

            opc_map[0160200 | dn << 9 | i] = op_asr_l_imm;
            opc_map[0160210 | dn << 9 | i] = op_lsr_l_imm;
            opc_map[0160220 | dn << 9 | i] = op_roxr_l_imm;
            opc_map[0160230 | dn << 9 | i] = op_ror_l_imm;
            opc_map[0160240 | dn << 9 | i] = op_asr_l_reg;
            opc_map[0160250 | dn << 9 | i] = op_lsr_l_reg;
            opc_map[0160260 | dn << 9 | i] = op_roxr_l_reg;
            opc_map[0160270 | dn << 9 | i] = op_ror_l_reg;

            opc_map[0160400 | dn << 9 | i] = op_asl_b_imm;
            opc_map[0160410 | dn << 9 | i] = op_lsl_b_imm;
            opc_map[0160420 | dn << 9 | i] = op_roxl_b_imm;
            opc_map[0160430 | dn << 9 | i] = op_rol_b_imm;
            opc_map[0160440 | dn << 9 | i] = op_asl_b_reg;
            opc_map[0160450 | dn << 9 | i] = op_lsl_b_reg;
            opc_map[0160460 | dn << 9 | i] = op_roxl_b_reg;
            opc_map[0160470 | dn << 9 | i] = op_rol_b_reg;

            opc_map[0160500 | dn << 9 | i] = op_asl_w_imm;
            opc_map[0160510 | dn << 9 | i] = op_lsl_w_imm;
            opc_map[0160520 | dn << 9 | i] = op_roxl_w_imm;
            opc_map[0160530 | dn << 9 | i] = op_rol_w_imm;
            opc_map[0160540 | dn << 9 | i] = op_asl_w_reg;
            opc_map[0160550 | dn << 9 | i] = op_lsl_w_reg;
            opc_map[0160560 | dn << 9 | i] = op_roxl_w_reg;
            opc_map[0160570 | dn << 9 | i] = op_rol_w_reg;

            opc_map[0160600 | dn << 9 | i] = op_asl_l_imm;
            opc_map[0160610 | dn << 9 | i] = op_lsl_l_imm;
            opc_map[0160620 | dn << 9 | i] = op_roxl_l_imm;
            opc_map[0160630 | dn << 9 | i] = op_rol_l_imm;
            opc_map[0160640 | dn << 9 | i] = op_asl_l_reg;
            opc_map[0160650 | dn << 9 | i] = op_lsl_l_reg;
            opc_map[0160660 | dn << 9 | i] = op_roxl_l_reg;
            opc_map[0160670 | dn << 9 | i] = op_rol_l_reg;
        }

        opc_map[0040000 | i] = op_negx_b;
        opc_map[0040100 | i] = op_negx_w;
        opc_map[0040200 | i] = op_negx_l;
        opc_map[0040300 | i] = op_move_from_sr;

        opc_map[0041000 | i] = op_clr_b;
        opc_map[0041100 | i] = op_clr_w;
        opc_map[0041200 | i] = op_clr_l;
        opc_map[0041300 | i] = op_move_from_ccr;

        opc_map[0042000 | i] = op_neg_b;
        opc_map[0042100 | i] = op_neg_w;
        opc_map[0042200 | i] = op_neg_l;
        opc_map[0042300 | i] = op_move_to_ccr;

        opc_map[0043000 | i] = op_not_b;
        opc_map[0043100 | i] = op_not_w;
        opc_map[0043200 | i] = op_not_l;
        opc_map[0043300 | i] = op_move_to_sr;

        opc_map[0044000 | i] = op_nbcd;
        opc_map[0044100 | i] = op_swap;
        opc_map[0044200 | i] = op_ext_w;
        opc_map[0044300 | i] = op_ext_l;

        opc_map[0045000 | i] = op_tst_b;
        opc_map[0045100 | i] = op_tst_w;
        opc_map[0045200 | i] = op_tst_l;
        opc_map[0045300 | i] = op_tas;

        opc_map[0046000 | i] = op_mul_l;
        opc_map[0046100 | i] = op_div_l;

        opc_map[0047100 | i] = op_trap;

        opc_map[0164300 | i] = op_bftst_d;
        opc_map[0165300 | i] = op_bfchg_d;
        opc_map[0166300 | i] = op_bfclr_d;
        opc_map[0167300 | i] = op_bfset_d;

        opc_map[0164700 | i] = op_bfextu_d;
        opc_map[0165700 | i] = op_bfexts_d;
        opc_map[0166700 | i] = op_bfffo_d;
        opc_map[0167700 | i] = op_bfins_d;

        for(int cd = 0; cd < 16; ++cd) {
            opc_map[0050300 | cd << 8 | i] = op_scc;
        }
    }

    // EA=AR
    for(int an = 0; an < 8; ++an) {
        for(int dn = 0; dn < 8; ++dn) {
            opc_map[0000410 | dn << 9 | an] = op_movep_w_from;
            opc_map[0000510 | dn << 9 | an] = op_movep_l_from;
            opc_map[0000610 | dn << 9 | an] = op_movep_w_to;
            opc_map[0000710 | dn << 9 | an] = op_movep_l_to;

            opc_map[0050110 | dn << 9 | an] = op_addqa_w;
            opc_map[0050210 | dn << 9 | an] = op_addqa_l;

            opc_map[0050510 | dn << 9 | an] = op_subqa_w;
            opc_map[0050610 | dn << 9 | an] = op_subqa_l;

            opc_map[0100410 | dn << 9 | an] = op_sbcd_a;
            opc_map[0100510 | dn << 9 | an] = op_pack_a;
            opc_map[0100610 | dn << 9 | an] = op_unpk_a;

            opc_map[0110110 | dn << 9 | an] = op_sub_w;
            opc_map[0110210 | dn << 9 | an] = op_sub_l;
            opc_map[0110310 | dn << 9 | an] = op_suba_w;

            opc_map[0110410 | dn << 9 | an] = op_subx_b_a;
            opc_map[0110510 | dn << 9 | an] = op_subx_w_a;
            opc_map[0110610 | dn << 9 | an] = op_subx_l_a;
            opc_map[0110710 | dn << 9 | an] = op_suba_l;

            opc_map[0130010 | dn << 9 | an] = op_cmp_b;
            opc_map[0130110 | dn << 9 | an] = op_cmp_w;
            opc_map[0130210 | dn << 9 | an] = op_cmp_l;
            opc_map[0130310 | dn << 9 | an] = op_cmpa_w;
            opc_map[0130710 | dn << 9 | an] = op_cmpa_l;

            opc_map[0140410 | dn << 9 | an] = op_abcd_a;
            opc_map[0140510 | dn << 9 | an] = op_exg_a;
            opc_map[0140610 | dn << 9 | an] = op_exg_da;

            opc_map[0150110 | dn << 9 | an] = op_add_w;
            opc_map[0150210 | dn << 9 | an] = op_add_l;
            opc_map[0150310 | dn << 9 | an] = op_adda_w;

            opc_map[0150410 | dn << 9 | an] = op_addx_b_a;
            opc_map[0150510 | dn << 9 | an] = op_addx_w_a;
            opc_map[0150610 | dn << 9 | an] = op_addx_l_a;
            opc_map[0150710 | dn << 9 | an] = op_adda_l;
        }
        opc_map[0044010 | an] = op_link_l;
        opc_map[0044110 | an] = op_bkpt;

        opc_map[0045110 | an] = op_tst_w;
        opc_map[0045210 | an] = op_tst_l;
        opc_map[0047110 | an] = op_trap;
        opc_map[0047120 | an] = op_link_w;
        opc_map[0047130 | an] = op_unlk;
        opc_map[0047140 | an] = op_move_to_usp;
        opc_map[0047150 | an] = op_move_from_usp;

        for(int cd = 0; cd < 16; ++cd) {
            opc_map[0050310 | cd << 8 | an] = op_dbcc;
        }
    }

    // EA=OTHER
    for(int i = 020; i < 074; ++i) {
        opc_map[0000000 | i] = op_ori_b;
        opc_map[0000100 | i] = op_ori_w;
        opc_map[0000200 | i] = op_ori_l;
        opc_map[0000300 | i] = op_cmp2_chk2_b;

        opc_map[0001000 | i] = op_andi_b;
        opc_map[0001100 | i] = op_andi_w;
        opc_map[0001200 | i] = op_andi_l;
        opc_map[0001300 | i] = op_cmp2_chk2_w;

        opc_map[0002000 | i] = op_subi_b;
        opc_map[0002100 | i] = op_subi_w;
        opc_map[0002200 | i] = op_subi_l;
        opc_map[0002300 | i] = op_cmp2_chk2_l;

        opc_map[0003000 | i] = op_addi_b;
        opc_map[0003100 | i] = op_addi_w;
        opc_map[0003200 | i] = op_addi_l;

        opc_map[0004000 | i] = op_btst_b_imm;
        opc_map[0004100 | i] = op_bchg_b_imm;
        opc_map[0004200 | i] = op_bclr_b_imm;
        opc_map[0004300 | i] = op_bset_b_imm;

        opc_map[0005000 | i] = op_eori_b;
        opc_map[0005100 | i] = op_eori_w;
        opc_map[0005200 | i] = op_eori_l;
        opc_map[0005300 | i] = op_cas_b;

        opc_map[0006000 | i] = op_cmpi_b;
        opc_map[0006100 | i] = op_cmpi_w;
        opc_map[0006200 | i] = op_cmpi_l;
        opc_map[0006300 | i] = op_cas_w;

        opc_map[0007000 | i] = op_moves_b;
        opc_map[0007100 | i] = op_moves_w;
        opc_map[0007200 | i] = op_moves_l;
        opc_map[0007300 | i] = op_cas_l;

        opc_map[0040000 | i] = op_negx_b;
        opc_map[0040100 | i] = op_negx_w;
        opc_map[0040200 | i] = op_negx_l;
        opc_map[0040300 | i] = op_move_from_sr;

        opc_map[0041000 | i] = op_clr_b;
        opc_map[0041100 | i] = op_clr_w;
        opc_map[0041200 | i] = op_clr_l;
        opc_map[0041300 | i] = op_move_from_ccr;

        opc_map[0042000 | i] = op_neg_b;
        opc_map[0042100 | i] = op_neg_w;
        opc_map[0042200 | i] = op_neg_l;
        opc_map[0042300 | i] = op_move_to_ccr;

        opc_map[0043000 | i] = op_not_b;
        opc_map[0043100 | i] = op_not_w;
        opc_map[0043200 | i] = op_not_l;
        opc_map[0043300 | i] = op_move_to_sr;

        opc_map[0044000 | i] = op_nbcd;
        opc_map[0044100 | i] = op_pea;
        opc_map[0044200 | i] = op_movem_to_w;
        opc_map[0044300 | i] = op_movem_to_l;

        opc_map[0045000 | i] = op_tst_b;
        opc_map[0045100 | i] = op_tst_w;
        opc_map[0045200 | i] = op_tst_l;
        opc_map[0045300 | i] = op_tas;

        opc_map[0046000 | i] = op_mul_l;
        opc_map[0046100 | i] = op_div_l;
        opc_map[0046200 | i] = op_movem_from_w;
        opc_map[0046300 | i] = op_movem_from_l;

        opc_map[0047200 | i] = op_jsr;
        opc_map[0047300 | i] = op_jmp;

        opc_map[0160300 | i] = op_asr_ea;
        opc_map[0161300 | i] = op_lsr_ea;
        opc_map[0162300 | i] = op_roxr_ea;
        opc_map[0163300 | i] = op_ror_ea;

        opc_map[0160700 | i] = op_asl_ea;
        opc_map[0161700 | i] = op_lsl_ea;
        opc_map[0162700 | i] = op_roxl_ea;
        opc_map[0163700 | i] = op_rol_ea;

        opc_map[0164300 | i] = op_bftst_m;
        opc_map[0165300 | i] = op_bfchg_m;
        opc_map[0166300 | i] = op_bfclr_m;
        opc_map[0167300 | i] = op_bfset_m;

        opc_map[0164700 | i] = op_bfextu_m;
        opc_map[0165700 | i] = op_bfexts_m;
        opc_map[0166700 | i] = op_bfffo_m;
        opc_map[0167700 | i] = op_bfins_m;

        for(int dn = 0; dn < 8; ++dn) {
            opc_map[0000400 | dn << 9 | i] = op_btst_b_dm;
            opc_map[0000500 | dn << 9 | i] = op_bchg_b_dm;
            opc_map[0000600 | dn << 9 | i] = op_bclr_b_dm;
            opc_map[0000700 | dn << 9 | i] = op_bset_b_dm;

            opc_map[0040400 | dn << 9 | i] = op_chk_l;
            opc_map[0040600 | dn << 9 | i] = op_chk_w;
            opc_map[0040700 | dn << 9 | i] = op_lea;

            opc_map[0050000 | dn << 9 | i] = op_addq_b;
            opc_map[0050100 | dn << 9 | i] = op_addq_w;
            opc_map[0050200 | dn << 9 | i] = op_addq_l;

            opc_map[0050400 | dn << 9 | i] = op_subq_b;
            opc_map[0050500 | dn << 9 | i] = op_subq_w;
            opc_map[0050600 | dn << 9 | i] = op_subq_l;

            opc_map[0100000 | dn << 9 | i] = op_or_b;
            opc_map[0100100 | dn << 9 | i] = op_or_w;
            opc_map[0100200 | dn << 9 | i] = op_or_l;
            opc_map[0100300 | dn << 9 | i] = op_divu_w;

            opc_map[0100400 | dn << 9 | i] = op_or_to_ea_b;
            opc_map[0100500 | dn << 9 | i] = op_or_to_ea_w;
            opc_map[0100600 | dn << 9 | i] = op_or_to_ea_l;
            opc_map[0100700 | dn << 9 | i] = op_divs_w;

            opc_map[0110000 | dn << 9 | i] = op_sub_b;
            opc_map[0110100 | dn << 9 | i] = op_sub_w;
            opc_map[0110200 | dn << 9 | i] = op_sub_l;
            opc_map[0110300 | dn << 9 | i] = op_suba_w;

            opc_map[0110400 | dn << 9 | i] = op_sub_to_ea_b;
            opc_map[0110500 | dn << 9 | i] = op_sub_to_ea_w;
            opc_map[0110600 | dn << 9 | i] = op_sub_to_ea_l;
            opc_map[0110700 | dn << 9 | i] = op_suba_l;

            opc_map[0130000 | dn << 9 | i] = op_cmp_b;
            opc_map[0130100 | dn << 9 | i] = op_cmp_w;
            opc_map[0130200 | dn << 9 | i] = op_cmp_l;
            opc_map[0130300 | dn << 9 | i] = op_cmpa_w;

            opc_map[0130400 | dn << 9 | i] = op_eor_b;
            opc_map[0130500 | dn << 9 | i] = op_eor_w;
            opc_map[0130600 | dn << 9 | i] = op_eor_l;
            opc_map[0130700 | dn << 9 | i] = op_cmpa_l;

            opc_map[0140000 | dn << 9 | i] = op_and_b;
            opc_map[0140100 | dn << 9 | i] = op_and_w;
            opc_map[0140200 | dn << 9 | i] = op_and_l;
            opc_map[0140300 | dn << 9 | i] = op_mulu_w;
            opc_map[0140400 | dn << 9 | i] = op_and_to_ea_b;
            opc_map[0140500 | dn << 9 | i] = op_and_to_ea_w;
            opc_map[0140600 | dn << 9 | i] = op_and_to_ea_l;
            opc_map[0140700 | dn << 9 | i] = op_muls_w;

            opc_map[0150000 | dn << 9 | i] = op_add_b;
            opc_map[0150100 | dn << 9 | i] = op_add_w;
            opc_map[0150200 | dn << 9 | i] = op_add_l;
            opc_map[0150300 | dn << 9 | i] = op_adda_w;

            opc_map[0150400 | dn << 9 | i] = op_add_to_ea_b;
            opc_map[0150500 | dn << 9 | i] = op_add_to_ea_w;
            opc_map[0150600 | dn << 9 | i] = op_add_to_ea_l;
            opc_map[0150700 | dn << 9 | i] = op_adda_l;

            for(int xg = 0; xg < 8; ++xg) {
                if(xg != 1) {
                    opc_map[0010000 | xg << 6 | dn << 9 | i] = op_move_b;
                    opc_map[0020000 | xg << 6 | dn << 9 | i] = op_move_l;
                    opc_map[0030000 | xg << 6 | dn << 9 | i] = op_move_w;
                } else {
                    opc_map[0020100 | dn << 9 | i] = op_movea_l;
                    opc_map[0030100 | dn << 9 | i] = op_movea_w;
                }
            }
        }

        for(int cd = 0; cd < 16; ++cd) {
            opc_map[0050300 | cd << 8 | i] = op_scc;
        }
    }

    // EA=IMM
    opc_map[0000074] = op_ori_b_ccr;
    opc_map[0000174] = op_ori_w_sr;
    opc_map[0001074] = op_andi_b_ccr;
    opc_map[0001174] = op_andi_w_sr;
    opc_map[0005074] = op_eori_b_ccr;
    opc_map[0005174] = op_eori_w_sr;
    opc_map[0006374] = op_cas2_w;
    opc_map[0007374] = op_cas2_l;
    opc_map[0043374] = op_move_to_sr;
    opc_map[0045374] = op_illegal;
    opc_map[0047160] = op_reset;
    opc_map[0047161] = op_nop;
    opc_map[0047162] = op_stop;
    opc_map[0047163] = op_rte;
    opc_map[0047164] = op_rtd;
    opc_map[0047165] = op_rts;
    opc_map[0047166] = op_trapv;
    opc_map[0047167] = op_rtr;
    opc_map[0047172] = op_movec_from;
    opc_map[0047173] = op_movec_to;
    for(int dn = 0; dn < 8; ++dn) {
        opc_map[0040473 | dn << 9] = op_chk_l;
        opc_map[0040673 | dn << 9] = op_chk_w;
        for(int i = 0; i < 256; ++i) {
            opc_map[0070000 | dn << 9 | i] = op_moveq;
        }
        for(int xg = 0; xg < 8; ++xg) {
            if(xg != 1) {
                opc_map[0010074 | xg << 6 | dn << 9] = op_move_b;
                opc_map[0020074 | xg << 6 | dn << 9] = op_move_l;
                opc_map[0030074 | xg << 6 | dn << 9] = op_move_w;
            } else {
                opc_map[0020174 | dn << 9] = op_movea_l;
                opc_map[0030174 | dn << 9] = op_movea_w;
            }
        }
        opc_map[0100074 | dn << 9] = op_or_b;
        opc_map[0100174 | dn << 9] = op_or_w;
        opc_map[0100274 | dn << 9] = op_or_l;

        opc_map[0110074 | dn << 9] = op_sub_b;
        opc_map[0110174 | dn << 9] = op_sub_w;
        opc_map[0110274 | dn << 9] = op_sub_l;
        opc_map[0110374 | dn << 9] = op_suba_w;
        opc_map[0110774 | dn << 9] = op_suba_l;

        opc_map[0130074 | dn << 9] = op_cmp_b;
        opc_map[0130174 | dn << 9] = op_cmp_w;
        opc_map[0130274 | dn << 9] = op_cmp_l;
        opc_map[0130374 | dn << 9] = op_cmpa_w;
        opc_map[0130774 | dn << 9] = op_cmpa_l;

        opc_map[0140074 | dn << 9] = op_and_b;
        opc_map[0140174 | dn << 9] = op_and_w;
        opc_map[0140274 | dn << 9] = op_and_l;
        opc_map[0140374 | dn << 9] = op_mulu_w;
        opc_map[0140774 | dn << 9] = op_muls_w;

        opc_map[0150074 | dn << 9] = op_add_b;
        opc_map[0150174 | dn << 9] = op_add_w;
        opc_map[0150274 | dn << 9] = op_add_l;
        opc_map[0150374 | dn << 9] = op_adda_w;
        opc_map[0150774 | dn << 9] = op_adda_l;

        for(int cd = 0; cd < 16; ++cd) {
            opc_map[0050372 | cd << 8 ] = op_trapcc;
            opc_map[0050373 | cd << 8 ] = op_trapcc;
            opc_map[0050374 | cd << 8 ] = op_trapcc;
        }
    }

    for(int imm = 0; imm < 256; ++imm) {
        opc_map[0060000 | 0 << 8 | imm] = op_bra;
        opc_map[0060000 | 1 << 8 | imm] = op_bsr;
        for(int cd = 2; cd < 16; ++cd) {
            opc_map[0060000 | cd << 8 | imm] = op_bcc;
        }
        opc_map[0070400 | imm] = op_emul_op;
    }
    for(int i = 0; i < 0x1000; ++i) {
        opc_map[0xa000 | i] = op_aline;
    }

    init_fpu_opc();
    init_mmu_opc();

    for(int i = 0; i < 8; ++i ) {
        opc_map[0173000 | i] = op_move16_inc_to_imm;
        opc_map[0173010 | i] = op_move16_imm_to_inc;
        opc_map[0173020 | i] = op_move16_addr_to_imm;
        opc_map[0173030 | i] = op_move16_imm_to_addr;
        opc_map[0173040 | i] = op_move16_inc_to_inc;
    }
}

void init_m68k() {
    build_cpufunctbl();
}

void exit_m68k(void) {
}


#define get_ibyte_1(o) get_byte(get_virtual_address(regs.pc) + (o) + 1)
#define get_iword_1(o) get_word(get_virtual_address(regs.pc) + (o))
#define get_ilong_1(o) get_long(get_virtual_address(regs.pc) + (o))

#if 0
int m68k_do_specialties (void)
{
#if USE_JIT
	// Block was compiled
	SPCFLAGS_CLEAR( SPCFLAG_JIT_END_COMPILE );

	// Retain the request to get out of compiled code until
	// we reached the toplevel execution, i.e. the one that
	// can compile then run compiled code. This also means
	// we processed all (nested) EmulOps
	if ((m68k_execute_depth == 0) && SPCFLAGS_TEST( SPCFLAG_JIT_EXEC_RETURN ))
		SPCFLAGS_CLEAR( SPCFLAG_JIT_EXEC_RETURN );
#endif

	if (SPCFLAGS_TEST( SPCFLAG_DOTRACE )) {
		Exception (9,last_trace_ad);
	}
	while (SPCFLAGS_TEST( SPCFLAG_STOP )) {
		if (SPCFLAGS_TEST( SPCFLAG_INT | SPCFLAG_DOINT )){
			SPCFLAGS_CLEAR( SPCFLAG_INT | SPCFLAG_DOINT );
			int intr = intlev ();
			if (intr != -1 && intr > regs.intmask) {
				Interrupt (intr);
				regs.stopped = 0;
				SPCFLAGS_CLEAR( SPCFLAG_STOP );
			}
		}
	}
	if (SPCFLAGS_TEST( SPCFLAG_TRACE ))
		do_trace ();

	if (SPCFLAGS_TEST( SPCFLAG_DOINT )) {
		SPCFLAGS_CLEAR( SPCFLAG_DOINT );
		int intr = intlev ();
		if (intr != -1 && intr > regs.intmask) {
			Interrupt (intr);
			regs.stopped = 0;
		}
	}
	if (SPCFLAGS_TEST( SPCFLAG_INT )) {
		SPCFLAGS_CLEAR( SPCFLAG_INT );
		SPCFLAGS_SET( SPCFLAG_DOINT );
	}
	if (SPCFLAGS_TEST( SPCFLAG_BRK )) {
		SPCFLAGS_CLEAR( SPCFLAG_BRK );
		return 1;
	}
	return 0;
}
#endif
// run one opcode
void m68k_do_execute() {
    if(setjmp(ex_jmp) == 0) {
        regs.opc = regs.pc;
        if(regs.pc & 1) {
            ADDRESS_ERROR();
            goto EXCEPTION;
        }
        uint16_t opc = FETCH();
        auto f = opc_map[opc];
        if(f) {
            f(opc, opc >> 9 & 7, opc >> 3 & 7, opc & 7);
        } else {
            ILLEGAL_INST();
        }        
    }
EXCEPTION:
    // TRACE
    if(regs.T == 2 || regs.T == 1 && regs.traced) {
        TRACE();
    }

    // IRQ
    for(int i = 0; i < 8; ++i) {
        if(regs.irq & (1 << i)) {
            regs.irq &= ~(1 << i);
            IRQ(i);
            break;
        }
    }
    regs.traced = false;
}

void m68k_execute(void) {
    regs.i_ea = regs.i_eav = 0;
    std::thread cpu_thread([] {
        for(;;) {
            m68k_do_execute();
        }
    });
    cpu_thread.detach();
}
