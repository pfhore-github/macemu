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

#include "compiler/compiler.h"
#include "compiler/prototypes.h"
#include "ea.h"
#include "ex_stack.h"
#include "exception.h"
#include "fpu/fpu_mpfr.h"
#include "newcpu.h"

using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536 >> 6];
using compile_t = std::function<void()>(uint16_t, int, int, int);
compile_t *compile_map[65536 >> 6];
std::unordered_map<uint32_t, JIT_OP> compiled;
void op_compile(uint32_t p) {
    regs.pc = p;
    uint16_t o = FETCH();
    if(compile_map[o >> 6]) {
        try {
            compiled[p].exec =
                compile_map[o >> 6](o, o >> 3 & 7, o & 7, o >> 9 & 7);
        } catch(ILLEGAL_INST_EX) {
            compiled[p].exec = ILLEGAL_INST;
        }
    } else {
        compiled[p].exec = ILLEGAL_INST;
    }
    compiled[p].nextop = regs.pc;
}
std::function<void()> compile_0441(uint16_t op, int type, int reg, int dm) {
    if(type == 0) {
        // SWAP
        return [reg]() {
            uint32_t v = regs.d[reg];
            v = (v >> 16) | (v << 16);
            TEST_NZ32(v);
            regs.v = regs.c = false;
            regs.d[reg] = v;
        };
    } else if(type == 1) {
        // BPKT
        return ILLEGAL_INST;
    } else {
        return [f = ea_addr(type, reg, 0, false)]() {
            regs.i_ea = f();
            PUSH32(regs.i_ea);
        };
    }
}
void EmulOp(uint16_t opcode, M68kRegisters *r);
std::function<void()> compile_emul_op(uint16_t op, int type, int reg, int dm) {
    return [op]() {
        M68kRegisters rr;
        memcpy(&rr.d, &regs.d, sizeof(uint32_t) * 8);
        memcpy(&rr.a, &regs.a, sizeof(uint32_t) * 8);
        rr.sr = GET_SR();
        EmulOp(op, &rr);
        SET_SR(rr.sr);
        memcpy(&regs.d, &rr.d, sizeof(uint32_t) * 8);
        memcpy(&regs.a, &rr.a, sizeof(uint32_t) * 8);
    };
}
void compile_aline_impl() { ALINE_EXCEPTION(); }
std::function<void()> compile_aline(uint16_t op, int type, int reg, int dm) {
    return compile_aline_impl;
}

void compile_fline_impl() { FP_UNDEF(); }
std::function<void()> compile_fline(uint16_t op, int type, int reg, int dm) {
    return compile_fline_impl;
}

void build_functbl() {
    compile_map[00000] = compile_ori_b;
    compile_map[00001] = compile_ori_w;
    compile_map[00002] = compile_ori_l;
    compile_map[00003] = compile_cmp2_chk2_b;

    compile_map[00010] = compile_andi_b;
    compile_map[00011] = compile_andi_w;
    compile_map[00012] = compile_andi_l;
    compile_map[00013] = compile_cmp2_chk2_w;

    compile_map[00020] = compile_subi_b;
    compile_map[00021] = compile_subi_w;
    compile_map[00022] = compile_subi_l;
    compile_map[00023] = compile_cmp2_chk2_l;

    compile_map[00030] = compile_addi_b;
    compile_map[00031] = compile_addi_w;
    compile_map[00032] = compile_addi_l;

    compile_map[00040] = compile_btst_imm;
    compile_map[00041] = compile_bchg_imm;
    compile_map[00042] = compile_bclr_imm;
    compile_map[00043] = compile_bset_imm;

    compile_map[00050] = compile_eori_b;
    compile_map[00051] = compile_eori_w;
    compile_map[00052] = compile_eori_l;
    compile_map[00053] = compile_cas_b;

    compile_map[00060] = compile_cmpi_b;
    compile_map[00061] = compile_cmpi_w;
    compile_map[00062] = compile_cmpi_l;
    compile_map[00063] = compile_cas_w;

    compile_map[00070] = compile_moves_b;
    compile_map[00071] = compile_moves_w;
    compile_map[00072] = compile_moves_l;
    compile_map[00073] = compile_cas_l;

    compile_map[00400] = compile_negx_b;
    compile_map[00401] = compile_negx_w;
    compile_map[00402] = compile_negx_l;
    compile_map[00403] = compile_move_from_sr;

    compile_map[00410] = compile_clr_b;
    compile_map[00411] = compile_clr_w;
    compile_map[00412] = compile_clr_l;
    compile_map[00413] = compile_move_from_ccr;

    compile_map[00420] = compile_neg_b;
    compile_map[00421] = compile_neg_w;
    compile_map[00422] = compile_neg_l;
    compile_map[00423] = compile_move_to_ccr;

    compile_map[00430] = compile_not_b;
    compile_map[00431] = compile_not_w;
    compile_map[00432] = compile_not_l;
    compile_map[00433] = compile_move_to_sr;

    compile_map[00440] = compile_nbcd;
    compile_map[00441] = compile_0441;
    compile_map[00442] = compile_movem_to_w;
    compile_map[00443] = compile_movem_to_l;

    compile_map[00450] = compile_tst_b;
    compile_map[00451] = compile_tst_w;
    compile_map[00452] = compile_tst_l;
    compile_map[00453] = compile_tas;

    compile_map[00460] = compile_mul_l;
    compile_map[00461] = compile_div_l;
    compile_map[00462] = compile_movem_from_w;
    compile_map[00463] = compile_movem_from_l;

    compile_map[00471] = compile_0471;
    compile_map[00472] = compile_jsr;
    compile_map[00473] = compile_jmp;

    compile_map[01603] = compile_asr_ea;
    compile_map[01613] = compile_lsr_ea;
    compile_map[01623] = compile_roxr_ea;
    compile_map[01633] = compile_ror_ea;

    compile_map[01643] = compile_bftst;
    compile_map[01653] = compile_bfchg;
    compile_map[01663] = compile_bfclr;
    compile_map[01673] = compile_bfset;

    compile_map[01607] = compile_asl_ea;
    compile_map[01617] = compile_lsl_ea;
    compile_map[01627] = compile_roxl_ea;
    compile_map[01637] = compile_rol_ea;

    compile_map[01647] = compile_bfextu;
    compile_map[01657] = compile_bfexts;
    compile_map[01667] = compile_bfffo;
    compile_map[01677] = compile_bfins;

    for(int dn = 0; dn < 8; ++dn) {
        compile_map[00004 | dn << 3] = compile_btst_dm; // or movep
        compile_map[00005 | dn << 3] = compile_bchg_dm; // or movep
        compile_map[00006 | dn << 3] = compile_bclr_dm; // or movep
        compile_map[00007 | dn << 3] = compile_bset_dm; // or movep

        compile_map[00201 | dn << 3] = compile_movea_l;
        compile_map[00301 | dn << 3] = compile_movea_w;

        compile_map[00404 | dn << 3] = compile_chk_l;
        compile_map[00406 | dn << 3] = compile_chk_w;
        compile_map[00407 | dn << 3] = compile_lea; // or extb

        compile_map[00500 | dn << 3] = compile_addq_b;
        compile_map[00501 | dn << 3] = compile_addq_w;
        compile_map[00502 | dn << 3] = compile_addq_l;

        compile_map[00504 | dn << 3] = compile_subq_b;
        compile_map[00505 | dn << 3] = compile_subq_w;
        compile_map[00506 | dn << 3] = compile_subq_l;

        compile_map[01000 | dn << 3] = compile_or_b;
        compile_map[01001 | dn << 3] = compile_or_w;
        compile_map[01002 | dn << 3] = compile_or_l;
        compile_map[01003 | dn << 3] = compile_divu_w;

        compile_map[01004 | dn << 3] = compile_or_to_ea_b; // or sbcd
        compile_map[01005 | dn << 3] = compile_or_to_ea_w; // or pack
        compile_map[01006 | dn << 3] = compile_or_to_ea_l; // or unpk
        compile_map[01007 | dn << 3] = compile_divs_w;

        compile_map[01100 | dn << 3] = compile_sub_b;
        compile_map[01101 | dn << 3] = compile_sub_w;
        compile_map[01102 | dn << 3] = compile_sub_l;
        compile_map[01103 | dn << 3] = compile_suba_w;

        compile_map[01104 | dn << 3] = compile_sub_to_ea_b; // or subx
        compile_map[01105 | dn << 3] = compile_sub_to_ea_w; // or subx
        compile_map[01106 | dn << 3] = compile_sub_to_ea_l; // or subx
        compile_map[01107 | dn << 3] = compile_suba_l;

        compile_map[01300 | dn << 3] = compile_cmp_b;
        compile_map[01301 | dn << 3] = compile_cmp_w;
        compile_map[01302 | dn << 3] = compile_cmp_l;
        compile_map[01303 | dn << 3] = compile_cmpa_w;

        compile_map[01304 | dn << 3] = compile_eor_b; // cmpm.b
        compile_map[01305 | dn << 3] = compile_eor_w; // cmpm.w
        compile_map[01306 | dn << 3] = compile_eor_l; // cmpm.l
        compile_map[01307 | dn << 3] = compile_cmpa_l;

        compile_map[01400 | dn << 3] = compile_and_b;
        compile_map[01401 | dn << 3] = compile_and_w;
        compile_map[01402 | dn << 3] = compile_and_l;
        compile_map[01403 | dn << 3] = compile_mulu_w;

        compile_map[01404 | dn << 3] = compile_and_to_ea_b; // or abcd
        compile_map[01405 | dn << 3] = compile_and_to_ea_w; // or exg
        compile_map[01406 | dn << 3] = compile_and_to_ea_l; // or exg D<>A
        compile_map[01407 | dn << 3] = compile_muls_w;

        compile_map[01500 | dn << 3] = compile_add_b;
        compile_map[01501 | dn << 3] = compile_add_w;
        compile_map[01502 | dn << 3] = compile_add_l;
        compile_map[01503 | dn << 3] = compile_adda_w;

        compile_map[01504 | dn << 3] = compile_add_to_ea_b; // or addx.b
        compile_map[01505 | dn << 3] = compile_add_to_ea_w; // or addx.w
        compile_map[01506 | dn << 3] = compile_add_to_ea_l; // or addx.l
        compile_map[01507 | dn << 3] = compile_adda_l;

        compile_map[01600 | dn << 3] = compile_shr_b;
        compile_map[01601 | dn << 3] = compile_shr_w;
        compile_map[01602 | dn << 3] = compile_shr_l;

        compile_map[01604 | dn << 3] = compile_shl_b;
        compile_map[01605 | dn << 3] = compile_shl_w;
        compile_map[01606 | dn << 3] = compile_shl_l;

        for(int i = 0; i < 4; ++i) {
            compile_map[00700 | dn << 3 | i] = compile_moveq;
            compile_map[00704 | dn << 3 | i] = compile_emul_op;
        }

        for(int xg = 0; xg < 8; ++xg) {
            if(xg != 1) {
                compile_map[00100 | xg | dn << 3] = compile_move_b;
                compile_map[00200 | xg | dn << 3] = compile_move_l;
                compile_map[00300 | xg | dn << 3] = compile_move_w;
            }
        }
    }
    for(int cd = 0; cd < 16; ++cd) {
        compile_map[00503 | cd << 2] = compile_scc; // or dbcc or trapcc
        for(int i = 0; i < 4; ++i) {
            if(cd == 0) {
                compile_map[00600 | cd << 2 | i] = compile_bra;
            } else if(cd == 1) {
                compile_map[00600 | cd << 2 | i] = compile_bsr;
            } else {
                compile_map[00600 | cd << 2 | i] = compile_bcc;
            }
        }
    }
    for(int i = 0; i < 0100; ++i) {
        compile_map[01200 | i] = compile_aline;
    }

    for(int i = 0; i < 0100; ++i) {
        compile_map[01700 | i] = compile_fline;
    }

    compile_map[01730] = compile_move16;
}
