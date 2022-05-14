// -*- c++ -*-
/*
 * UAE - The Un*x Amiga Emulator
 *
 * MC68000 emulation
 *
 * Copyright 1995 Bernd Schmidt
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

#ifndef NEWCPU_H
#define NEWCPU_H
#include "spcflags.h"
#include "fpu/fpu_mpfr.h"
#include <atomic>
#include <functional>
#include <memory>
#include <setjmp.h>
#include <stdint.h>
#include <future>
enum class TT { NORMAL, MOVE16, LFC, AA };
enum class TM {
    USER_DATA = 1,
    USER_CODE,
    SUPER_DATA = 5,
    SUPER_CODE,
};

enum class SZ { LONG, BYTE, WORD, LINE };

struct paddr {
    uint32_t addr;
    unsigned int upa : 2;
    SZ sz : 2;
    TT tt : 2;
    TM tm : 3;
    bool rw : 1;
};
struct ssw_t {
    bool read = true;
    bool cp = false, ct = false, cm = false, atc = false;
    TT tt = TT::NORMAL;
    TM tm = TM::USER_DATA;
    SZ sz;
    uint16_t to_value() const {
        return cp << 15 | ct << 13 | cm << 12 | atc << 10 |
               read << 8 | int(sz) << 5 | int(tt) << 3 | int(tm);
    }
};
struct m68k_reg {
    uint32_t r[16];
    uint32_t *d = r;
    uint32_t *a = &r[8];
    uint32_t opc;
    uint32_t pc;
    bool c, v, z, n, x;
    uint8_t IM;
    bool M, S;
    uint8_t T;
    bool exception;
    uint32_t isp;
    uint32_t usp;
    uint32_t msp;
    uint32_t vbr;
    uint8_t sfc, dfc;

    M68881 fpu;
   
    // MMU
    uint32_t urp;
    uint32_t srp;
    bool tcr_e, tcr_p;

    struct ttc_t {
        uint8_t address_base;
        uint8_t address_mask;
        bool E;
        uint8_t S;
        uint8_t U;
        uint8_t CM;
        bool W;
    };
    ttc_t ITTR[2], DTTR[2];
    struct mmusr_t {
        uint32_t PA;
        bool B, G, S, M, W, T, R;
        uint8_t U, CM;
    } MMUSR;

    bool cacr_de, cacr_ie;

    uint32_t i_ea;

    // BUS ERROR DATA
    uint32_t err_address;
    ssw_t err_ssw;

    // emulator flag

    std::unique_ptr<std::promise<void>> sleep;

    std::atomic<uint32_t> spcflags;
    std::atomic<uint8_t> irq;
    bool traced = false;

};
// no multi cpu
extern m68k_reg regs;
void init_m68k();
void exit_m68k();
void m68k_reset();
void m68k_execute();
void m68k_compile_execute();
inline void JUMP(uint32_t pc) {
    regs.pc = pc;
    regs.traced = true;
}

uint8_t GET_CCR();

uint16_t GET_SR();
void SET_CCR(uint8_t v);
void SET_SR(uint16_t sr);
#define OP(name)                                                               \
    void op_##name(                                                            \
        uint16_t __attribute__((unused)) xop, int __attribute__((unused)) dm,  \
        int __attribute__((unused)) type, int __attribute__((unused)) reg)

#endif /* NEWCPU_H */
