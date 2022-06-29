#include "exception.h"
#include "SDL.h"
#include "cpu_emulation.h"
#include "ex_stack.h"
#include "main.h"
#include "memory.h"
#include "newcpu.h"
#include "sysdeps.h"
#include "mmu/mmu_68040.h"
void SET_SR(uint16_t v);
void SAVE_SP();
void LOAD_SP();
void reset_ex() {
    regs.S = true;
    regs.M = false;
    regs.T = 0;
    regs.IM = 7;
    regs.vbr = 0;
    regs.cacr_de = regs.cacr_ie = false;
    mmu_d.TTR[0].E = false;
    mmu_d.TTR[1].E = false;
    mmu_i.TTR[0].E = false;
    mmu_i.TTR[1].E = false;
    try {
        regs.a[7] = read32(0);
        regs.pc = read32(4);
        PREFETCH();
    } catch(BUS_ERROR_EX &) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "double fault occurerd at %08x\n",
                     regs.pc);
        abort();
    }
}
void RAISE(int e, int f, const std::vector<uint16_t> &data, bool next,
           bool irq) {
    if(regs.exception) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "double fault occurerd at %08x\n",
                     regs.pc);
        abort();
    }
    regs.exception = true;
    uint16_t sr = GET_SR();
    SAVE_SP();
    regs.T = 0;
    regs.S = true;
    // IRQ
    if(irq) {
        if( std::exchange( regs.M, false) ) {
            f = 1;
        }
    }
    LOAD_SP();
    for(auto v : data) {
        PUSH16(v);
    }
    PUSH16(f << 12 | e << 2);
    if(next) {
        PUSH32(regs.pc);
    } else {
        PUSH32(regs.opc);
    }
    PUSH16(sr);
    regs.pc = read32(regs.vbr + (e << 2));
}


