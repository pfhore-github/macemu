#include "exception.h"
#include "SDL.h"
#include "cpu_emulation.h"
#include "ex_stack.h"
#include "main.h"
#include "memory.h"
#include "newcpu.h"
#include "sysdeps.h"
void SET_SR(uint16_t v);
void SAVE_SP();
void LOAD_SP();
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
    regs.exception = false;
}

void BUSERROR(const paddr &v, bool atc, bool ma) {
    regs.err_address = v.addr;
    regs.err_ssw.read = !v.rw;
    regs.err_ssw.atc = atc;
    regs.err_ssw.lk = v.lk;
    regs.err_ssw.tt = TT(int(v.tt) & 3);
    regs.err_ssw.tm = TM(int(v.tm) & 7);
    regs.err_ssw.sz = SZ(int(v.sz) & 3);
    throw BUS_ERROR_EX{};
}
