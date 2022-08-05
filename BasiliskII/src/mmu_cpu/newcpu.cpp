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

#include "compiler/compiler.h"
#include "ea.h"
#include "ex_stack.h"
#include "exception.h"
#include "fpu/fpu.h"
#include "newcpu.h"
m68k_reg regs;
m680x0 cpu;
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


void EmulOp(uint16_t opcode, M68kRegisters *r);

using op_t = void (*)(uint16_t, int, int, int);
op_t opc_map[65536 >> 6];
void build_functbl();
void init_m68k() {
    init_fpu();
    init_mmu();

    build_functbl();
    init_fpu_opc();
    init_mmu_opc();
}

void exit_m68k(void) {}
extern std::unordered_map<uint32_t, void (*)()> rom_functions;
void reset_ex();
// run one opcode
void op_compile(uint32_t p);
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
    regs.trace_suspend = false;
    try {
        if(compiled.find(regs.opc) == compiled.end()) {
            op_compile(regs.opc);
        }
        auto [f, nextop] = compiled[regs.opc];
        f();
        if(!std::exchange(regs.jumped, false)) {
            regs.pc = nextop;
        }
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
            regs.trace_suspend = true;
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
    } catch(M68K_EXCEPTION &) {
    }
    // TRACE
    if(!regs.trace_suspend && (regs.T == 2 || (regs.T == 1 && regs.traced))) {
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

void JUMP(uint32_t pc) {
    if(pc & 1) {
        // ADDRESS ERROR
        RAISE2(3, pc & ~1, false);
        throw M68K_EXCEPTION{};
    }
    regs.pc = pc;
    regs.jumped = true;
}