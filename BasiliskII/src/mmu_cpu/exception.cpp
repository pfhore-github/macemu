#include "SDL.h"
#include "sysdeps.h"
#include <vector>

#include "cpu_emulation.h"
#include "exception.h"
#include "main.h"
#include "newcpu.h"
void SET_SR(uint16_t v);
jmp_buf ex_jmp;
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
        regs.M = false;
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
void BUSERROR(uint32_t addr, bool code, uint16_t ssw, uint32_t ea) {
    RAISE(2, 7,
          {
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              0,
              LOW(addr),
              HIGH(addr),
              0,
              0,
              0,
              // SSW
              ssw,
              LOW(ea),
              HIGH(ea),
          },
          false);
    longjmp(ex_jmp, 1);
}
