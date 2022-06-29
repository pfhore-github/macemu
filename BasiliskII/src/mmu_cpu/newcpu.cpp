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
#include "ex_stack.h"
#include "exception.h"
#include "fpu/fpu.h"

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
    return GET_CCR() | regs.IM << 8 | regs.M << 12 | regs.S << 13 |
           regs.T << 14;
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

OP(aline) {
    PREFETCH();
    ALINE_EXCEPTION();
}
void EmulOp(uint16_t opcode, M68kRegisters *r);
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
op_t opc_map[65536 >> 6];
static void build_cpufunctbl() {

    opc_map[00000] = op_ori_b;
    opc_map[00001] = op_ori_w;
    opc_map[00002] = op_ori_l;
    opc_map[00003] = op_cmp2_chk2_b;

    opc_map[00010] = op_andi_b;
    opc_map[00011] = op_andi_w;
    opc_map[00012] = op_andi_l;
    opc_map[00013] = op_cmp2_chk2_w;

    opc_map[00020] = op_subi_b;
    opc_map[00021] = op_subi_w;
    opc_map[00022] = op_subi_l;
    opc_map[00023] = op_cmp2_chk2_l;

    opc_map[00030] = op_addi_b;
    opc_map[00031] = op_addi_w;
    opc_map[00032] = op_addi_l;

    opc_map[00040] = op_btst_imm;
    opc_map[00041] = op_bchg_imm;
    opc_map[00042] = op_bclr_imm;
    opc_map[00043] = op_bset_imm;

    opc_map[00050] = op_eori_b;
    opc_map[00051] = op_eori_w;
    opc_map[00052] = op_eori_l;
    opc_map[00053] = op_cas_b;

    opc_map[00060] = op_cmpi_b;
    opc_map[00061] = op_cmpi_w;
    opc_map[00062] = op_cmpi_l;
    opc_map[00063] = op_cas_w;

    opc_map[00070] = op_moves_b;
    opc_map[00071] = op_moves_w;
    opc_map[00072] = op_moves_l;
    opc_map[00073] = op_cas_l;

    opc_map[00400] = op_negx_b;
    opc_map[00401] = op_negx_w;
    opc_map[00402] = op_negx_l;
    opc_map[00403] = op_move_from_sr;

    opc_map[00410] = op_clr_b;
    opc_map[00411] = op_clr_w;
    opc_map[00412] = op_clr_l;
    opc_map[00413] = op_move_from_ccr;

    opc_map[00420] = op_neg_b;
    opc_map[00421] = op_neg_w;
    opc_map[00422] = op_neg_l;
    opc_map[00423] = op_move_to_ccr;

    opc_map[00430] = op_not_b;
    opc_map[00431] = op_not_w;
    opc_map[00432] = op_not_l;
    opc_map[00433] = op_move_to_sr;

    opc_map[00440] = op_nbcd;       // or link
    opc_map[00441] = op_0441;       // swap or bpkt or pea
    opc_map[00442] = op_movem_to_w; // or ext.w
    opc_map[00443] = op_movem_to_l; // or ext.l

    opc_map[00450] = op_tst_b;
    opc_map[00451] = op_tst_w;
    opc_map[00452] = op_tst_l;
    opc_map[00453] = op_tas; // or illegal

    opc_map[00460] = op_mul_l;
    opc_map[00461] = op_div_l;
    opc_map[00462] = op_movem_from_w;
    opc_map[00463] = op_movem_from_l;

    opc_map[00471] = op_0471;
    opc_map[00472] = op_jsr;
    opc_map[00473] = op_jmp;

    opc_map[01603] = op_asr_ea;
    opc_map[01613] = op_lsr_ea;
    opc_map[01623] = op_roxr_ea;
    opc_map[01633] = op_ror_ea;

    opc_map[01643] = op_bftst;
    opc_map[01653] = op_bfchg;
    opc_map[01663] = op_bfclr;
    opc_map[01673] = op_bfset;

    opc_map[01607] = op_asl_ea;
    opc_map[01617] = op_lsl_ea;
    opc_map[01627] = op_roxl_ea;
    opc_map[01637] = op_rol_ea;

    opc_map[01647] = op_bfextu;
    opc_map[01657] = op_bfexts;
    opc_map[01667] = op_bfffo;
    opc_map[01677] = op_bfins;

    for(int dn = 0; dn < 8; ++dn) {
        opc_map[00004 | dn << 3] = op_btst_dm; // or movep
        opc_map[00005 | dn << 3] = op_bchg_dm; // or movep
        opc_map[00006 | dn << 3] = op_bclr_dm; // or movep
        opc_map[00007 | dn << 3] = op_bset_dm; // or movep

        opc_map[00201 | dn << 3] = op_movea_l;
        opc_map[00301 | dn << 3] = op_movea_w;

        opc_map[00404 | dn << 3] = op_chk_l;
        opc_map[00406 | dn << 3] = op_chk_w;
        opc_map[00407 | dn << 3] = op_lea; // or extb

        opc_map[00500 | dn << 3] = op_addq_b;
        opc_map[00501 | dn << 3] = op_addq_w;
        opc_map[00502 | dn << 3] = op_addq_l;

        opc_map[00504 | dn << 3] = op_subq_b;
        opc_map[00505 | dn << 3] = op_subq_w;
        opc_map[00506 | dn << 3] = op_subq_l;

        opc_map[01000 | dn << 3] = op_or_b;
        opc_map[01001 | dn << 3] = op_or_w;
        opc_map[01002 | dn << 3] = op_or_l;
        opc_map[01003 | dn << 3] = op_divu_w;

        opc_map[01004 | dn << 3] = op_or_to_ea_b; // or sbcd
        opc_map[01005 | dn << 3] = op_or_to_ea_w; // or pack
        opc_map[01006 | dn << 3] = op_or_to_ea_l; // or unpk
        opc_map[01007 | dn << 3] = op_divs_w;

        opc_map[01100 | dn << 3] = op_sub_b;
        opc_map[01101 | dn << 3] = op_sub_w;
        opc_map[01102 | dn << 3] = op_sub_l;
        opc_map[01103 | dn << 3] = op_suba_w;

        opc_map[01104 | dn << 3] = op_sub_to_ea_b; // or subx
        opc_map[01105 | dn << 3] = op_sub_to_ea_w; // or subx
        opc_map[01106 | dn << 3] = op_sub_to_ea_l; // or subx
        opc_map[01107 | dn << 3] = op_suba_l;

        opc_map[01300 | dn << 3] = op_cmp_b;
        opc_map[01301 | dn << 3] = op_cmp_w;
        opc_map[01302 | dn << 3] = op_cmp_l;
        opc_map[01303 | dn << 3] = op_cmpa_w;

        opc_map[01304 | dn << 3] = op_eor_b; // cmpm.b
        opc_map[01305 | dn << 3] = op_eor_w; // cmpm.w
        opc_map[01306 | dn << 3] = op_eor_l; // cmpm.l
        opc_map[01307 | dn << 3] = op_cmpa_l;

        opc_map[01400 | dn << 3] = op_and_b;
        opc_map[01401 | dn << 3] = op_and_w;
        opc_map[01402 | dn << 3] = op_and_l;
        opc_map[01403 | dn << 3] = op_mulu_w;

        opc_map[01404 | dn << 3] = op_and_to_ea_b; // or abcd
        opc_map[01405 | dn << 3] = op_and_to_ea_w; // or exg
        opc_map[01406 | dn << 3] = op_and_to_ea_l; // or exg D<>A
        opc_map[01407 | dn << 3] = op_muls_w;

        opc_map[01500 | dn << 3] = op_add_b;
        opc_map[01501 | dn << 3] = op_add_w;
        opc_map[01502 | dn << 3] = op_add_l;
        opc_map[01503 | dn << 3] = op_adda_w;

        opc_map[01504 | dn << 3] = op_add_to_ea_b; // or addx.b
        opc_map[01505 | dn << 3] = op_add_to_ea_w; // or addx.w
        opc_map[01506 | dn << 3] = op_add_to_ea_l; // or addx.l
        opc_map[01507 | dn << 3] = op_adda_l;

        opc_map[01600 | dn << 3] = op_shr_b;
        opc_map[01601 | dn << 3] = op_shr_w;
        opc_map[01602 | dn << 3] = op_shr_l;

        opc_map[01604 | dn << 3] = op_shl_b;
        opc_map[01605 | dn << 3] = op_shl_w;
        opc_map[01606 | dn << 3] = op_shl_l;

        for(int i = 0; i < 4; ++i) {
            opc_map[00700 | dn << 3 | i] = op_moveq;
            opc_map[00704 | dn << 3 | i] = op_emul_op;
        }
        for(int xg = 0; xg < 8; ++xg) {
            if(xg != 1) {
                opc_map[00100 | xg | dn << 3] = op_move_b;
                opc_map[00200 | xg | dn << 3] = op_move_l;
                opc_map[00300 | xg | dn << 3] = op_move_w;
            }
        }
    }

    for(int cd = 0; cd < 16; ++cd) {
        opc_map[00503 | cd << 2] = op_scc; // or dbcc or trapcc
        for(int i = 0; i < 4; ++i) {
            if(cd == 0) {
                opc_map[00600 | cd << 2 | i] = op_bra;
            } else if(cd == 1) {
                opc_map[00600 | cd << 2 | i] = op_bsr;
            } else {
                opc_map[00600 | cd << 2 | i] = op_bcc;
            }
        }
    }

    for(int i = 0; i < 0100; ++i) {
        opc_map[01200 | i] = op_aline;
    }

    opc_map[01730] = op_move16;

    init_fpu_opc();
    init_mmu_opc();
}

void init_m68k() {
    build_cpufunctbl();
    init_fpu();
    init_mmu();
}

void exit_m68k(void) {}
extern std::unordered_map<uint32_t, void (*)()> rom_functions;
void reset_ex();
// run one opcode
void m68k_do_execute() {
    if(regs.must_reset.load()) {
        reset_ex();
        regs.must_reset.store(false);
        return;
    }
    if(auto it = rom_functions.find(regs.pc & 0xfffff);
       it != rom_functions.end()) {
        it->second();
        return;
    }
    regs.opc = regs.pc;
    uint16_t opc = FETCH();
    bool trace_suspend = false;
    try {
        op_t f = opc_map[opc >> 6];
        if(!f) {
            throw ILLEGAL_INST_EX{};
        }
        f(opc, opc >> 9 & 7, opc >> 3 & 7, opc & 7);
    } catch(ILLEGAL_INST_EX &) {
        PREFETCH();
        RAISE0(4, false);
    } catch(BUS_ERROR_EX &e) {

        uint16_t ssw = int(e.tm) | int(e.tt) << 3 | int(e.size) << 5 |
                       e.rw << 8 | e.lk << 9 | e.atc << 10 | e.ma << 11;
        // check CP
        // check CU
        uint32_t ea_v = 0;

        if(regs.T == 2 || (regs.T == 1 && regs.traced)) {
            trace_suspend = true;
            ssw |= 1 << 13;
            ea_v = regs.opc;
        } else if(e.cm) {
            ssw |= 1 << 12;
            ea_v = regs.i_ea;
        }

        std::vector<uint16_t> data(26u, 0);
        data[18] = LOW(e.addr);
        data[19] = HIGH(e.addr);
        data[23] = ssw;
        data[24] = LOW(ea_v);
        data[25] = HIGH(ea_v);
        RAISE(2, 7, data, false);
        return;
    }
    // TRACE
    if(!trace_suspend && (regs.T == 2 || (regs.T == 1 && regs.traced))) {
        TRACE();
        return;
    }

    // IRQ
    for(int i = 7; i > 0; --i) {
        if(regs.IM <= i && regs.irq & (1 << i)) {
            regs.IM = i;
            regs.irq &= ~(1 << i);
            RAISE0(25 + i, true, true);
            break;
        }
    }
    regs.i_ea = 0;
    regs.traced = false;
}
bool debug = false;

void dump_regs();
void run_m68k(const std::vector<uint32_t> &until) {
    for(;;) {
        for(auto u : until) {
            if((regs.pc & 0xfffff) == u) {
                return;
            }
        }
        if(debug) {
            dump_regs();
        }
        m68k_do_execute();
    };
}
void run_m68k(uint32_t until) {
    while((regs.pc & 0xfffff) != until) {
        if(debug) {
            dump_regs();
        }
        m68k_do_execute();
    };
}
int m68k_thread(void *) {
    for(;;) {
        run_m68k(0);
    }
}
void m68k_execute() {
    regs.i_ea = 0;
    SDL_Thread *t = SDL_CreateThread(m68k_thread, "cpu", nullptr);
    SDL_DetachThread(t);
}

OP(0441) {
    if(type == 0) {
        op_swap(reg);
    } else if(type == 1) {
        op_bkpt();
    } else {
        op_pea(type, reg);
    }
}

void op_pea(int type, int reg) {
    regs.i_ea = EA_Addr(type, reg, 0, false);
    PUSH32(regs.i_ea);
}

void op_nop() { regs.traced = true; }

OP(0471) {
    switch(type) {
    case 0:
    case 1:
        op_trap(type << 3 | reg);
        break;
    case 2:
        op_link_w(reg);
        break;
    case 3:
        op_unlk(reg);
        break;
    case 4:
        op_move_to_usp(reg);
        break;
    case 5:
        op_move_from_usp(reg);
        break;
    case 6:
        switch(reg) {
        case 0:
            op_reset();
            break;
        case 1:
            op_nop();
            break;
        case 2:
            op_stop();
            break;
        case 3:
            op_rte();
            break;
        case 4:
            op_rtd();
            break;
        case 5:
            op_rts();
            break;
        case 6:
            op_trapv();
            break;
        case 7:
            op_rtr();
            break;
        }
        break;
    case 7:
        if(reg == 2) {
            op_movec_from();
        } else if(reg == 3) {
            op_movec_to();
        } else {
            ILLEGAL_INST();
        }
    }
}

OP(lea) {
    if(type == 0) {
        op_extb(reg);
    } else {
        regs.i_ea = EA_Addr(type, reg, 0, false);
        regs.a[dm] = regs.i_ea;
    }
}

void JUMP(uint32_t pc) {
    if(pc & 1) {
        // ADDRESS ERROR
        RAISE2(3, pc & ~1, false);
        return;
    }
    regs.pc = pc;
}