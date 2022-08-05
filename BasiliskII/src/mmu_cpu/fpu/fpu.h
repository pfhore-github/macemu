#ifndef MMU_CPU_FPU_H
#define MMU_CPU_FPU_H 1
#include "fpu/fpu_mpfr.h"
#include <array>
#define _USE_MATH_DEFINES
void init_fpu();
void exit_fpu();

void init_fpu_opc();

enum class RND_MODE { ROUND_RN, ROUND_RZ, ROUND_RM, ROUND_RP };
enum class RND_PREC { ROUND_EXT, ROUND_SGL, ROUND_DBL, ROUND_DEF };
struct M68881 {
    // FPU
    std::array<fpvalue, 8> fp;
    struct fpcr_t {
        RND_PREC prec;
        bool inex1;
        bool inex2;
        bool dz;
        bool unfl;
        bool ovfl;
        bool operr;
        bool snan;
        bool bsun;
    } FPCR;
    struct fpsr_t {
        bool ac_inex;
        bool ac_dz;
        bool ac_unfl;
        bool ac_ovfl;
        bool ac_iop;
        bool inex1;
        bool inex2;
        bool dz;
        bool unfl;
        bool ovfl;
        bool operr;
        bool snan;
        bool bsun;
        int8_t quotient;
        bool nan;
        bool i;
        bool z;
        bool n;
    } FPSR;
    uint32_t fpiar;

    // internal
    fpvalue src_v, dst_v;
};

void load_single(uint32_t addr, fpvalue *dst);
void load_ext(uint32_t addr, fpvalue *dst);
void load_packed(uint32_t addr, fpvalue *dst);
void load_double(uint32_t addr, fpvalue *dst);
void fmove_cr(uint8_t op, fpvalue *v);
void fmovem_from_mem(uint32_t *addr, int mask);
void fmovem_to_mem(uint32_t addr, int mask);
void fmovem_to_mem_decr(uint32_t *addr, int mask);
void fmove_cr(uint8_t op, fpvalue *v) ;
void fpu_postprocess(fpvalue &d, RND_PREC round = RND_PREC::ROUND_DEF);
#endif