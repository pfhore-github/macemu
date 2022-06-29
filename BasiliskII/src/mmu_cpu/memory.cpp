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
#include "mmu/mmu_68040.h"
#include "newcpu.h"
#include <unordered_map>
#include <vector>
template <class F> class MyFinally {
    F f;

  public:
    MyFinally(F v) : f(v) {}
    ~MyFinally() { f(); }
    MyFinally(const MyFinally<F> &) = delete;
    MyFinally<F> &operator=(const MyFinally<F> &) = delete;
};
uint8_t read8(uint32_t addr) {
    try {
        return b_read8(mmu_d.do_mmu(addr, false, regs.S));
    } catch(BUS_ERROR_EX &e) {
        e.addr = addr;
        e.tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        e.size = SZ::BYTE;
        e.lk = regs.ea_rw;
        e.rw = true;
        throw e;
    }
}
uint16_t read16(uint32_t addr) {
    try {
        if(addr & 1) {
            uint16_t v = read8(addr) << 8;
            try {
                v |= read8(addr + 1);
            } catch(BUS_ERROR_EX &e) {
                e.ma = true;
                throw e;
            }
            return v;
        } else {
            return b_read16(mmu_d.do_mmu(addr, false, regs.S));
        }
    } catch(BUS_ERROR_EX &e) {
        e.addr = addr;
        e.tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        e.size = SZ::WORD;
        e.lk = regs.ea_rw;
        e.rw = true;
        throw e;
    }
}

uint16_t FETCH() {
    uint32_t addr = std::exchange(regs.pc, regs.pc + 2);
    try {
        return b_read16(mmu_i.do_mmu(addr, false, regs.S));
    } catch(BUS_ERROR_EX &e) {
        e.addr = addr;
        e.tm = regs.S ? TM::SUPER_CODE : TM::USER_CODE;
        e.size = SZ::WORD;
        e.rw = true;
        throw e;
    }
}

void PREFETCH() {
    uint32_t addr = regs.pc + 2;
    try {
        for(int i = 0; i < 16; addr += 2, i += 2) {
            b_read16(mmu_i.do_mmu(addr, false, regs.S));
        }
    } catch(BUS_ERROR_EX &e) {
        e.addr = addr;
        e.tm = regs.S ? TM::SUPER_CODE : TM::USER_CODE;
        e.size = SZ::LINE;
        e.rw = true;
        throw e;
    }
}
uint32_t read32(uint32_t addr) {
    uint32_t v;
    try {
        switch(addr & 3) {
        case 0: // aligned
            return b_read32(mmu_d.do_mmu(addr, false, regs.S));
        case 1:
        case 3: // mis-aligned
            v = b_read8(mmu_d.do_mmu(addr, false, regs.S)) << 24;
            try {
                v |= b_read16(mmu_d.do_mmu((addr + 2), false, regs.S)) << 8;
                v |= b_read8(mmu_d.do_mmu(addr + 3, false, regs.S));
                return v;
            } catch(BUS_ERROR_EX &e) {
                e.ma = true;
                throw e;
            }
        case 2: // half-aligned
            v = b_read16(mmu_d.do_mmu(addr, false, regs.S)) << 16;
            try {
                v |= b_read16(mmu_d.do_mmu(addr + 2, false, regs.S));
                return v;
            } catch(BUS_ERROR_EX &e) {
                e.ma = true;
                throw e;
            }
        }
        return 0;
    } catch(BUS_ERROR_EX &e) {
        e.addr = addr;
        e.tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        e.size = SZ::LONG;
        e.lk = regs.ea_rw;
        e.rw = true;
        throw e;
    }
}
uint32_t FETCH32() {
    uint32_t addr = std::exchange(regs.pc, regs.pc + 4);
    try {
        return b_read32(mmu_i.do_mmu(addr, false, regs.S));
    } catch(BUS_ERROR_EX &e) {
        e.addr = addr;
         e.tm = regs.S ? TM::SUPER_CODE : TM::USER_CODE;
        e.size = SZ::LONG;
        e.rw = true;
        throw e;
    }
}
void write8(uint32_t addr, uint8_t v) {
    MyFinally e = []() { regs.ea_rw = false; };
    try {
        b_write8(mmu_d.do_mmu(addr, true, regs.S), v);
    } catch(BUS_ERROR_EX &e) {
        e.addr = addr;
        e.tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        e.size = SZ::BYTE;
        e.lk = regs.ea_rw;
        e.rw = false;
        throw e;
    }
}
void write16(uint32_t addr, uint16_t v) {
    MyFinally e = []() { regs.ea_rw = false; };
    try {
        if(addr & 1) {
            b_write8(mmu_d.do_mmu(addr, true, regs.S), v >> 8);
            try {
                b_write8(mmu_d.do_mmu(addr + 1, true, regs.S), v & 0xff);
            } catch(BUS_ERROR_EX &e) {
                e.ma = true;
                throw e;
            }
        } else {
            b_write16(mmu_d.do_mmu(addr, true, regs.S), v);
        }
    } catch(BUS_ERROR_EX &e) {
        e.addr = addr;
        e.tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        e.size = SZ::WORD;
        e.lk = regs.ea_rw;
        e.rw = false;
        throw e;
    }
}
void write32(uint32_t addr, uint32_t v) {
    MyFinally e = []() { regs.ea_rw = false; };
    try {
        if(addr & 3) {
            write16(addr, v >> 16);
            try {
                write16(addr + 2, v & 0xffff);
            } catch(BUS_ERROR_EX &e) {
                e.ma = true;
                throw e;
            }
        } else {
            b_write32(mmu_d.do_mmu(addr, true, regs.S), v);
        }
    } catch(BUS_ERROR_EX &e) {
       e.addr = addr;
        e.tm = regs.S ? TM::SUPER_DATA : TM::USER_DATA;
        e.size = SZ::LONG;
        e.lk = regs.ea_rw;
        e.rw = false;
        throw e;
    }
}
