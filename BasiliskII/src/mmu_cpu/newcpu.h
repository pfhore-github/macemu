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
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <setjmp.h>
#include <stdint.h>
#include <mutex>
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
    SZ sz : 2;
    TT tt : 2;
    TM tm : 3;
    bool lk : 1;
    bool rw : 1;
};
struct ssw_t {
    bool read = true, lk = false;
    bool cp = false, ct = false, cm = false, atc = false;
    bool ma = false;
    TT tt = TT::NORMAL;
    TM tm = TM::USER_DATA;
    SZ sz;
    uint16_t to_value() const {
        return cp << 15 | ct << 13 | cm << 12 | ma << 11 | atc << 10 | lk << 9 | read << 8 |
               int(sz) << 5 | int(tt) << 3 | int(tm);
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

    bool cacr_de, cacr_ie;

    uint32_t i_ea;
    bool ea_rw;

    // BUS ERROR DATA

    // emulator flag

    std::unique_ptr<std::promise<void>> sleep;

    std::atomic<uint32_t> spcflags;
    std::atomic<uint8_t> irq;
    std::atomic<bool> must_reset;
    bool traced = false;

    bool cdis = false;
};
// no multi cpu
extern m68k_reg regs;
void init_m68k();
void init_mmu();
void exit_m68k();
void m68k_reset();
void m68k_execute();
void m68k_compile_execute();
void JUMP(uint32_t pc);

uint8_t GET_CCR();

uint16_t GET_SR();
void SET_CCR(uint8_t v);
void SET_SR(uint16_t sr);

#define OP(name)                                                               \
    void op_##name(                                                            \
        uint16_t __attribute__((unused)) xop, int __attribute__((unused)) dm,  \
        int __attribute__((unused)) type, int __attribute__((unused)) reg)

#endif /* NEWCPU_H */
