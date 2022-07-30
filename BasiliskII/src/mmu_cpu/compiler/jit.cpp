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

#include "compiler/jit.h"
#include "ea.h"
#include "ex_stack.h"
#include "exception.h"
#include "fpu/fpu_mpfr.h"
#include "newcpu.h"
#include "op.h"

#include "jit_proto.h"
#include <asmjit/x86.h>
#include <setjmp.h>
using namespace asmjit;
jmp_buf jit_ex;
typedef void (*Func)();
void exit_jit() { longjmp(jit_ex, 1); }
void m68k_do_buserror(BUS_ERROR_EX &e);
uint32_t jit_read8(uint32_t addr) {
    try {
        return read8(addr);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        exit_jit();
    }
    return 0;
}

uint32_t jit_read16(uint32_t addr) {
    try {
        return read16(addr);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        exit_jit();
    }
    return 0;
}

uint32_t jit_read32(uint32_t addr) {
    try {
        return read32(addr);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        exit_jit();
    }
    return 0;
}

void jit_write8(uint32_t addr, uint32_t v) {
    try {
        write8(addr, v);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        exit_jit();
    }
}

void jit_write16(uint32_t addr, uint32_t v) {
    try {
        write16(addr, v);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        exit_jit();
    }
}

void jit_write32(uint32_t addr, uint32_t v) {
    try {
        write32(addr, v);
    } catch(BUS_ERROR_EX &e) {
        m68k_do_buserror(e);
        exit_jit();
    }
}

static asmjit::JitRuntime jit_rt;
std::unordered_map<uint32_t, std::shared_ptr<JIT_CODE>> jit_codes;
jit_t jit_map[65536 >> 6];
extern op_t opc_map[65536 >> 6];
uint64_t get_jit_address(const std::unordered_map<uint32_t, uint64_t> *p,
                         uint32_t pc) {
    return p->find(pc)->second;
}

bool jit_compile(uint32_t from, uint32_t to);
void m68k_postop();
bool jit_jump(uint32_t to) {
    regs.pc = to;
    auto it = jit_codes.find(to >> 12);
    if(it == jit_codes.end()) {
        if(!jit_compile(to & ~0xfff, (to & ~0xfff) + 0x1000)) {
            return false;
        }
    }
    if(setjmp(jit_ex) == 0) {
        auto f = it->second->func;
        (*f)();
    }
    return true;
}
void jit_illegal(uint32_t pc) {
    try {
        regs.opc = pc;
        PREFETCH();
        RAISE0(4, false);
    } catch(BUS_ERROR_EX &) {
    }
    exit_jit();
}
JIT_CODE *compiling = nullptr;
bool jit_compile(uint32_t from, uint32_t to) {
    std::shared_ptr<JIT_CODE> code(new JIT_CODE);
    compiling = code.get();
    code->holder.init(jit_rt.environment());
    x86::Assembler a(&code->holder);
    for(uint32_t u = from; u < to; u += 2) {
        code->jit_labels[u] = a.newLabel();
    }
    uint32_t pp = regs.pc;
    a.push(x86::rbp);
    a.mov(x86::rsp, x86::rbp);
    // alignment to 16 byte
    a.sub(x86::rsp, 8);
    a.push(x86::rbx);
#ifdef __WIN32
    a.push(x86::rdi);
    a.push(x86::rsi);
#endif
    a.push(x86::r12);
    a.push(x86::r13);
    a.push(x86::r14);
    a.push(x86::r15);
    a.mov(x86::r15, &regs);
    a.mov(ARG1, &code->init_entry);
    a.mov(eARG2, REG_LWORD(pc));
    a.call(get_jit_address);
    a.jmp(x86::rax);
    for(regs.pc = from; regs.pc < to;) {
        a.bind(code->jit_labels[regs.pc]);
        a.mov(REG_BYTE(trace_suspend), 0);
        regs.opc = regs.pc;
        uint16_t opc = FETCH();
        a.mov(REG_LWORD(pc), regs.pc);
        int type = opc >> 3 & 7;
        int reg = opc & 7;
        try {
            // do jit
            jit_t f = jit_map[opc >> 6];
            if(!f) {
                throw ILLEGAL_INST_EX{};
            }
            f(opc, type, reg, a);
            a.call(m68k_postop);
        } catch(ILLEGAL_INST_EX &) {
            a.mov(ARG1, regs.opc);
            a.call(jit_illegal);
            a.call(m68k_postop);
        } catch(BUS_ERROR_EX &e) {
            return false;
        }
    }
    a.pop(x86::r15);
    a.pop(x86::r14);
    a.pop(x86::r13);
    a.pop(x86::r12);
#ifdef __WIN32
    a.pop(x86::rsi);
    a.pop(x86::rdi);
#endif
    a.pop(x86::rbx);
    a.leave();
    a.ret();

    regs.pc = pp;
    Error err = jit_rt.add(
        &code->func, &code->holder); // Add the generated code to the runtime.
    if(err) {
        return false;
    }
    for(auto &[i, v] : code->jit_labels) {
        if(v.isValid()) {
            code->init_entry[i] = reinterpret_cast<ptrdiff_t>(code->func) +
                                  code->holder.labelOffset(v);
        }
    }
    jit_codes[from >> 12] = code;
    return true;
}
void jit_ea_addr(int type, int reg, int sz, bool w, x86::Assembler &a);
void jit_lea(uint16_t op, int type, int reg, x86::Assembler &a);
void jit_pea(int type, int reg, x86::Assembler &a);

void jit_0441(uint16_t op, int type, int reg, x86::Assembler &a) {
    if(type == 0) {
        // SWAP
        a.mov(x86::eax, JIT_REG_D_L(reg));
        a.ror(x86::eax, 16);
        a.mov(JIT_REG_D_L(reg), x86::eax);
        a.test(x86::eax, x86::eax);
        a.sete(REG_BYTE(z));
        a.sets(REG_BYTE(n));
        a.mov(REG_BYTE(v), 0);
        a.mov(REG_BYTE(c), 0);
    } else if(type == 1) {
        // BPKT
        a.mov(ARG1, regs.opc);
        a.call(jit_illegal);
    } else {
        jit_pea(type, reg, a);
    }
}
void TRAP(int v);

void jit_aline(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    a.call(ALINE_EXCEPTION);
    a.call(exit_jit);
}

void jit_fline(uint16_t op, int type, int reg, asmjit::x86::Assembler &a) {
    a.call(FP_UNDEF);
    a.call(exit_jit);
}

void build_jitfunctbl() {

    jit_map[00000] = jit_ori_b;
    jit_map[00001] = jit_ori_w;
    jit_map[00002] = jit_ori_l;
    jit_map[00003] = jit_cmp2_chk2_b;

    jit_map[00010] = jit_andi_b;
    jit_map[00011] = jit_andi_w;
    jit_map[00012] = jit_andi_l;
    jit_map[00013] = jit_cmp2_chk2_w;

    jit_map[00020] = jit_subi_b;
    jit_map[00021] = jit_subi_w;
    jit_map[00022] = jit_subi_l;
    jit_map[00023] = jit_cmp2_chk2_l;

    jit_map[00030] = jit_addi_b;
    jit_map[00031] = jit_addi_w;
    jit_map[00032] = jit_addi_l;

    jit_map[00040] = jit_btst_imm;
    jit_map[00041] = jit_bchg_imm;
    jit_map[00042] = jit_bclr_imm;
    jit_map[00043] = jit_bset_imm;

    jit_map[00050] = jit_eori_b;
    jit_map[00051] = jit_eori_w;
    jit_map[00052] = jit_eori_l;
    jit_map[00053] = jit_cas_b;

    jit_map[00060] = jit_cmpi_b;
    jit_map[00061] = jit_cmpi_w;
    jit_map[00062] = jit_cmpi_l;
    jit_map[00063] = jit_cas_w;

    jit_map[00070] = jit_moves_b;
    jit_map[00071] = jit_moves_w;
    jit_map[00072] = jit_moves_l;
    jit_map[00073] = jit_cas_l;

    jit_map[00400] = jit_negx_b;
    jit_map[00401] = jit_negx_w;
    jit_map[00402] = jit_negx_l;
    jit_map[00403] = jit_move_from_sr;

    jit_map[00410] = jit_clr_b;
    jit_map[00411] = jit_clr_w;
    jit_map[00412] = jit_clr_l;
    jit_map[00413] = jit_move_from_ccr;

    jit_map[00420] = jit_neg_b;
    jit_map[00421] = jit_neg_w;
    jit_map[00422] = jit_neg_l;
    jit_map[00423] = jit_move_to_ccr;

    jit_map[00430] = jit_not_b;
    jit_map[00431] = jit_not_w;
    jit_map[00432] = jit_not_l;
    jit_map[00433] = jit_move_to_sr;

    jit_map[00440] = jit_nbcd;
    jit_map[00441] = jit_0441;
    jit_map[00442] = jit_movem_to_w;
    jit_map[00443] = jit_movem_to_l;

    jit_map[00450] = jit_tst_b;
    jit_map[00451] = jit_tst_w;
    jit_map[00452] = jit_tst_l;
    jit_map[00453] = jit_tas;

    jit_map[00460] = jit_mul_l;
    jit_map[00461] = jit_div_l;
    jit_map[00462] = jit_movem_from_w;
    jit_map[00463] = jit_movem_from_l;

    jit_map[00471] = jit_0471;
    jit_map[00472] = jit_jsr;
    jit_map[00473] = jit_jmp;

    jit_map[01603] = jit_asr_ea;
    jit_map[01613] = jit_lsr_ea;
    jit_map[01623] = jit_roxr_ea;
    jit_map[01633] = jit_ror_ea;

    jit_map[01643] = jit_bftst;
    jit_map[01653] = jit_bfchg;
    jit_map[01663] = jit_bfclr;
    jit_map[01673] = jit_bfset;

    jit_map[01607] = jit_asl_ea;
    jit_map[01617] = jit_lsl_ea;
    jit_map[01627] = jit_roxl_ea;
    jit_map[01637] = jit_rol_ea;

    jit_map[01647] = jit_bfextu;
    jit_map[01657] = jit_bfexts;
    jit_map[01667] = jit_bfffo;
    jit_map[01677] = jit_bfins;

    for(int dn = 0; dn < 8; ++dn) {
        jit_map[00004 | dn << 3] = jit_btst_dm; // or movep
        jit_map[00005 | dn << 3] = jit_bchg_dm; // or movep
        jit_map[00006 | dn << 3] = jit_bclr_dm; // or movep
        jit_map[00007 | dn << 3] = jit_bset_dm; // or movep

        jit_map[00201 | dn << 3] = jit_movea_l;
        jit_map[00301 | dn << 3] = jit_movea_w;

        jit_map[00404 | dn << 3] = jit_chk_l;
        jit_map[00406 | dn << 3] = jit_chk_w;
        jit_map[00407 | dn << 3] = jit_lea; // or extb

        jit_map[00500 | dn << 3] = jit_addq_b;
        jit_map[00501 | dn << 3] = jit_addq_w;
        jit_map[00502 | dn << 3] = jit_addq_l;

        jit_map[00504 | dn << 3] = jit_subq_b;
        jit_map[00505 | dn << 3] = jit_subq_w;
        jit_map[00506 | dn << 3] = jit_subq_l;

        jit_map[01000 | dn << 3] = jit_or_b;
        jit_map[01001 | dn << 3] = jit_or_w;
        jit_map[01002 | dn << 3] = jit_or_l;
        jit_map[01003 | dn << 3] = jit_divu_w;

        jit_map[01004 | dn << 3] = jit_or_to_ea_b; // or sbcd
        jit_map[01005 | dn << 3] = jit_or_to_ea_w; // or pack
        jit_map[01006 | dn << 3] = jit_or_to_ea_l; // or unpk
        jit_map[01007 | dn << 3] = jit_divs_w;

        jit_map[01100 | dn << 3] = jit_sub_b;
        jit_map[01101 | dn << 3] = jit_sub_w;
        jit_map[01102 | dn << 3] = jit_sub_l;
        jit_map[01103 | dn << 3] = jit_suba_w;

        jit_map[01104 | dn << 3] = jit_sub_to_ea_b; // or subx
        jit_map[01105 | dn << 3] = jit_sub_to_ea_w; // or subx
        jit_map[01106 | dn << 3] = jit_sub_to_ea_l; // or subx
        jit_map[01107 | dn << 3] = jit_suba_l;

        jit_map[01300 | dn << 3] = jit_cmp_b;
        jit_map[01301 | dn << 3] = jit_cmp_w;
        jit_map[01302 | dn << 3] = jit_cmp_l;
        jit_map[01303 | dn << 3] = jit_cmpa_w;

        jit_map[01304 | dn << 3] = jit_eor_b; // cmpm.b
        jit_map[01305 | dn << 3] = jit_eor_w; // cmpm.w
        jit_map[01306 | dn << 3] = jit_eor_l; // cmpm.l
        jit_map[01307 | dn << 3] = jit_cmpa_l;

        jit_map[01400 | dn << 3] = jit_and_b;
        jit_map[01401 | dn << 3] = jit_and_w;
        jit_map[01402 | dn << 3] = jit_and_l;
        jit_map[01403 | dn << 3] = jit_mulu_w;

        jit_map[01404 | dn << 3] = jit_and_to_ea_b; // or abcd
        jit_map[01405 | dn << 3] = jit_and_to_ea_w; // or exg
        jit_map[01406 | dn << 3] = jit_and_to_ea_l; // or exg D<>A
        jit_map[01407 | dn << 3] = jit_muls_w;

        jit_map[01500 | dn << 3] = jit_add_b;
        jit_map[01501 | dn << 3] = jit_add_w;
        jit_map[01502 | dn << 3] = jit_add_l;
        jit_map[01503 | dn << 3] = jit_adda_w;

        jit_map[01504 | dn << 3] = jit_add_to_ea_b; // or addx.b
        jit_map[01505 | dn << 3] = jit_add_to_ea_w; // or addx.w
        jit_map[01506 | dn << 3] = jit_add_to_ea_l; // or addx.l
        jit_map[01507 | dn << 3] = jit_adda_l;

        jit_map[01600 | dn << 3] = jit_shr_b;
        jit_map[01601 | dn << 3] = jit_shr_w;
        jit_map[01602 | dn << 3] = jit_shr_l;

        jit_map[01604 | dn << 3] = jit_shl_b;
        jit_map[01605 | dn << 3] = jit_shl_w;
        jit_map[01606 | dn << 3] = jit_shl_l;

        for(int i = 0; i < 4; ++i) {
            jit_map[00700 | dn << 3 | i] = jit_moveq;
            //            opc_map[00704 | dn << 3 | i] = op_emul_op;
        }

        for(int xg = 0; xg < 8; ++xg) {
            if(xg != 1) {
                jit_map[00100 | xg | dn << 3] = jit_move_b;
                jit_map[00200 | xg | dn << 3] = jit_move_l;
                jit_map[00300 | xg | dn << 3] = jit_move_w;
            }
        }
    }

    for(int cd = 0; cd < 16; ++cd) {
        jit_map[00503 | cd << 2] = jit_scc; // or dbcc or trapcc
        for(int i = 0; i < 4; ++i) {
            if(cd == 0) {
                jit_map[00600 | cd << 2 | i] = jit_bra;
            } else if(cd == 1) {
                jit_map[00600 | cd << 2 | i] = jit_bsr;
            } else {
                jit_map[00600 | cd << 2 | i] = jit_bcc;
            }
        }
    }

    for(int i = 0; i < 0100; ++i) {
        jit_map[01200 | i] = jit_aline;
    }

    for(int i = 0; i < 0100; ++i) {
        jit_map[01700 | i] = jit_fline;
    }

    jit_map[01730] = jit_move16;
}