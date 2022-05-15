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
extern uint8_t *ROMBaseHost;
extern std::vector<std::byte> RAM;

constexpr presult MMU_BUS_ERROR{false, false, false, false, 0,
                                false, 0,     false, true,  0};

constexpr presult MMU_BUS_INALID{false, false, false, false, 0,
                                 false, 0,     false, false, 0};

struct atc_entry_t {
    uint32_t addr : 20;
    bool R : 1;
    bool W : 1;
    bool M : 1;
    uint8_t cm : 2;
    bool S : 1;
    uint8_t U : 2;
};
constexpr atc_entry_t ATC_INVALID{0, false, false, false, 0, false, 0};
// MMU enabled access
presult test_TTR(const uint32_t addr, bool is_code, bool rw) {
    const m68k_reg::ttc_t *ttr = is_code ? regs.ITTR : regs.DTTR;
    uint8_t base = addr >> 24;
    for(int i = 0; i < 2; ++i) {
        if(ttr[i].E &&
           ((base & ~ttr->address_mask) ==
            (ttr[i].address_base & ~ttr->address_mask)) &&
           ((ttr[i].S & 2) || (ttr[i].S == !!regs.S))) {
            uint32_t raddr = base << 12 | ((addr >> 12) & 0xfff);
            return {true,          true,   ttr->W, false, ttr->CM,
                    ttr[i].S == 1, ttr->U, false,  false, raddr};
        }
    }
    return MMU_BUS_INALID;
}

std::unordered_map<uint32_t, atc_entry_t> atc_gcache, atc_lcache;
presult ptest(const uint32_t addr, bool rw, bool s) {
    try {
        uint32_t ri = (addr >> 25) << 2;
        uint32_t pi = (addr >> 18 & 0x7f) << 2;
        uint32_t pgi, atc_key = s << 31;
        if(regs.tcr_p) {
            pgi = ((addr >> 13) & 0x1f) << 2;
            atc_key |= addr >> 13;
        } else {
            pgi = ((addr >> 12) & 0x3f) << 2;
            atc_key |= addr >> 12;
        }
        atc_entry_t *entry;
        bool wp = false;
        bool g;
        if(auto gi = atc_gcache.find(atc_key); gi != atc_gcache.end()) {
            entry = &gi->second;
            g = true;
        } else if(auto li = atc_lcache.find(atc_key); li != atc_lcache.end()) {
            entry = &li->second;
            g = false;
        } else {
            uint32_t mst = s ? regs.srp : regs.urp;
            uint32_t rtd = b_read32(mst | ri);
            if(!(rtd & 2)) {
                atc_lcache[atc_key] = ATC_INVALID;
                goto INVALID;
            }
            if(rtd & 4) {
                wp = true;
            }
            b_write32(mst | ri, rtd |= 8);
            uint32_t ptd_addr = (rtd & ~0x1ff) | pi;
            uint32_t ptd = b_read32(ptd_addr);
            if(!(ptd & 2)) {
                atc_lcache[atc_key] = ATC_INVALID;
                goto INVALID;
            }
            b_write32(ptd_addr, ptd |= 8);
            if(ptd & 4) {
                wp = true;
            }
            uint32_t pd_addr = (ptd & ~(regs.tcr_p ? 0x3f : 0x7f)) | pgi;
            uint32_t pd = b_read32(pd_addr);
            switch(pd & 3) {
            case 0:
                goto INVALID;
            case 2:
                pd_addr = pd & ~3;
                pd = b_read32(pd_addr);
                if((pd & 1) == 0) {
                    goto INVALID;
                }
            }
            pd |= 8;
            if(pd & 4) {
                wp = true;
            }
            bool m = pd & 0x10;
            if(rw && !wp && !((pd & 0x80) && !s)) {
                pd |= 0x10;
                m = true;
            }
            b_write32(pd_addr, pd);
            atc_entry_t ent{
                pd >> (regs.tcr_p ? 13 : 12),
                true,
                wp,
                m,
                uint8_t(pd >> 5 & 3),
                bool(pd & 0x80),
                uint8_t(pd >> 8 & 3),
            };
            g = pd >> 10 & 1;
            if(g) {
                atc_gcache[atc_key] = ent;
                entry = &atc_gcache[atc_key];
            } else {
                atc_lcache[atc_key] = ent;
                entry = &atc_lcache[atc_key];
            }
        }
        uint32_t xaddr = entry->addr;
        if(regs.tcr_p) {
            xaddr = (xaddr << 1) | (addr & 1);
        }
        return {true,     false,    wp, entry->M, entry->cm,
                entry->S, entry->U, g,  false,    xaddr};
    } catch(BUS_ERROR_EX &) {
        return MMU_BUS_ERROR;
    }
INVALID:
    return MMU_BUS_INALID;
}
uint32_t do_mmu(uint32_t vaddr, bool code, bool rw, bool s) {
    presult ttr_result = test_TTR(vaddr, code, rw);
    if(ttr_result.T) {
        if(ttr_result.W && rw) {
            throw BUS_ERROR_EX{};
        }
        return (ttr_result.addr << 12) | (vaddr & 0xfff);
    }
    if(!regs.tcr_e) {
        return vaddr;
    }

    presult result = ptest(vaddr, rw, s);
    if(result.B) {
        // BUS ERROR during table search
        throw BUS_ERROR_EX{true};
    }
    if(!result.R) {
        // no entry for the address
        throw BUS_ERROR_EX{true};
    }
    // Supervisor table
    if(result.S && !s) {
        throw BUS_ERROR_EX{true};
    }

    // readonly table
    if(result.W && rw) {
        throw BUS_ERROR_EX{true};
    }

    return (result.addr << 12) | (vaddr & 0xfff);
}
uint8_t read8(uint32_t addr, bool imem) {
    try {
        return b_read8(do_mmu(addr, imem, false, regs.S));
    } catch(BUS_ERROR_EX &e) {
        TM tm = TM(int(regs.S ? TM::SUPER_DATA : TM::USER_DATA) + (imem ? 1 : 0));
        paddr pe{addr, 0, SZ::BYTE, TT::NORMAL, tm, false};
        BUSERROR(pe, e.atc);
    }
}
uint16_t read16(uint32_t addr, bool imem) {
    try {
        return b_read16(do_mmu(addr, imem, false, regs.S));
    } catch(BUS_ERROR_EX &e) {
        TM tm = TM(int(regs.S ? TM::SUPER_DATA : TM::USER_DATA) + (imem ? 1 : 0));
        paddr pe{addr, 0, SZ::WORD, TT::NORMAL, tm, false};
        BUSERROR(pe, e.atc);
    }
}

uint16_t FETCH() {
    uint16_t v = read16(regs.pc, true);
    regs.pc += 2;
    return v;
}
uint32_t read32(uint32_t addr, bool imem) {
    try {
        uint32_t v = b_read32(do_mmu(addr, imem, false, regs.S));
        return v;
    } catch(BUS_ERROR_EX &e) {
        TM tm = TM(int(regs.S ? TM::SUPER_DATA: TM::USER_DATA ) + (imem ? 1 : 0));
        paddr pe{addr, 0, SZ::LONG, TT::NORMAL, tm, false};
        BUSERROR(pe, e.atc);
    }
}
uint32_t FETCH32() {
    uint32_t v = read32(regs.pc, true);
    regs.pc += 4;
    return v;
}
void write8(uint32_t addr, uint8_t v) {
    try {
        b_write8(do_mmu(addr, false, true, regs.S), v);
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_DATA: TM::USER_DATA ;
        paddr pe{addr, 0, SZ::BYTE, TT::NORMAL, tm, true};
        BUSERROR(pe, e.atc);
    }
}
void write16(uint32_t addr, uint16_t v) {
    try {
        b_write16(do_mmu(addr, false, true, regs.S), v);
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_DATA: TM::USER_DATA ;
        paddr pe{addr, 0, SZ::WORD, TT::NORMAL, tm, true};
        BUSERROR(pe, e.atc);
    }
}
void write32(uint32_t addr, uint32_t v) {
    try {
        b_write32(do_mmu(addr, false, true, regs.S), v);
    } catch(BUS_ERROR_EX &e) {
        TM tm = regs.S ? TM::SUPER_DATA: TM::USER_DATA ;
        paddr pe{addr, 0, SZ::LONG, TT::NORMAL, tm, true};
        BUSERROR(pe, e.atc);
    }
}

using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536 >> 6];
// CINV( used only for jit)
OP(cinv_push_dc) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
//    uint32_t addr = regs.a[reg];
    switch(type) {
    case 1:
        break; // CINVL DC, (AN)
    case 2:
        break; // CINVP DC, (AN)
    case 3:
        break; // CINVA DC
    case 5:
        break; // CPUSHL DC, (AN)
    case 6:
        break; // CPUSHP DC, (AN)
    case 7:
        break; // CPUSHA DC
    default:
        ILLEGAL_INST();
    }
}

OP(cinv_push_ic) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
//    uint32_t addr = regs.a[reg];
    switch(type) {
    case 1:
        break; // CINVL IC, (AN)
    case 2:
        break; // CINVP IC, (AN)
    case 3:
        break; // CINVA IC
    case 5:
        break; // CPUSHL IC, (AN)
    case 6:
        break; // CPUSHP IC, (AN)
    case 7:
        break; // CPUSHA IC
    default:
        ILLEGAL_INST();
    }
}

OP(cinv_push_bc) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
//    uint32_t addr = regs.a[reg];
    switch(type) {
    case 1:
        break; // CINVL BC, (AN)
    case 2:
        break; // CINVP BC, (AN)
    case 3:
        break; // CINVA BC
    case 5:
        break; // CPUSHL BC, (AN)
    case 6:
        break; // CPUSHP BC, (AN)
    case 7:
        break; // CPUSHA BC
    default:
        ILLEGAL_INST();
    }
}

OP(pflush) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    uint32_t addr = regs.a[reg];
    uint32_t atc_key;
    if(regs.tcr_p) {
        atc_key = addr >> 13;
    } else {
        atc_key = addr >> 12;
    }
    switch(type) {
    case 0: // pflushn (an)
        if(regs.dfc == 1 || regs.dfc == 2) {
            atc_lcache.erase(atc_key);
        } else {
            atc_lcache.erase(0x80000000 | atc_key);
        }
        break;
    case 1: // pflush (an)
        if(regs.dfc == 1 || regs.dfc == 2) {
            atc_gcache.erase(atc_key);
            atc_lcache.erase(atc_key);
        } else {
            atc_gcache.erase(0x80000000 | atc_key);
            atc_lcache.erase(0x80000000 | atc_key);
        }
        break;
    case 2: // pflushan
        atc_lcache.clear();
        break;
    case 3: // plusha
        atc_gcache.clear();
        atc_lcache.clear();
        break;
    default:
        ILLEGAL_INST();
    }
}

OP(ptest) {
    if(!regs.S) {
        PRIV_ERROR();
        return;
    }
    uint32_t addr = regs.a[reg];
    bool rw;
    if(type == 1) {
        rw = false;
    } else if(type == 5) {
        rw = true;
    } else {
        ILLEGAL_INST();
    }
    presult result = test_TTR(addr, !(regs.dfc & 1), rw);
    if(!result.T) {
        result = ptest(addr, rw, true);
    }
    regs.MMUSR.PA = result.addr << 12;
    regs.MMUSR.B = result.B;
    regs.MMUSR.G = result.G;
    regs.MMUSR.U = result.U;
    regs.MMUSR.S = result.S;
    regs.MMUSR.CM = result.CM;
    regs.MMUSR.M = result.M;
    regs.MMUSR.W = result.W;
    regs.MMUSR.T = result.T;
    regs.MMUSR.R = result.R;
}

void init_mmu_opc() {
    opc_map[01721] = op_cinv_push_dc;
    opc_map[01722] = op_cinv_push_ic;
    opc_map[01723] = op_cinv_push_bc;
    opc_map[01724] = op_pflush;
    opc_map[01725] = op_ptest;
}
