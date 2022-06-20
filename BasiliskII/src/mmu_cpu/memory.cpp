/*
 * UAE - The Un*x Amiga Emulator
 *
 * Memory management
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

#include "sysdeps.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu_emulation.h"
#include "main.h"
#include "video.h"

#include "exception.h"
#include "mbus.h"
#include "memory.h"
#include "newcpu.h"
#include <unordered_map>
#include <vector>

uint32_t do_mmu(uint32_t vaddr, bool code, bool rw, bool s);
uint8_t read8(uint32_t addr) {
    try {
        return b_read8(do_mmu(addr, false, false, regs.S));
    } catch(BUS_ERROR_EX &e) {
        TM tm = TM(int(regs.S ? TM::SUPER_DATA : TM::USER_DATA));
        paddr pe{addr, 0, SZ::BYTE, TT::NORMAL, tm, false};
        BUSERROR(pe, e.atc);
    }
}
uint16_t read16(uint32_t addr) {
    try {
        return b_read16(do_mmu(addr, false, false, regs.S));
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        paddr pe{addr, 0, SZ::WORD, TT::NORMAL, tm, false};
        BUSERROR(pe, e.atc);
    }
}

uint16_t FETCH() {
    uint32_t addr = std::exchange(regs.pc, regs.pc + 2);
    try {
        return b_read16(do_mmu(addr, true, false, regs.S));
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_CODE : TM::USER_CODE;
        paddr pe{addr, 0, SZ::WORD, TT::NORMAL, tm, false};
        BUSERROR(pe, e.atc);
    }
}
uint32_t read32(uint32_t addr) {
    try {
        uint32_t v = b_read32(do_mmu(addr, false, false, regs.S));
        return v;
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        paddr pe{addr, 0, SZ::LONG, TT::NORMAL, tm, false};
        BUSERROR(pe, e.atc);
    }
}
uint32_t FETCH32() {
    uint32_t addr = std::exchange(regs.pc, regs.pc + 4);
    try {
        uint32_t v = b_read32(do_mmu(addr, true, false, regs.S));
        return v;
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_CODE : TM::USER_CODE;
        paddr pe{addr, 0, SZ::LONG, TT::NORMAL, tm, false};
        BUSERROR(pe, e.atc);
    }
}
void write8(uint32_t addr, uint8_t v) {
    try {
        b_write8(do_mmu(addr, false, true, regs.S), v);
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        paddr pe{addr, 0, SZ::BYTE, TT::NORMAL, tm, true};
        BUSERROR(pe, e.atc);
    }
}
void write16(uint32_t addr, uint16_t v) {
    try {
        b_write16(do_mmu(addr, false, true, regs.S), v);
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        paddr pe{addr, 0, SZ::WORD, TT::NORMAL, tm, true};
        BUSERROR(pe, e.atc);
    }
}
void write32(uint32_t addr, uint32_t v) {
    try {
        b_write32(do_mmu(addr, false, true, regs.S), v);
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        paddr pe{addr, 0, SZ::LONG, TT::NORMAL, tm, true};
        BUSERROR(pe, e.atc);
    }
}
