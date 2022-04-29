#include "SDL.h"
#include "sysdeps.h"
#include <vector>

#include "cpu_emulation.h"
#include "exception.h"
#include "main.h"
#include "memory.h"
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
struct ssw_t {
    bool read = true;
    bool cp = false, ct = false, cm = false, ma = false, atc = false;
    TT tt = TT::NORMAL;
    TM tm = TM::USER_DATA;
    SZ sz;
    uint16_t to_value() const {
        return cp << 15 | ct << 13 | cm << 12 | ma << 11 | atc << 10 |
               read << 8 | int(sz) << 5 | int(tt) << 3 | int(tm);
    }
};
void BUSERROR(const paddr &v, bool atc, bool ma) {
    uint32_t ea = regs.i_eav;
    ssw_t ssw{.read = !v.rw,
              .cm = (ea != 0xffffffffU),
              .ma = ma,
              .atc = atc,
              .tt = v.tt,
              .tm = v.tm,
              .sz = v.sz};
    uint32_t ea_v = (ea == 0xffffffffU ? 0 : ea);
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
              LOW(v.addr),
              HIGH(v.addr),
              0,
              0,
              0,
              // SSW
              ssw.to_value(),
              LOW(ea_v),
              HIGH(ea_v),
          },
          false);
    longjmp(ex_jmp, 1);
}
