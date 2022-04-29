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
#include <vector>
#include <unordered_map>
extern uint8 *ROMBaseHost;
extern std::vector<uint8_t> RAM;

TM tm_mmu_code(bool c) { return c ? TM::MMU_CODE : TM::MMU_DATA; }
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
presult test_TTR(const paddr &vaddr) {
    TM tm = TM(int(vaddr.tm) & 7);
    bool is_code =
        tm == TM::USER_CODE || tm == TM::MMU_CODE || tm == TM::SUPER_CODE;
    bool is_super = vaddr.tm != TM::USER_CODE && vaddr.tm != TM::USER_DATA;
    const m68k_reg::ttc_t *ttr = is_code ? regs.ITTR : regs.DTTR;
    uint8_t base = vaddr.addr >> 24;
    for(int i = 0; i < 2; ++i) {
        if(ttr[i].E && (base == ttr[i].address_base) &&
           ((ttr[i].S & 2) || (ttr[i].S == !!is_super))) {
            uint32_t raddr = (base & ~ttr->address_mask) << 12 |
                             ((vaddr.addr >> 12) & 0xfff);
            return {true,          true,   ttr->W, false, ttr->CM,
                    ttr[i].S == 1, ttr->U, false,  false, raddr};
        }
    }
    return MMU_BUS_INALID;
}

std::unordered_map<uint32_t, atc_entry_t> atc_gcache, atc_lcache;
inline TM code_tm() { return regs.S ? TM::SUPER_CODE : TM::USER_CODE; }
inline TM data_tm() { return regs.S ? TM::SUPER_DATA : TM::USER_DATA; }
presult ptest(const paddr &vaddr) {
    TM tm = TM(int(vaddr.tm) & 7);
    bool is_super = tm != TM::USER_CODE && tm != TM::USER_DATA;

    uint32_t ri = (vaddr.addr >> 25) << 2;
    uint32_t pi = (vaddr.addr >> 18 & 0x7f) << 2;
    uint32_t pgi, atc_key = is_super << 31;
    if(regs.tcr_p) {
        pgi = ((vaddr.addr >> 13) & 0x1f) << 2;
        atc_key |= vaddr.addr >> 13;
    } else {
        pgi = ((vaddr.addr >> 12) & 0x3f) << 2;
        atc_key |= vaddr.addr >> 12;
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
        uint32_t mst = is_super ? regs.srp : regs.urp;
        uint32_t rtd;
        if(!b_read32(mst | ri, &rtd)) {
            return MMU_BUS_ERROR;
        }
        if(!(rtd & 2)) {
            atc_lcache[atc_key] = ATC_INVALID;
            return MMU_BUS_INALID;
        }
        if(rtd & 4) {
            wp = true;
        }
        rtd |= 8;
        if(!b_write32(mst | ri, &rtd)) {
            return MMU_BUS_ERROR;
        }
        uint32_t ptd_addr = (rtd & ~0x1ff) | pi;
        uint32_t ptd;
        if(!b_read32(ptd_addr, &ptd)) {
            return MMU_BUS_ERROR;
        }
        if(!(ptd & 2)) {
            atc_lcache[atc_key] = ATC_INVALID;
            return MMU_BUS_INALID;
        }
        ptd |= 8;
        if(!b_write32(ptd_addr, &ptd)) {
            return MMU_BUS_ERROR;
        }
        if(ptd & 4) {
            wp = true;
        }
        uint32_t pd_addr = (ptd & ~(regs.tcr_p ? 0x3f : 0x7f)) | pgi;
        uint32_t pd;
        if(!b_read32(pd_addr, &pd)) {
            return MMU_BUS_ERROR;
        }
        switch(pd & 3) {
        case 0:
            return MMU_BUS_INALID;
        case 2:
            pd_addr = pd & ~3;
            if(!b_read32(pd_addr, &pd)) {
                return MMU_BUS_ERROR;
            }
            if((pd & 1) == 0) {
                return MMU_BUS_INALID;
            }
        }
        pd |= 8;
        if(pd & 4) {
            wp = true;
        }
        bool m = pd & 0x10;
        if(vaddr.rw && !wp && !((pd & 0x80) && !is_super)) {
            pd |= 0x10;
            m = true;
        }
        if(!b_write32(pd_addr, &pd)) {
            return MMU_BUS_ERROR;
        }
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
        xaddr = (xaddr << 1) | (vaddr.addr & 1);
    }
    return {true,     false,    wp, entry->M, entry->cm,
            entry->S, entry->U, g,  false,    xaddr};
}
uint32_t do_mmu(const paddr &vaddr) {
    TM tm = TM(int(vaddr.tm) & 7);
    bool is_super = tm != TM::USER_CODE && tm != TM::USER_DATA;

    presult ttr_result = test_TTR(vaddr);
    if(ttr_result.T) {
        if(ttr_result.W && vaddr.rw) {
            BUSERROR(vaddr, true);
        }
        return (ttr_result.addr << 12) | (vaddr.addr & 0xfff);
    }
    if(!regs.tcr_e) {
        return vaddr.addr;
    }

    presult result = ptest(vaddr);
    if(result.B) {
        // BUS ERROR during table search
        BUSERROR(vaddr, true);
    }
    if(!result.R) {
        // no entry for the address
        BUSERROR(vaddr, true);
    }
    // Supervisor table
    if(result.S && !is_super) {
        BUSERROR(vaddr, true);
    }

    // readonly table
    if(result.W && vaddr.rw) {
        BUSERROR(vaddr, true);
    }

    return (result.addr << 12) | (vaddr.addr & 0xfff);
}
uint8_t read8(uint32_t addr, bool imem) {
    uint8_t v;
    TM tm = imem ? code_tm() : data_tm();
    paddr pe{addr, 0, SZ::BYTE, TT::NORMAL, tm, false};
    uint32_t r = do_mmu(pe);
    if(!b_read8(r, &v)) {
        BUSERROR(pe);
    }
    return v;
}
uint16_t read16(uint32_t addr, bool imem) {
    uint16_t v;
    TM tm = imem ? code_tm() : data_tm();
    paddr pe{addr, 0, SZ::WORD, TT::NORMAL, tm, false};
    uint32_t r = do_mmu(pe);
    if(!b_read16(r, &v)) {
        BUSERROR(pe);
    }
    return v;
}

uint16_t FETCH() {
    uint16_t v = read16(regs.pc, true);
    regs.pc += 2;
    return v;
}
uint32_t read32(uint32_t addr, bool imem) {
    uint32_t v;
    TM tm = imem ? code_tm() : data_tm();
    paddr pe{addr, 0, SZ::LONG, TT::NORMAL, tm, false};
    uint32_t r = do_mmu(pe);
    if(!b_read32(r, &v)) {
        BUSERROR(pe);
    }
    return v;
}
uint32_t FETCH32() {
    uint32_t v = read32(regs.pc, true);
    regs.pc += 4;
    return v;
}
void write8(uint32_t addr, uint8_t v) {
    paddr pe{addr, 0, SZ::BYTE, TT::NORMAL, data_tm(), true};
    uint32_t r = do_mmu(pe);
    if(!b_write8(r, &v)) {
        BUSERROR(pe, regs.i_eav);
    }
}
void write16(uint32_t addr, uint16_t v) {
    paddr pe{addr, 0, SZ::WORD, TT::NORMAL, data_tm(), true};
    uint32_t r = do_mmu(pe);
    if(!b_write16(r, &v)) {
        BUSERROR(pe, regs.i_eav);
    }
}
void write32(uint32_t addr, uint32_t v) {
    paddr pe{addr, 0, SZ::LONG, TT::NORMAL, data_tm(), true};
    uint32_t r = do_mmu(pe);
    if(!b_write32(r, &v)) {
        BUSERROR(pe, regs.i_eav);
    }
}

uint8_t readIO8(uint32_t addr) { return 0; }
uint16_t readIO16(uint32_t addr) { return 0; }
uint32_t readIO32(uint32_t addr) { return 0; }
void writeIO8(uint32_t addr, uint8_t v) {}
void writeIO16(uint32_t addr, uint16_t v) {}
void writeIO32(uint32_t addr, uint32_t v) {}
using op_t = void (*)(uint16_t, int, int, int);
extern op_t opc_map[65536];
// CINV( used only for jit)
OP(cinvl_na) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinvl_d) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinvl_i) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinvl_both) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinvp_na) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinvp_d) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinvp_i) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinvp_both) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinva_na) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinva_d) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinva_i) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cinva_both) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}

// CPUSH( use for only jit)
OP(cpushl_na) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpushl_d) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpushl_i) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpushl_both) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpushp_na) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpushp_d) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpushp_i) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpushp_both) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpusha_na) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpusha_d) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpusha_i) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}
OP(cpusha_both) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}

OP(pflush) {
    if(!regs.S) {
        PRIV_ERROR();
    }
    atc_gcache.clear();
    atc_lcache.clear();
}

OP(pflushn) {
    if(!regs.S) {
        PRIV_ERROR();
    }
    atc_lcache.clear();
}

OP(pflusha) {
    if(!regs.S) {
        PRIV_ERROR();
    }
    uint32_t addr = regs.a[reg];
    uint32_t atc_key;
    if(regs.tcr_p) {
        atc_key = addr >> 13;
    } else {
        atc_key = addr >> 12;
    }
    if(regs.dfc == 1 || regs.dfc == 2) {
        atc_gcache.erase(atc_key);
        atc_lcache.erase(atc_key);
    } else {
        atc_gcache.erase(0x80000000 | atc_key);
        atc_lcache.erase(0x80000000 | atc_key);
    }
}

OP(pflushan) {
    if(!regs.S) {
        PRIV_ERROR();
    }
    uint32_t addr = regs.a[reg];
    uint32_t atc_key;
    if(regs.tcr_p) {
        atc_key = addr >> 13;
    } else {
        atc_key = addr >> 12;
    }
    if(regs.dfc == 1 || regs.dfc == 2) {
        atc_lcache.erase(atc_key);
    } else {
        atc_lcache.erase(0x80000000 | atc_key);
    }
}

OP(ptestr) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}

OP(ptestw) {
    if(!regs.S) {
        PRIV_ERROR();
    }
}

void init_mmu_opc() {
    for(int i = 0; i < 8; ++i) {
        opc_map[0172010 | i] = op_cinvl_na;
        opc_map[0172110 | i] = op_cinvl_d;
        opc_map[0172210 | i] = op_cinvl_i;
        opc_map[0172310 | i] = op_cinvl_both;
        opc_map[0172020 | i] = op_cinvp_na;
        opc_map[0172120 | i] = op_cinvp_d;
        opc_map[0172220 | i] = op_cinvp_i;
        opc_map[0172320 | i] = op_cinvp_both;
        opc_map[0172030 | i] = op_cinva_na;
        opc_map[0172130 | i] = op_cinva_d;
        opc_map[0172230 | i] = op_cinva_i;
        opc_map[0172330 | i] = op_cinva_both;

        opc_map[0172050 | i] = op_cpushl_na;
        opc_map[0172150 | i] = op_cpushl_d;
        opc_map[0172250 | i] = op_cpushl_i;
        opc_map[0172350 | i] = op_cpushl_both;
        opc_map[0172060 | i] = op_cpushp_na;
        opc_map[0172160 | i] = op_cpushp_d;
        opc_map[0172260 | i] = op_cpushp_i;
        opc_map[0172360 | i] = op_cpushp_both;
        opc_map[0172070 | i] = op_cpusha_na;
        opc_map[0172170 | i] = op_cpusha_d;
        opc_map[0172270 | i] = op_cpusha_i;
        opc_map[0172370 | i] = op_cpusha_both;

        opc_map[0172400 | i] = op_pflushn;
        opc_map[0172410 | i] = op_pflush;
        opc_map[0172420 | i] = op_pflushan;
        opc_map[0172430 | i] = op_pflusha;

        opc_map[0142510 | i] = op_ptestw;
        opc_map[0142550 | i] = op_ptestr;
    }
}
